//
//  Movable.cpp
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 6. 26..
//
//
#include "Movable.h"
#include "GameManager.h"
#include "LanguageManager.h"
//bool Movable::init(){}
Movable::Movable()
{
    ignoreGravity = false;
    waterAccelX = 100;
    waterMaxX = 50;
    waterAccelYN = 100;
    waterAccelYP = 90;
    waterMinY = 60;
    inWater = false;
    isTowardLeft = false;
    isFregile = false;
    exitingVehicle = false;
//    state = STATE_STANDING;
    xSpeed = 0;
    ySpeed = 0;
    velocity = Point(0,-100);
    jumped = false;
    blinkingTime = 0;
    freezed = false;
    immortal = false;
    centerPosition = Point(0,0);
    
    untouchable = false;
    onGround = true;
//    wantsToStopHorizontal = false;
//    wantsToStopVertical = false;
    wantsToGoLeft = false;
    wantsToGoRight = false;
    wantsToJump = false;
    weight = 1;
    target = NULL;
    effectType = 0;
    iWasInWater = false;
//    startSkillAura(AURA_LIGHTNING); // test
//    legAniName = NULL;
//    isTowardLeft = true;
}
void Movable::init(int unit, int eng, float extraSpd, const char* sptName)
{
    if (unit == UNIT_HERO_ORC) {
        spine = spine::SkeletonAnimation::createWithJsonFile("orc.json", "orc.atlas", 1);
        this->addChild(spine);
        spine->setAnimation(0, "idle", true);
        spine->setPosition(Vec2(60, 15));
        this->Sprite::initWithFile("shadow.png");
    }else{
        this->Sprite::initWithSpriteFrameName(sptName);
    }
    setAnchorPoint(Vec2(0.5, 14/getBoundingBox().size.height));
    this->getTexture()->setAliasTexParameters();
    
    this->sptName = sptName;
    this->energy = eng;
    this->maxEnergy = eng;
    this->setTag(unit);
    this->extraSpeed = extraSpd;
    this->weight = 1;
    this->jumpAllowed = false;
    this->secondTag = -1;
    unitType = unit;
}
void Movable::onDead(){
    
}
void Movable::onHit(){
    
}
void Movable::showTalk(std::string text, float time, float y, float delay){
    talkBalloonY = y;
    if (imgTalkBalloon != nullptr) {
        imgTalkBalloon->removeFromParentAndCleanup(true);
    }
    imgTalkBalloon = ImageView::create("talkBalloonEmpty.png");
    imgTalkBalloon->setAnchorPoint(Point(0.5, 0));
    imgTalkBalloon->setScale9Enabled(true);
    imgTalkBalloon->setCapInsets(Rect(12, 4, 1, 4));
    imgTalkBalloon->setOpacity(0);
    imgTalkBalloon->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0), NULL));
    
    lblTalk = Label::createWithTTF(text, GameManager::getInstance()->getFont(FONT_VISITOR), 36);
    imgTalkBalloon->addChild(lblTalk);
    lblTalk->setColor(Color3B(104, 85, 84));
    lblTalk->setScale(0.3);
    lblTalk->setOpacity(0);
    lblTalk->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0), NULL));
    float gap = 3;
    lblTalk->setPosition(Point(lblTalk->getBoundingBox().size.width/2 + gap, imgTalkBalloon->getBoundingBox().size.height/2 + 1.5));
    float width = lblTalk->getBoundingBox().size.width + gap*2;
    if (width < imgTalkBalloon->getContentSize().width) {
        width = imgTalkBalloon->getContentSize().width;
    }
    imgTalkBalloon->setContentSize(Size(width, imgTalkBalloon->getContentSize().height));
    
    GameManager::getInstance()->getWorld()->addChild(imgTalkBalloon, 10000);
    talkVisibleTime = time + delay;
    talkVisibleTimeElapsed = time + delay;
    this->schedule(schedule_selector(Movable::updateTalkBalloon), 0.07f, CC_REPEAT_FOREVER, delay);
}
void Movable::updateTalkBalloon(float dt){
    talkVisibleTimeElapsed -= dt;
    if (talkVisibleTimeElapsed < 0) {
        int opacity = imgTalkBalloon->getOpacity();
        opacity -= 10;
        imgTalkBalloon->setOpacity(opacity);
        lblTalk->setOpacity(opacity);
        if (opacity <= 0) {
            imgTalkBalloon->removeFromParentAndCleanup(true);
            imgTalkBalloon = nullptr;
            this->unschedule(schedule_selector(Movable::updateTalkBalloon));
            return;
        }
    }
    imgTalkBalloon->setPosition(this->getPosition() + Point(0, talkBalloonY));
}
void Movable::dontHurt(){
    isHurting = false;
}
float Movable::getCurrentY(){
    return getPositionY();
}
Movable* Movable::createMovable(int unit, int eng, float extraSpd, const char* sptName)
{
    Movable* drop = new Movable();
    drop->init(unit, eng, extraSpd, sptName);
    return drop;
}

Movable* Movable::createMovable(int unit, int eng, float extraSpd, const char* sptName, const char* animationName)
{
    return Movable::createMovable(unit, eng, extraSpd, sptName, animationName, false);
}

Movable* Movable::createMovable(int unit, int eng, float extraSpd, const char* sptName, const char* animationName, bool repeat)
{
    Movable* drop = Movable::createMovable(unit, eng, extraSpd, sptName);
    
    drop->runAnimation(animationName, repeat);
    return drop;
}

void Movable::setFlippedX(bool flip){
    Sprite::setFlippedX(flip);
    isTowardLeft = !flip;
    if(spine){
        spine->setScaleX(flip?-1:1);
    }
}
void Movable::encounteredCliff(int direction){}
void Movable::encounteredWall(){}
void Movable::onLanded(){
    onGround = true;
    isDropping = false;
    tumbled = false;
    jumped = false;
    this->stopActionByTag(ACTION_TAG_ROTATION);
    setRotation(0);
    if (!inWater) {
        iWasInWater = false;
    }
}
void Movable::updatePosition(float dt)
{
    bool toLeft = wantsToGoLeft;
    bool toRight = wantsToGoRight;
    bool toUp = wantsToGoUp;
    bool toDown = wantsToGoDown;
    
    if (onGround) {
        exitingVehicle = false;
        jumped = false;
//        if (state == STATE_JUMPING) {
//            state = STATE_STANDING;
//            onLanded();
//            return;
//        }
    }
    
    if (wantsToGoLeft && !isTowardLeft) {
        setFlippedX(false);
    }else if(wantsToGoRight && isTowardLeft){
        setFlippedX(true);
    }
    
    Point vlct;
    
    if (this->freezed) {
        vlct = Point(inWater?vlct.x*0.9:vlct.x * (freezed?0.95:0.30), vlct.y);
    }else{
//        if (wantsToJump){
//            if(inWater || (!jumped && (onGround || jumpAllowed))) {
//                Point jumpForce = Point(0.0, inWater?(onGround?waterAccelYP+10:waterAccelYP):1850.0);
//                vlct = vlct + jumpForce;
//                jumped = true;
//                onGround = false;
//                wantsToJump = false;
//                onJumped();
//
//            }else if (doubleJump && !tumbled && (jumped || !onGround) && !iWasInWater) {
//                Point jumpForce = Point(0.0, inWater?(onGround?waterAccelYP+60:waterAccelYP):1850.0);
//                vlct = Point(vlct.x, 230);
//                tumbled = true;
//                wantsToJump = false;
//
//                tumbleAnimation();
//            }
//        }

        float horizontalMove = 0;
        float verticalMove = 0;
//        float speed = 2800;
        float speed = extraSpeed*100;
        if (toLeft) {
            horizontalMove = inWater?-waterAccelX:-speed;
        }else if(toRight){
            horizontalMove = inWater?waterAccelX:speed;
        }else{
            horizontalMove = 0;
        }
        if(toDown){
            verticalMove = inWater?-waterAccelX:-speed;
        }else if(toUp){
            verticalMove = inWater?waterAccelX:speed;
        }else{
            verticalMove = 0;
        }
        
        vlct = vlct + Point(horizontalMove*dt, verticalMove*dt);
    }
    
//    Point minMovement = Point(inWater?-waterMaxX:-50.0-extraSpeed, inWater?-waterMinY:-280.0 - extraJump);
//    Point maxMovement = Point(inWater?waterMaxX:50.0+extraSpeed, 280.0 + extraJump);
    Point minMovement = Point(inWater?-waterMaxX:-280.0, inWater?-waterMinY:-280.0 - extraJump);
    Point maxMovement = Point(inWater?waterMaxX:280.0, 280.0 + extraJump);
    
    // Clamp allows you limit a hero's speed while Moving
    vlct = vlct.getClampPoint(minMovement, maxMovement);
    Point stepVelocity = vlct * dt;
    //this->setPosition(ccpAdd(getPosition(), stepVelocity));
    desiredPosition = getPosition() + stepVelocity;
    
    this->velocity = vlct;
}
void Movable::freezeForAWhile(float dur){
    freezed = true;
    this->runAction(Sequence::create(DelayTime::create(dur), CallFunc::create(CC_CALLBACK_0(Movable::releaseFreezing, this)), nullptr));
}
void Movable::releaseFreezing(){
    freezed = false;
    if (fixAngleToTargetWhenReleaseFreezing) {
        movingAngle = -(-GameManager::getInstance()->getAngle(this->getPosition(), this->target->getPosition() + Point(0, TILE_SIZE/2)) + 180);
    }
}
void Movable::onDropping(){
    isDropping = true;
}
void Movable::banJump(float dt){
    jumpAllowed = false;
}
void Movable::setOnGround(bool ground){
    if (onGround && !ground) {
        jumpAllowed = true;
        this->scheduleOnce(schedule_selector(Movable::banJump), 0.06f);
    }
    onGround = ground;
    
    
}
void Movable::tumbleAnimation()
{
//    state = STATE_JUMPING;
//    Action* action = RotateBy::create(0.5, isTowardLeft?-360:360);
//    action->setTag(ACTION_TAG_ROTATION);
//    this->runAction(action);
}

void Movable::onJumped(){
    
}
void Movable::updatePositionForStraight(float dt)
{
    Point vlct;
    float baseSpeed = 50;
    
    vlct = Point((baseSpeed+extraSpeed)*cos(movingAngle*3.14/180),
                 (baseSpeed+extraSpeed)*sin(movingAngle*3.14/180));
    
    Point minMovement = Point(-baseSpeed-extraSpeed, -280.0);
    Point maxMovement = Point(baseSpeed+extraSpeed, 280.0);
    
    // Clamp allows you limit a hero's speed while Moving
    vlct = vlct.getClampPoint(minMovement, maxMovement);
    Point stepVelocity = vlct * dt;

    //this->setPosition(ccpAdd(getPosition(), stepVelocity));
    if (freezed) {
        stepVelocity = Point::ZERO;
    }
    desiredPosition = getPosition() + stepVelocity;
    this->velocity = vlct;
}

void Movable::updatePositionToFlyToFollowTarget(float dt)
{
//    baseSpeed+extraSpeed
    Point tPos = target->getPosition();
    Point pos = getPosition();
    float diff = sqrtf((tPos.x - pos.x)*(tPos.x-pos.x)+(tPos.y - pos.y)*(tPos.y - pos.y));
    float toMove = dt*50 + dt*extraSpeed;
    
    float toX = (tPos.x - pos.x)*(toMove/diff);
    float toY = (tPos.y - pos.y)*(toMove/diff);
    desiredPosition = Point(pos.x + toX, pos.y + toY);
    velocity = Point(0, this->getPositionY() >= desiredPosition.y?-1:1);
}

void Movable::updatePositionForBouncing(float dt)
{
    Point vlct;
    float baseSpeed = 50;
    
    Point gravity = Point(0.0, -850.0);
    if (onGround) {
        gravity = Point(0.0, 0);
//        if (state == STATE_JUMPING) {
//            state = STATE_STANDING;
//        }
    }
    Point gravityStep = gravity * dt;
    
    vlct = this->velocity + gravityStep;
    
    Point minMovement = Point(-baseSpeed-extraSpeed, -280.0);
    Point maxMovement = Point(baseSpeed+extraSpeed, 280.0);
    
    vlct = vlct.getClampPoint(minMovement, maxMovement);
    Point stepVelocity = vlct * dt;
    
    desiredPosition = getPosition() + stepVelocity;
    this->velocity = vlct;
}

Rect Movable::collectBoundingBox()
{
    return Rect(desiredPosition.x - TILE_SIZE/2, desiredPosition.y - getContentSize().height/2, TILE_SIZE, TILE_SIZE*2);
}
Rect Movable::collisionBoundingBox()
{
    float width = getContentSize().width/2;
    float height = getContentSize().height*getScaleY();
    if (width <= TILE_SIZE/2) {
        width = TILE_SIZE/2;
    }
    if (height <= 0) {
        height = TILE_SIZE/2;
    }
//    return Rect(desiredPosition.x - TILE_SIZE/2, desiredPosition.y - getContentSize().height/2, width, height);//TILE_SIZE, TILE_SIZE);
    return Rect(desiredPosition.x - width/2, desiredPosition.y - height/2, width, TILE_SIZE/2);
    Rect bounding = boundingBox();
   
    if (bounding.size.width < TILE_SIZE*2 && bounding.size.height < TILE_SIZE*2) {
        Rect collisionBox = RectInset(bounding, 1, 0);
        Point diff = desiredPosition - getPosition();
        Rect returnBoundingBox = RectOffset(collisionBox, diff.x, diff.y);
        return returnBoundingBox;
    }else{
        Point pos = getPosition();
        Rect rect = Rect(pos.x - TILE_SIZE/2, pos.y - getContentSize().height/2, TILE_SIZE, getContentSize().height);
        Rect collisionBox = RectInset(rect, 0, 0);
        Point diff = desiredPosition - getPosition();
        Rect returnBoundingBox = RectOffset(collisionBox, diff.x, diff.y);
        return returnBoundingBox;
    }
}
Rect Movable::damageBoundingBox()
{
    Rect bounding = getBoundingBox();
    Rect collisionBox = RectInset(bounding, 3, 3);
//    Point diff = 0;//desiredPosition - getPosition();
//    Rect returnBoundingBox = RectOffset(collisionBox, diff.x, diff.y);
//    return returnBoundingBox;
    return collisionBox;
}

Point Movable::getGravityPosition()
{
    return Point(getPosition().x, getPosition().y-getContentSize().height/2 + TILE_SIZE/2);
}

/*Rect Movable::currentBoundingBox()
{
    Rect bounding = boundingBox();
    
    if (vehicleType == VEHICLE_GOLIATH) {
        
        Point pos = getPosition();
        Rect collisionBox = Rect(pos.x - TILE_SIZE, pos.y - 28, TILE_SIZE*2, TILE_SIZE*2);
        return collisionBox;
    }
    
    if (bounding.size.width < TILE_SIZE*2 && bounding.size.height < TILE_SIZE*2) {
        Rect collisionBox = RectInset(bounding, 0, 0);
//        Point diff = ccpSub(desiredPosition, getPosition());
//        Rect returnBoundingBox = RectOffset(collisionBox, diff.x, diff.y);
        return collisionBox;
    }else{
        Point pos = getPosition();
        Rect rect = Rect(pos.x - TILE_SIZE/2, pos.y - TILE_SIZE + 1, TILE_SIZE, TILE_SIZE*2 - 2);
        Rect collisionBox = RectInset(rect, 0, 0);
//        Point diff = ccpSub(desiredPosition, getPosition());
//        Rect returnBoundingBox = RectOffset(collisionBox, diff.x, diff.y);
        return collisionBox;
    }
}*/

Rect Movable::RectOffset(Rect rect, float x, float y)
{
    return Rect(rect.origin.x + x, rect.origin.y + y, rect.size.width, rect.size.height);
}

Rect Movable::RectInset(Rect rect, float x, float y)
{
    return Rect(rect.origin.x + x, rect.origin.y + y, rect.size.width - x*2, rect.size.height - y*2);
}

/*void Movable::setOrder(int order)
{
    orderAction = order;
    if (currentAction != orderAction) {
        if (orderAction == ENEMY_ACTION_MOVE_TO_LEFT) {
            isTowardLeft = true;
            this->setFlippedX(isTowardLeft);
            
            if(unitType == UNIT_SNAIL_BALL){
                this->stopAllActions();
                this->runAction(RepeatForever::create(RotateBy::create(1, isTowardLeft?-360:360)));
            }
        }else if(orderAction == ENEMY_ACTION_MOVE_TO_RIGHT){
            isTowardLeft = false;
            this->setFlippedX(isTowardLeft);
            
            if(unitType == UNIT_SNAIL_BALL){
                this->stopAllActions();
                this->runAction(RepeatForever::create(RotateBy::create(1, isTowardLeft?-360:360)));
            }
        }
        
        switch (unitType) {
            case UNIT_LIZARD_MAN:
                //this->runAnimation("lizardMan");
                break;
                
            default:
                break;
        }
        currentAction = orderAction;
    }
}
*/
void Movable::setTarget(Movable* t)
{
    if(t == nullptr) return;
    target = t;
    targetMoveTilePos = WORLD->getCoordinateFromPosition(t->getApproachingPoint(getPosition()), WORLD->theMap);
}
float Movable::findTarget(Movable* tar, float dist)
{
    Point pos = this->getPosition();
    Point tPos = tar->getPosition();
    float distance = sqrtf((pos.x - tPos.x)*(pos.x - tPos.x) + (pos.y - tPos.y)*(pos.y - tPos.y));
    
    if (distance < dist){
        if(!isTargetFound)
            isTargetFound = true;
    }
//    else{
//        if(isTargetFound)
//            isTargetFound = false;
//    }
    return distance;
}
void Movable::runAnimationLater(float dt){
    runAnimation(animationName, isAnimationRepeat);
}
void Movable::runAnimation(const char* name, bool repeat, float delay){
    animationName = name;
    isAnimationRepeat = repeat;
    this->scheduleOnce(schedule_selector(Movable::runAnimationLater), delay);
}
void Movable::runAnimation(const char* name, bool repeat)
{
    this->stopActionByTag(ACTION_TAG_ANIMATION);
    Animation* animation = AnimationCache::getInstance()->getAnimation(name);
    if(animation == nullptr) return;
    Animate* animate = Animate::create(animation);
    if(repeat)
    {
        RepeatForever* forever = RepeatForever::create(animate);
        int frameCount = animation->getFrames().size();
        animate->setCurrentFrameIndex(rand()%frameCount);
        forever->setTag(ACTION_TAG_ANIMATION);
        this->runAction(forever);
    }
    else
    {
        animate->setTag(ACTION_TAG_ANIMATION);
        this->runAction(animate);
    }
}

void Movable::fireAnimationDone(Node* nd)
{
    
}

void Movable::jumpAnimation()
{
//    isStopped = false;
//    state = STATE_JUMPING;
}
void Movable::blinkForSec(int sec){
    blinkingTime = sec;
    this->schedule(schedule_selector(Movable::blinking), 0.1f);
}
void Movable::blinkForAWhile()
{
    if (blinkingTime <= 0) {
        blinkingTime = 0.6f;
        this->schedule(schedule_selector(Movable::blinking), 0.1f);
    }else{
        blinkingTime = 0.6f;
    }
}

void Movable::blinking(float dt)
{
    if (this->getColor().g == 115) {
        this->setColor(Color3B(255, 255, 255));
    }else{
        this->setColor(Color3B(255, 115, 115));
    }
    
    blinkingTime -= dt;
    if (blinkingTime < 0) {
        this->unschedule(schedule_selector(Movable::blinking));
        this->setColor(Color3B(255, 255, 255));
    }
}

void Movable::setInWater(bool inOrOut)
{
    inWater = inOrOut;
    if (inWater) {
        iWasInWater = true;
    }
}

void Movable::runSchedule(){
//    CCLOG("Movable runSchedule");
}

void Movable::stop(bool search){
    stopNew();
    return;
//    runAnimation(animation_type_idle);
    isTemporaryFlying = false;
    this->forceAttackTarget = false;
    resetRoute();
    targetCoordinate = Point::ZERO;
    isArrived = true;
    target = nullptr;
    canFindTarget = attackType != ATTACK_TYPE_NONE;
    if(isGoingToBuild){
        isGoingToBuild = false;
        Movable* unit = WORLD->buildTheBuilding(builderBuildingIndex, builderCoordinate.x, builderCoordinate.y, builderSize.width, builderSize.height, builderSpriteName);
        if(unit == nullptr){ // condition not met
            return;
        }
        // progress
        
        Sprite* spt = Sprite::create("uiBox.png");
        spt->setPosition(unit->getPosition() + Point(50, unit->getContentSize().height/2 + 10));
        WORLD->addChild(spt, 1000);
        auto timer = ProgressTimer::create(Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(unit->unitType)));
        processTimer = timer;
        timer->setType(ProgressTimer::Type::RADIAL);
        spt->addChild(timer);
        timer->setName("timer");
        spt->setScale(0.3f);
        timer->setPosition(spt->getContentSize()/2);
        timer->setPercentage(0);
        timer->runAction(Sequence::create(ProgressTo::create(WORLD->getUnitCreateTime(unit->unitType), 100), CallFunc::create(CC_CALLBACK_0(Movable::resetProcessTimer, this)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), nullptr));
        // progress end
        
        if(WORLD->selectedArray.find((EnemyBase*)this) != WORLD->selectedArray.end()){
            WORLD->deselect(this);
        }
        builderBuilding = unit;
        unit->setRotation(0);
        unit->buildingCompleteTime = WORLD->getUnitCreateTime(builderBuildingIndex);
        unit->setSpriteFrame("underConstrunction0.png");
        GM->runAnimation(unit, "underConstrunction", true);
        this->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
                                         DelayTime::create(0.2f),
                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
                                         DelayTime::create(0.2f),
                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
                                         DelayTime::create(0.2f),
                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
                                         DelayTime::create(0.2f),
                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
                                         DelayTime::create(0.2f),
                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
                                         DelayTime::create(0.2f),
                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
                                         DelayTime::create(0.2f),
                                         NULL));
        this->runAction(Sequence::create(DelayTime::create(unit->buildingCompleteTime), CallFunc::create(CC_CALLBACK_0(Movable::onBuildComplete, this)), NULL));
        this->setVisible(false); // test
        this->attackType = ATTACK_TYPE_NONE;
        this->untouchable = true;
    }
    if (attackFlagPos != Point::ZERO && target == nullptr && search) {
        if(attackFlagPos == getPosition()){
            attackFlagPos = Point::ZERO;
        }else{
            moveToTarget(attackFlagPos);
            canFindTarget = true;
            return;
        }
    }
    
    if (isEnemy && wantToEli && search) {
        WORLD->attackNearHero((EnemyBase*)this);
    }else if (!isGatheringGold && !WORLD->isThisSpotAvailable(this) && canMove && !isFlying) {
        isTemporaryFlying = true;
        Point coordinate = WORLD->getCoordinateFromPosition(getPosition());
        coordinate = Point(coordinate.x + rand()%3 - 1, coordinate.y + rand()%3 - 1);
        if(coordinate.x + 1 >= WORLD->mapSize.width){
            coordinate.x = WORLD->mapSize.width - 1;
        }else if(coordinate.x < 0){
            coordinate.x = 0;
        }
        if(coordinate.y + 1 >= WORLD->mapSize.height){
            coordinate.y = WORLD->mapSize.height - 1;
        }else if(coordinate.y < 0){
            coordinate.y = 0;
        }
        Point newPos = WORLD->getPositionFromTileCoordinate(coordinate.x, coordinate.y);
//        Point nowPos = getPosition();
        WORLD->moveTo((EnemyBase*)this, newPos);
//        runMoveAnimation(DIRECTION_E);
    }
}
void Movable::pauseProcess(){
    if(processTimer != nullptr){
        processTimer->pause();
    }
    for(auto btn: btns){
        btn->pause();
    }
    if(btns.size() > 0){
        ProgressTimer* timer = (ProgressTimer*)btns.at(0)->getChildByName("timer");
        if(timer != nullptr){
            timer->pause();
        }
    }
}
void Movable::resumeProcess(){
    if(processTimer != nullptr){
        processTimer->resume();
    }
    for(auto btn: btns){
        btn->resume();
    }
    if(btns.size() > 0){
        ProgressTimer* timer = (ProgressTimer*)btns.at(0)->getChildByName("timer");
        if(timer != nullptr){
            timer->resume();
        }
    }
}
void Movable::resetProcessTimer(){
    processTimer = nullptr;
}
void Movable::onBuildComplete(){//building(float dt){
//    buildingTimer += dt;
//    if(buildingTimer >= buildingCompleteTime){
    log("building complete");
    
    if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 6) {
        HUD->tutorialIndex++;
        HUD->talkIndex = 0;
        HUD->talkText = LM->getText("tutorial 7");
        Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
        spt->stopAllActions();
        spt->setSpriteFrame("barracks.png");
        WORLD->addGold(800);
        WORLD->addLumber(400);
    }else if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 7) {
        HUD->tutorialIndex++;
        HUD->talkIndex = 0;
        HUD->talkText = LM->getText("tutorial 8");
        Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
        spt->stopAllActions();
        spt->setSpriteFrame("swordmanStand0.png");
        WORLD->addGold(300);
    }
    builderBuilding->stopAllActions();
    builderBuilding->setSpriteFrame(WORLD->getSpriteNameForUnit(builderBuilding->unitType));
    builderBuilding->isBuildingComplete = true;
    WORLD->addDecoToBuilding(builderBuilding);
    if(builderBuilding->unitType == UNIT_WATCHERTOWER){
        builderBuilding->canFindTarget = true;
    }
    if(builderBuilding->unitType == UNIT_ORC_HQ){
        builderBuilding->unitType = UNIT_CASTLE;
    }
    if(builderBuilding->unitType == UNIT_CASTLE || builderBuilding->unitType == UNIT_BARRACKS || builderBuilding->unitType == UNIT_AIRPORT || builderBuilding->unitType == UNIT_FACTORY || builderBuilding->unitType == UNIT_ORC_HQ || builderBuilding->unitType == UNIT_ORC_BARRACKS || builderBuilding->unitType == UNIT_ORC_TROLL_HOUSE || builderBuilding->unitType == UNIT_TEMPLE){
        builderBuilding->startProductSchedule();
    }
    this->untouchable = false;
    WORLD->updateFoodMaxState();
    WORLD->resetPathState();
    WORLD->updateMiniMapForNonMoving();
    this->setVisible(true); // test
    if(!this->isCarryingGold && !this->isCarryingTree){
        this->attackType = ATTACK_TYPE_NEAR;
    }
    this->untouchable = false;
    stopNew();
//    }
}
void Movable::releaseGathering(){
    
}
void Movable::runAnimation(int aniType){
    if (freezed) {
        return;
    }
    if (aniType != ANIMATION_TYPE_ATTACK && currentAnimationType == aniType) {
        return;
    }
    std::string name = this->unitName;//this->getName();//getCharacterName(unitType);
    if(spine){
        if(aniType == ANIMATION_TYPE_IDLE){
            spine->setAnimation(0, "idle", true);
        }else if(aniType == ANIMATION_TYPE_ATTACK){
            spine->setAnimation(0, "attack", false);
        }else{
            spine->setAnimation(0, "run", true);
        }
    }else{
        std::string aniName;
        bool repeat = true;
        if(aniType == ANIMATION_TYPE_IDLE){
            aniName = strmake("%sStand", name.c_str());
        }else if(aniType == ANIMATION_TYPE_ATTACK){
            aniName = StringUtils::format("%sAttack", unitName.c_str());
            repeat = false;
        } else{
            aniName = strmake("%sRun", name.c_str());
        }
        Animation* animation = AnimationCache::getInstance()->getAnimation(aniName);
        if(animation != nullptr){
            runAnimation(aniName.c_str(), repeat);
        }
    }
    currentAnimationType = aniType;
}
std::string Movable::getCharacterName(int unitType){
    
    return "";
}
void Movable::attackUpdate(float dt){
//    if(isBuilding) return; // test
    attackCoolTime -= dt;
    if(!target || target->energy <= 0 || target->untouchable){
        stopNew();
        target = nullptr;
        return;
    }
    
    if(unitActDetail != UNIT_ACT_DETAIL_ATTACK){
//        attackCoolTime = 0;
        unitActDetail = UNIT_ACT_DETAIL_ATTACK;
    }
    
    if(attackCoolTime < 0){
        attackCoolTime = attackCoolTimeMax;
    }else{
        
        return;
    }
    
    runAnimation(ANIMATION_TYPE_ATTACK);
    
    skillCounter++;
    if (skillCounter > 3) { // test
        skillCounter = 0;
        isSkillOn = true;
    }
    if (isSkillOn){
        if (unitType == UNIT_HERO_ORC){
            startSkillAura(AURA_LIGHTNING); // test
        }
    }
    if(canMove)
        this->setFlippedX(getPositionX() < target->getPositionX());
    isInAttackMotion = true;
    Sequence* seq = Sequence::create(DelayTime::create(attackHappenTime), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)), nullptr);
    seq->setTag(attackTag);
    this->runAction(seq);
}
void Movable::startSkillAura(int aura){
    selectedSkillAura = aura;
    skillTarget = target;
    if(aura == AURA_LIGHTNING){
        updateSkillAura(0);
        this->schedule(schedule_selector(Movable::updateSkillAura), 0.3f);
        this->runAction(Sequence::create(DelayTime::create(39.0f/30 - 0.06f*6), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_LIGHTNING;
    }
}
void Movable::updateSkillAura(float dt){
    int diff = 50;
    if(selectedSkillAura == AURA_LIGHTNING){
        Sprite* spt = Sprite::createWithSpriteFrameName("lightningAura0.png");
        GM->runAnimation(spt, "auraLightning", false, true);
        this->addChild(spt);
        spt->setRotation(rand()%360);
        if (spine) {
            spt->setPosition(spine->getPosition() + Vec2(diff - rand()%(diff*2), spine->getBoundingBox().size.height/2 + diff - rand()%(diff*2)));
        }else{
            spt->setPosition(Vec2(getContentSize().width/2 - diff + rand()%(diff*2), getContentSize().height/2 - diff + rand()%(diff*2)));
        }
        spt->setScale(1/getScaleX());
    }
}
void Movable::stopSkillAura(){
    this->unschedule(schedule_selector(Movable::updateSkillAura));
}
void Movable::showSkillEffect(){
    if (skillEffectIndex == EFFECT_LIGHTNING && skillTarget){
        WORLD->runEffect(EFFECT_LIGHTNING, skillTarget->getPosition());
    }
}
void Movable::attack(){
    isInAttackMotion = false;
    if(!this || !target || target == nullptr || target->untouchable || target->energy <= 0) {
        target = nullptr;
        if (unitAct == UNIT_ACT_ATTACK) {
            stopNew();
        }
        return;
    }
    if (isSkillOn){
        if (unitType == UNIT_HERO_ORC){
            stopSkillAura(); // test
            WORLD->splashDamage(skillTarget->getPosition(), 10500, ap, isEnemy, this);
            WORLD->shakeScreenOnce();
            isSkillOn = false;
        }
    }
    if(target->getParent() == nullptr){
        stopNew();
        return;
    }
    if(unitType == UNIT_GOBLIN_BOMB){
        WORLD->runEffect(EFFECT_EXPLODE_MIDDLE, getPosition());
        WORLD->splashDamage(getPosition(), 100000, ap, isEnemy, nullptr);
        WORLD->removeDeadUnit((EnemyBase*)this);
        return;
    }
    if (isBuilding && !isBuildingComplete) {
        return;
    }
    if (attackType == ATTACK_TYPE_NEAR) {
        if(target->getHitAndIsDead(ap, this)){
            WORLD->removeDeadUnit((EnemyBase*)target);
            stopNew();
        }
    }else if(attackType == ATTACK_TYPE_RANGE){
        float speed = 1000;
        if(unitType == UNIT_ARCHER || unitType == UNIT_WATCHERTOWER){
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, ap, 0, "arrow.png");
            if(unitType == UNIT_WATCHERTOWER){
                speed = 1500;
            }
        }else if(unitType == UNIT_ORC_BUNKER || unitType == UNIT_ORC_HQ || unitType == UNIT_ORC_SPEAR){
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, ap, 0, "spear.png");
            speed = 1500;
        }else if(unitType == UNIT_CATAPULT || unitType == UNIT_HELICOPTER){
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, ap, 0, "boulder.png");
            missile->runAction(RotateBy::create(2, 720));
            missile->runAction(ScaleTo::create(0.5f, 1.5f));
        }else if(unitType == UNIT_WIZARD){
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, ap, 0, "fireBall0.png");
            speed = 300;
        }else{
            return;
        }
        if(WORLD->isInScreen(getPosition())){
            GM->playSoundEffect(SOUND_ARROW);
        }else{
            GM->playSoundEffect(SOUND_ARROW, 0.1f);
        }
        
        missile->ap = ap;
        missile->target = target;
        missile->attackTargetType = attackTargetType;
        WORLD->spriteBatch->addChild(missile, 100);
//        this->getParent()->addChild(missile, 100);
        float distance = getPosition().distance(target->getPosition());
        float dur = distance/speed;
        float height = 600*dur;
        if(height > 250){
            height = 250;
        }
        if(unitType == UNIT_ARCHER || unitType == UNIT_ORC_SPEAR){
            missile->runAction(Sequence::create(EaseOut::create(MoveBy::create(dur/2, Point(height*(isTowardLeft?-1:1), 0)), 2), EaseIn::create(MoveBy::create(dur/2, Point(-height*(isTowardLeft?-1:1), 0)), 2), NULL));
        }
        Point extraPos = Point::ZERO;
        if(unitType == UNIT_WATCHERTOWER || unitType == UNIT_ORC_BUNKER || unitType == UNIT_ORC_HQ){ // straight
            if(unitType == UNIT_WATCHERTOWER || unitType == UNIT_HELICOPTER){
                extraPos = Point(0, 100);
            }
            missile->setRotation(-GM->getAngle(getPosition(), target->getPosition()) - 90);
        }else if(unitType == UNIT_WIZARD){
            // straight not rotated
        }else{ // curve
            missile->runAction(Sequence::create(EaseOut::create(MoveBy::create(dur/2, Point(0, height)), 2), EaseIn::create(MoveBy::create(dur/2, Point(0, -height)), 2), NULL));
            
            missile->setRotation(isTowardLeft?-45:45);
        }
        missile->schedule(schedule_selector(Movable::updateMissileAngle), 0.05f);
        missile->setPosition(getPosition() + extraPos);
        missile->updateMissileAngle(0);
        missile->untouchable = true;
        missile->shooter = this;
        missile->isEnemy = isEnemy;
        missile->runAction(Sequence::create(MoveTo::create(dur, target->getPosition()), CallFunc::create(CC_CALLBACK_0(Movable::onMissileMoveDone, missile)), NULL));
    }
}
void Movable::updateMissileAngle(float dt){
    if(lastMissilePosition != Point::ZERO)
        this->setRotation(-GM->getAngle(lastMissilePosition, getPosition()) - 90);
    lastMissilePosition = getPosition();
//    this->setRotation(90);
}
void Movable::onMissileMoveDone(){
    removeFromParent();
    if (attackTargetType == ATTACK_TARGET_TYPE_SPLASH) {
        if(this->shooter && this->shooter->unitType == UNIT_WIZARD){
            WORLD->runEffect(EFFECT_FIREBALL_EXPLOSION, getPosition());
        }else{
            WORLD->runEffect(EFFECT_EXPLODE_MIDDLE, getPosition());
        }
        
        WORLD->splashDamage(getPosition(), 10500, ap, isEnemy, shooter);
    }else{
        if (target && target->energy > 0) {
            if(target->getHitAndIsDead(ap, shooter)){
                WORLD->removeDeadUnit((EnemyBase*)target);
                stop();
            }
        }
    }
}
void Movable::refreshApproachPoints(){
    int index = 0;
    Point currentPos = getPosition();
    if (isBuilding) {
        int offset = 2;
        Point startPos = WORLD->getPositionFromTileCoordinate(buildingStartCoordinate.x, buildingStartCoordinate.y + buildingOccupySize.height) + Point(-TILE_SIZE/2, TILE_SIZE/2);
        approachingPoints[0] = startPos + Point(-offset, -offset);
        approachingPoints[1] = startPos + Point(buildingOccupySize.width*TILE_SIZE + offset, -offset);
        approachingPoints[2] = startPos + Point(-offset, buildingOccupySize.height*TILE_SIZE + offset);
        approachingPoints[3] = startPos + Point(buildingOccupySize.width*TILE_SIZE + offset, buildingOccupySize.height*TILE_SIZE + offset);
        index = 4;
        for (int i = 0; i < buildingOccupySize.width; i++) {
            approachingPoints[index] = startPos + Point(TILE_SIZE/2 + TILE_SIZE*i, -offset);
            approachingPoints[index + 1] = startPos + Point(TILE_SIZE/2 + TILE_SIZE*i, buildingOccupySize.height*TILE_SIZE + offset);
            index += 2;
        }
        for (int i = 0; i < buildingOccupySize.height; i++) {
            approachingPoints[index] = startPos + Point(-offset, TILE_SIZE/2 + TILE_SIZE*i);
            approachingPoints[index + 1] = startPos + Point(buildingOccupySize.width*TILE_SIZE + offset, TILE_SIZE/2 + TILE_SIZE*i);
            index += 2;
        }
    }else{
        int bottomY = getBoundingBox().origin.y;
        for (int x = -1; x < 2; x++) {
            for (int y = 0; y < 3; y++) {
                if(x == 0 && y == 1){
                    continue;
                }
                approachingPoints[index] = Point(currentPos.x + x*51, bottomY + y*51);
                index++;
            }
        }
    }
    approachPointCount = index;
    for(int i = 0; i< approachPointCount; i++){
        if(approachingPoints[i].x < 1){
            approachingPoints[i].x = 1;
        }else if(approachingPoints[i].x > WORLD->mapSize.width*TILE_SIZE - 1){
            approachingPoints[i].x = WORLD->mapSize.width*TILE_SIZE - 1;
        }
        if(approachingPoints[i].y < 1){
            approachingPoints[i].y = 1;
        }else if(approachingPoints[i].y > WORLD->mapSize.height*TILE_SIZE - 1){
            approachingPoints[i].y = WORLD->mapSize.height*TILE_SIZE - 1;
        }
        
    }
}
cocos2d::Point Movable::getApproachingPoint(cocos2d::Point from){
    if (lastApprochingPointCheckPosition != getPosition()) { // calculate points
        refreshApproachPoints();
    }
    lastApprochingPointCheckPosition = getPosition();
    int minLength = 90000;
    int pickedIndex = -1;
    PointArray* array;
    Point minPos;
    Point coordinate, destCoordinate;
    
    for (int i = 0; i < approachPointCount; i++) {
        coordinate = WORLD->getCoordinateFromPosition(from, WORLD->theMap);
        destCoordinate = WORLD->getCoordinateFromPosition(approachingPoints[i], WORLD->theMap);
        if(!WORLD->isCoordinateValid(destCoordinate) || WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(destCoordinate)) || !WORLD->isCoordinateValid(coordinate) || WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(coordinate))){
            continue;
        }
        
//        array = GM->getPath(coordinate, destCoordinate);
//        if (array == nullptr || array->count() == 0) {
//            continue;
//        }
//        else{
//            return approachingPoints[i];
            if (minLength > abs(from.x - approachingPoints[i].x) + abs(from.y - approachingPoints[i].y)) {
                minPos = approachingPoints[i];
                minLength = abs(from.x - approachingPoints[i].x) + abs(from.y - approachingPoints[i].y);
            }
//        }
//        if( i < 4){
//            if(array->count() < minDistance){
//                minDistance = array->count();
//                pickedIndex = i;
//            }
//            if(i == 3 && pickedIndex > -1){
//                return approachingPoints[pickedIndex];
//            }
//        }else{
//            return approachingPoints[i];
//        }
    }
    if (minLength < 90000) {
        return minPos;
    }
//    GM->drawPath();
//    if(pickedIndex >= 0){
//        return approachingPoints[pickedIndex];
//    }
    return getPosition();
}
bool Movable::getHitAndIsDead(int ap, Movable* attacker){
    if(energy <= 0) return true;
//    if(WORLD->testSuper){
//        if(attacker != nullptr && attacker->alliSide == WHICH_SIDE_HERO){
//            ap = 10000;
//        }else{
//            ap = 0;
//        }
//    }
    energy -= ap;
    updateEnergy();
    if (isBuilding && energy < maxEnergy/2 && !this->getChildByName("sptFire")) {
        Sprite* spt = Sprite::createWithSpriteFrameName("fire0.png");
        this->addChild(spt);
        spt->setName("sptFire");
        spt->setPosition(Point(getContentSize().width/3, getContentSize().height*2/3));
        GM->runAnimation(spt, "fire", true);
    }
    if (this->unitType != UNIT_MINE) {
        if(WORLD->isInScreen(getPosition())){
            GM->playSoundEffect(SOUND_HIT);
        }else{
            GM->playSoundEffect(SOUND_HIT, 0.1f);
        }
    }
//    log("energy: %d, ap: %d", energy, ap);
    
    if (attacker != target && energy > 0 && attacker != nullptr && !attacker->untouchable) {
        if (!target || target == nullptr || canRevengeAttack || WORLD->getAttackPriority(attacker) > WORLD->getAttackPriority(target)) {
            bool canAttack = WORLD->canAttack(this, attacker);
            if(canAttack){
                WORLD->revengeAttack(this, attacker);
            }
        }
    }
    if(HUD->isRaid || unitType != UNIT_MINE){
        WORLD->runEffect(EFFECT_HIT, getPosition(), isTowardLeft?0:180);//GM->getAngle(getPosition(), attacker->getPosition()));
    }
    return energy <= 0;
}
bool Movable::isTargetLockedOn(){
    if(target == nullptr)
        return false;
    int range = 3;
    if(this->getPositionX() - range <= target->getPositionX() && this->getPositionX() + range >= target->getPositionX()){
        return true;
    }
    
    if(this->getPositionY() - range <= target->getPositionY() && this->getPositionY() + range >= target->getPositionY()){
        return true;
    }
    return false;
}
void Movable::showEffect(int effect, float delay){
    if(effect == EFFECT_WING){
        for (int i = 0; i < 2; i++) {
            Sprite* sptWing = Sprite::createWithSpriteFrameName("wing.png");
            this->addChild(sptWing, 100);
            sptWing->setFlippedX(i==1);
            sptWing->setAnchorPoint(Point(i==0?0:1, 0));
            sptWing->setOpacity(0);
            sptWing->setScale(0.5f);
            sptWing->setPosition(Point(i==0?this->getContentSize().width + 3:-3, -this->getContentSize().height));
            float fadeInTime = 0.1f;
            float moveTime = 1;
            float fadeOutTime = 0.3f;
            sptWing->runAction(Sequence::create(DelayTime::create(delay), MoveBy::create(0.2f, Point(i==0?5:-5, 0)), nullptr));
            sptWing->runAction(Sequence::create(DelayTime::create(delay), EaseInOut::create(ScaleTo::create(0.2f, 1, 1), 2), nullptr));
            sptWing->runAction(Sequence::create(DelayTime::create(delay), EaseInOut::create(MoveBy::create(0.2f, Point(0, 15)), 2),  nullptr));
            sptWing->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0.1f), DelayTime::create(moveTime - fadeInTime - fadeOutTime), FadeOut::create(fadeOutTime), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sptWing)), nullptr));
            
        }
    }else if(effect == EFFECT_BOTTOM_RECT_GLOW){
        Sprite* spt = Sprite::createWithSpriteFrameName("glowRect.png");
        this->addChild(spt, -1);
        spt->setAnchorPoint(Point(0.5, 0.5));
        spt->setPosition(Point(getContentSize().width/2, getContentSize().height/2));
        spt->setOpacity(0);
        spt->setScale(1);
        spt->runAction(Sequence::create(DelayTime::create(delay), ScaleTo::create(0.2f, 2.5), nullptr));
        spt->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0.1f), DelayTime::create(1), FadeOut::create(0.3f), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
    }else if(effect == EFFECT_RISING_PARTICLE){
        float extraDelay = 0;
        for (int i = 0; i < 6; i++) { // sequence
            for (int j = 0; j < 3; j++) {   // particle at once
                Sprite* spt = Sprite::createWithSpriteFrameName("glowParticle.png");
                this->addChild(spt);
                spt->setOpacity(0);
                float fadeInTime = 0.1f;
                float moveTime = 0.4f;
                float fadeOutTime = 0.2f;
                
                spt->setPosition(Point(rand()%(int)this->getContentSize().width, rand()%(int)getContentSize().height + (rand()%10)*0.1f));
                spt->runAction(Sequence::create(DelayTime::create(delay + extraDelay), FadeIn::create(fadeInTime), DelayTime::create(moveTime - fadeInTime - fadeOutTime), FadeOut::create(fadeOutTime), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
                spt->runAction(Sequence::create(DelayTime::create(delay + extraDelay), MoveBy::create(moveTime, Point(0, 10)), nullptr));
            }
            extraDelay += 0.2f;
        }
    }else if(effect == EFFECT_PARTICLE_TORNADO){
        float extraDelay = 0;
        for (int i = 0; i < 5; i++) { // sequence
            extraDelay = i*0.07f;
            for (int j = 0; j < 16; j++) {   // particle at once
                Sprite* spt = Sprite::createWithSpriteFrameName("glowParticle.png");
                this->addChild(spt);
                spt->setPosition(Point(-3, i*3));
                spt->setOpacity(0);
//                spt->setScale(0.5f);
                spt->runAction(Sequence::create(DelayTime::create(delay + extraDelay), FadeIn::create(0.05f), DelayTime::create(0.1f), FadeOut::create(0.05f), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
                spt->runAction(Sequence::create(DelayTime::create(delay + extraDelay), MoveBy::create(0.2f, Point(this->getContentSize().width + 6, 0)), NULL));
                spt->runAction(Sequence::create(DelayTime::create(delay + extraDelay), EaseInOut::create(MoveBy::create(0.2f, Point(0, 5)), 2), nullptr));
                extraDelay += 0.08f;
            }
//            extraDelay += 0.1f;
        }
    }
}
void Movable::moveToTarget(Movable* unit){
    moveFlagPos = unit->getPosition();
    moveToPos = Vec2::ZERO;
    unitActDetail = UNIT_ACT_DETAIL_IDLE;
    target = unit;
    return;
    try
    {
//        forceAttackTarget = false;
        if(!canMove){
            return;
        }
        if(unit && unit->untouchable){
            if(dummyTarget == nullptr){
                dummyTarget = new Movable();
                dummyTarget->initWithSpriteFrameName("workerAxeStand0.png");
                this->addChild(dummyTarget);
                dummyTarget->setVisible(false);
                dummyTarget->untouchable = true;
            }
            dummyTarget->setPosition(unit->getPosition());
            target = dummyTarget;
        }else{
            target = unit;
        }
        
        moveToTarget();
    }catch(...){
        
    }
}
void Movable::attackDdangTo(Vec2 pos){
    moveFlagPos = pos;
    log("attackddang: %f, %f", pos.x, pos.y);
    moveToPos = Vec2::ZERO;
    unitAct = UNIT_ACT_ATTACK_DDANG;
    unitActDetail = UNIT_ACT_DETAIL_IDLE;
    runAnimation(ANIMATION_TYPE_IDLE);
}
void Movable::moveToTarget(cocos2d::Point targetPos){
    Point selfPos = WORLD->getCoordinateFromPosition(this->getPosition());
    targetMoveTilePos = WORLD->getCoordinateFromPosition(targetPos);
    if(WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(targetMoveTilePos)) || WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(selfPos))){
        return;
    }
    
    resetRoute();
    if(isFlying || isTemporaryFlying){
        if(target == nullptr){
            addRoute(targetPos); // test
        }else{
            addRoute(target->getPosition()); // test
        }
        
        isTemporaryFlying = false;
    }else{
        if (failedFindPathStart == selfPos && failedFindPathEnd == targetMoveTilePos) {
            return;
        }
        PointArray* array = GM->getPath(selfPos, targetMoveTilePos);
        if(!array || array->count() == 0){
            if(target){
                unreachableTarget = target;
            }
            failedFindPathStart = selfPos;
            failedFindPathEnd = targetMoveTilePos;
            if(targetMoveTilePos == attackFlagTilePos){
                failedAttackFlagPos = attackFlagPos;
                attackFlagPos = Vec2::ZERO;
            }
            stop(false);
            return;
        }
        
        Point pos;
        int startI = array->count()-1;
        if(startI > 1){
            startI--;
        }
        for (int i = startI; i >= 0; i--) {
            pos = array->getControlPointAtIndex(i);
            addRoute(WORLD->getPositionFromTileCoordinate(pos.x, pos.y));
        }
        if (routePositionArray != nullptr && routePositionArray->count() > 0) {
            if(!WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(WORLD->getCoordinateFromPosition(targetPos)))){
                routePositionArray->removeControlPointAtIndex(routePositionArray->count() - 1); // remove last tile pos
                addRoute(targetPos); // test
            }
            if(routePositionArray->count() > 2){
                routePositionArray->removeControlPointAtIndex(0);
            }
//            runMoveAnimation(DIRECTION_E);
        }else{
            if(target != nullptr && attackType == ATTACK_TYPE_RANGE && attackRange > getPosition().distanceSquared(target->getPosition())){
                
            }else{
                if(!isZombie){
                    attackFlagPos = Point::ZERO;
                    stop();
                }
            }
        }
    }
}
void Movable::moveToTarget(){
    if(!canMove || target == nullptr){
        return;
    }
//    if(target == nullptr){
//        return;
//    }
//
    Point newTargetPos;
    Point currentPos = getPosition();
    if (lastValidPoint != Point::ZERO) {
//        currentPos = lastValidPoint;
    }
    
    Point approachPos = target->getApproachingPoint(currentPos);
//    if(isFlying || isTemporaryFlying){
//        newTargetPos = target->getPosition();
//    }else{
        newTargetPos = WORLD->getCoordinateFromPosition(approachPos, WORLD->theMap);
//    }
    
    if(newTargetPos == targetCoordinate){
        return;
    }
    
    targetCoordinate = newTargetPos;
    Point selfPos = WORLD->getCoordinateFromPosition(currentPos, WORLD->theMap);
    if(newTargetPos != targetMoveTilePos){
        targetMoveTilePos = newTargetPos;
    }
    
    
    if (failedFindPathStart.x == selfPos.x &&
        failedFindPathStart.y == selfPos.y &&
        failedFindPathEnd.y == targetMoveTilePos.y &&
        failedFindPathEnd.x == targetMoveTilePos.x) {
        stop();
        return;
    }
    if(newTargetPos == selfPos){
        if(!WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(WORLD->getCoordinateFromPosition(target->getPosition(), WORLD->theMap)))){
            addRoute(target->getPosition());
        }else if(!WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(WORLD->getCoordinateFromPosition(target->getApproachingPoint(getPosition()), WORLD->theMap)))){
            addRoute(target->getApproachingPoint(getPosition()));
        }
    }else{
//        WORLD->draw->clear();
        resetRoute();
        unitActDetail = UNIT_ACT_DETAIL_WALK_ROUTE;
        
        if(isFlying || isTemporaryFlying){
//            addRoute(target->getPosition()); // test
            addRoute(target->getApproachingPoint(getPosition()));
//            runMoveAnimation(DIRECTION_E);
        }else{
            PointArray* array = GM->getPath(selfPos, targetMoveTilePos);
//            GM->drawPath();
//            Point nextPos = target->getPosition();
            if(array == nullptr || array->count() == 0){
                failedFindPathStart = selfPos;
                failedFindPathEnd = targetMoveTilePos;
//                stop();
                return;
            }else{
//                if(!WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(WORLD->getCoordinateFromPosition(approachPos, WORLD->theMap)))){
//                    nextPos = WORLD->getPositionFromTileCoordinate(array->getControlPointAtIndex(array->count()-1).x, array->getControlPointAtIndex(array->count()-1).y);
//                }
//                array = GM->getPath(selfPos, targetMoveTilePos);
            }
            
            Point pos;
            int startI = array->count()-1;
            if(startI > 1){
                startI--;
            }
            for (int i = startI; i >= 0; i--) {
                pos = array->getControlPointAtIndex(i);
                addRoute(WORLD->getPositionFromTileCoordinate(pos.x, pos.y));
            }
            if (routePositionArray != nullptr && routePositionArray->count() > 0) {
                if(!WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(WORLD->getCoordinateFromPosition(approachPos, WORLD->theMap)))){
//                    if(target->isBuilding){
//                        lastValidPoint = routePositionArray->getControlPointAtIndex(routePositionArray->count() - 1);
//                    }else{
//                        lastValidPoint = Point::ZERO;
//                    }
                    
//                    addRoute(approachPos); // test
                    
                    if(!WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(WORLD->getCoordinateFromPosition(approachPos, WORLD->theMap)))){
                        routePositionArray->removeControlPointAtIndex(routePositionArray->count() - 1);
                        if(routePositionArray->count() > 1){
                            routePositionArray->removeControlPointAtIndex(0);
                        }
                        addRoute(approachPos);
                    }
                }
                
//                runMoveAnimation(DIRECTION_E);
            }else{
                if(attackType == ATTACK_TYPE_RANGE && attackRange > getPosition().distanceSquared(target->getPosition())){
                    
                }else{
                    stop(false);
                }
            }
        }
    }
}
void Movable::resetRoute(){
    while(routePositionArray != nullptr && routePositionArray->count() > 0){
        routePositionArray->removeControlPointAtIndex(0);
    }
}
void Movable::addRoute(cocos2d::Point dest){
    isArrived = false;
    movePointCounter = 0;
    if(routePositionArray == nullptr){
        routePositionArray = PointArray::create(200);
        routePositionArray->retain();
        
    }else{
        
    }
    routePositionArray->addControlPoint(dest);
}
void Movable::gatherGold(Movable* mine){
    if(mine == nullptr || mine->energy <= 0){
        isGatheringGold = false;
        stop();
        return;
    }
    this->isGatheringGold = true;
    this->isGatheringTree = false;
//    this->isTemporaryFlying = true;
    
    this->unitAct = UNIT_ACT_GATHER_GOLD;
    this->unitActDetail = UNIT_ACT_DETAIL_IDLE;
    this->moveToPos = Vec2::ZERO;
    this->target = mine;
    this->moveFlagPos = mine->getPosition();
//    this->moveToTarget(mine); // old
    this->currentMine = mine;
    this->gatheringTimer = 3;
//    runMoveAnimation(DIRECTION_E);
}

void Movable::gatherTree(Movable* tree){
    this->currentTree = tree;
    if(tree == nullptr) {
        this->isGatheringTree = false;
        stopNew();
//        EnemyBase* tree = WORLD->getNearestTree(getPosition());
//        if(tree != nullptr){
//            gatherTree(tree);
//        }
//        lastTreePos = this->getPosition();
        return;
    }
    lastTreePos = tree->getPosition();
    attackCoolTime = 0;
    this->isGatheringTree = true;
    this->isGatheringGold = false;
//    isTemporaryFlying = true;
    this->moveToTarget(tree);
    unitAct = UNIT_ACT_GATHER_TREE;
//    runMoveAnimation(DIRECTION_E);
}
void Movable::attackTree(){
    if(currentTree != nullptr){
        Movable* tree = (Movable*)currentTree;
        if(tree->energy <= 0){ // someone took it
            gatherTree(WORLD->getNearestTree(getPosition()));
            return;
        }
        
        tree->energy -= ap;
        if(tree->energy <= 0){
            Point treePos = WORLD->getCoordinateFromPosition(tree->getPosition(), WORLD->theMap);//Point((int)currentTree->getPositionX()/TILE_SIZE, (int)currentTree->getPositionY()/TILE_SIZE);
            WORLD->decoLayer->setTileGID(0, treePos);
            WORLD->updateMiniMapForNonMoving();
            WORLD->resetPathState();
            
            isCarryingTree = true;
            EnemyBase* tank = WORLD->getNearestLumberTank(getPosition());
            unitName = strmake("%sWood", getName().c_str());
            runAnimation(ANIMATION_TYPE_IDLE);
            if(tank != nullptr){
                returningPlace = tank;
//                isTemporaryFlying = true;
                moveToTarget(tank);
                unitAct = UNIT_ACT_GATHER_TREE;
//                runMoveAnimation(DIRECTION_E);
            }
            WORLD->mutualArray.eraseObject((EnemyBase*)currentTree);
            
            for(auto spt: tree->childrenSprite){
                spt->setSpriteFrame(strmake("treeTrunk%d.png", rand()%3));
                spt->setLocalZOrder(-10000);
                spt->runAction(Sequence::create(FadeOut::create(5), SPT_REMOVE_FUNC, NULL));
            }
//            tree->setLocalZOrder(-10000);
            tree->setLocalZOrder(-this->getBoundingBox().origin.y - 100);
            tree->childrenSprite.clear();
            
//            Point pos;
//            for(auto unit: WORLD->mutualArray){
//                pos = Point((int)unit->getPositionX()/TILE_SIZE, (int)unit->getPositionY()/TILE_SIZE);
//                if(pos == treePos){
//                    spt = (Sprite*)unit;
//                    spt->setSpriteFrame("treeTrunk.png");
//                    WORLD->mutualArray.eraseObject(unit);
//                    break;
//                }
//            }
            currentTree = nullptr;
        }
    }
}
void Movable::queueUnit(int index){
    Button* btn = Button::create("uiBox.png");
    this->getParent()->getParent()->addChild(btn, 10);
    btn->setTag(index);
    btns.pushBack(btn);
    btn->setScale(0.4f);
    Sprite* sptBuilding = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(index));
    btn->addClickEventListener(CC_CALLBACK_1(Movable::cancelProduct, this));
    btn->addChild(sptBuilding);
    sptBuilding->setPosition(Point(btn->getContentSize().width/2, btn->getContentSize().height/2));
    sptBuilding->setOpacity(50);
    
    sptBuilding = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(index));
    auto timer = ProgressTimer::create(sptBuilding);
    timer->setType(ProgressTimer::Type::RADIAL);
    btn->addChild(timer);
    timer->setName("timer");
    //        timer->setReverseProgress(true);
    timer->setPosition(Point(btn->getContentSize().width/2, btn->getContentSize().height/2));
    timer->setPercentage(0);
    
    timer->setPosition(btn->getContentSize()/2);
    
    if(btns.size() == 1){
        productCompleteTimer = WORLD->getUnitCreateTime(index);
//        btns.at(0)->runAction(RotateBy::create(productCompleteTimer, 360));
        timer->runAction(ProgressTo::create(productCompleteTimer, 100));
    }
    updateProductButtons();
    if (index == UNIT_SWORDMAN) {
//        if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 8) {
//            HUD->tutorialIndex++;
//            HUD->talkIndex = 0;
//            HUD->talkText = LM->getText("tutorial 9");
//            Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
//            spt->stopAllActions();
//            Sprite* sptTemp = Sprite::create("handIcon.png");
//            spt->setSpriteFrame(sptTemp->getSpriteFrame());
//            Size size = Director::getInstance()->getWinSize();
//            spt->runAction(RepeatForever::create(Sequence::create(MoveTo::create(0, Point(size.width/2 - 750 - 100, 330 + 50)), MoveBy::create(0.5f, Point(150, -200)), DelayTime::create(1), NULL)));
//        }
        
        if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 8) {
            HUD->tutorialIndex++;
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
}
void Movable::startProductSchedule(){
    this->schedule(schedule_selector(Movable::productUpdate), 0.5f);
}
void Movable::stopProductSchedule(){
    this->unschedule(schedule_selector(Movable::productUpdate));
}
void Movable::productUpdate(float dt){
    if(btns.size() > 0){
        productCompleteTimer -= dt;
        if(productCompleteTimer <= 0 && WORLD->foodMax >= WORLD->foodInUse+ GM->getFoodUseForUnit(btns.at(0)->getTag())){
            Button* btn = btns.at(0);
            int index = btn->getTag();
            std::string spriteName = WORLD->getSpriteNameForUnit(index);
            EnemyBase* unit = WORLD->createUnit(index, isEnemy?WHICH_SIDE_ENEMY:WHICH_SIDE_HERO, ITS_NOT_BUILDING, approachingPoints[0], GM->getUnitName(index), 1, spriteName.c_str());
            btns.eraseObject(btn);
            btn->removeFromParent();
            if(btns.size() > 0){
                productCompleteTimer = WORLD->getUnitCreateTime(btns.at(0)->getTag());
//                btns.at(0)->runAction(RotateBy::create(productCompleteTimer, 360));
                ProgressTimer* timer = (ProgressTimer*)btns.at(0)->getChildByName("timer");
                timer->runAction(ProgressTo::create(productCompleteTimer, 100));
            }
            if(!GM->isColosseum && GM->currentStageIndex == 0){
                WORLD->checkClearGame();
            }
            WORLD->totalProducedUnit++;
            updateProductButtons();
        }
    }
}
void Movable::cancelProduct(Ref* ref){
    Button* btn = (Button*)ref;
    int index = btn->getTag();
    if(btns.at(0) == btn && btns.size() > 1 && btns.at(1) != nullptr){
        productCompleteTimer = WORLD->getUnitCreateTime(btns.at(1)->getTag());
    }
    
    WORLD->addGold(WORLD->getGoldPriceForUnit(index)*0.7f);
    WORLD->addLumber(WORLD->getLumberPriceForUnit(index)*0.7f);
    btns.eraseObject(btn);
    btn->removeFromParent();
    updateProductButtons();
}
void Movable::updateProductButtons(){
    int counter = 0;
    for(auto btn: btns){
//        int index = btn->getTag();
        btn->setPosition(this->getPosition() + Point(-buildingOccupySize.width*TILE_SIZE/2 + counter*80, TILE_SIZE));
        counter++;
    }
}
void Movable::stopNew(){
    canRevengeAttack = true;
    unitAct = UNIT_ACT_NONE;
    unitActDetail = UNIT_ACT_DETAIL_IDLE;
    runAnimation(ANIMATION_TYPE_IDLE);
    moveToPos = Vec2::ZERO;
    isTemporaryFlying = false;
    if (attackFlagPos != Vec2::ZERO) {
        unitAct = UNIT_ACT_ATTACK_DDANG;
        moveFlagPos = attackFlagPos;
        moveToPos = Vec2::ZERO;
    }
    // check multiple units in the same spot
    if (!isGatheringGold && !WORLD->isThisSpotAvailable(this) && canMove && !isFlying) {
        isTemporaryFlying = true;
        Point coordinate = WORLD->getCoordinateFromPosition(getPosition());
        coordinate = Point(coordinate.x + rand()%3 - 1, coordinate.y + rand()%3 - 1);
        if(coordinate.x + 1 >= WORLD->mapSize.width){
            coordinate.x = WORLD->mapSize.width - 1;
        }else if(coordinate.x < 0){
            coordinate.x = 0;
        }
        if(coordinate.y + 1 >= WORLD->mapSize.height){
            coordinate.y = WORLD->mapSize.height - 1;
        }else if(coordinate.y < 0){
            coordinate.y = 0;
        }
        Point newPos = WORLD->getPositionFromTileCoordinate(coordinate.x, coordinate.y);
        //        WORLD->moveTo((EnemyBase*)this, newPos);
        unitAct = UNIT_ACT_MOVE;
        moveToPos = Vec2::ZERO;
        moveFlagPos = newPos;
    }
    
    if(isGoingToBuild){
        isGoingToBuild = false;
        Movable* unit = WORLD->buildTheBuilding(builderBuildingIndex, builderCoordinate.x, builderCoordinate.y, builderSize.width, builderSize.height, builderSpriteName);
        if(unit == nullptr){ // condition not met
            return;
        }
        // progress
        
        Sprite* spt = Sprite::create("uiBox.png");
        spt->setPosition(unit->getPosition() + Point(50, unit->getContentSize().height/2 + 10));
        WORLD->addChild(spt, 1000);
        auto timer = ProgressTimer::create(Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(unit->unitType)));
        processTimer = timer;
        timer->setType(ProgressTimer::Type::RADIAL);
        spt->addChild(timer);
        timer->setName("timer");
        spt->setScale(0.3f);
        timer->setPosition(spt->getContentSize()/2);
        timer->setPercentage(0);
        timer->runAction(Sequence::create(ProgressTo::create(WORLD->getUnitCreateTime(unit->unitType), 100), CallFunc::create(CC_CALLBACK_0(Movable::resetProcessTimer, this)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), nullptr));
        // progress end
        
        if(WORLD->selectedArray.find((EnemyBase*)this) != WORLD->selectedArray.end()){
            WORLD->deselect(this);
        }
        builderBuilding = unit;
        unit->setRotation(0);
        unit->buildingCompleteTime = WORLD->getUnitCreateTime(builderBuildingIndex);
        unit->setSpriteFrame("underConstrunction0.png");
        GM->runAnimation(unit, "underConstrunction", true);
        this->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
                                         DelayTime::create(0.2f),
                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
                                         DelayTime::create(0.2f),
                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
                                         DelayTime::create(0.2f),
                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
                                         DelayTime::create(0.2f),
                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
                                         DelayTime::create(0.2f),
                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
                                         DelayTime::create(0.2f),
                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
                                         DelayTime::create(0.2f),
                                         NULL));
        this->runAction(Sequence::create(DelayTime::create(unit->buildingCompleteTime), CallFunc::create(CC_CALLBACK_0(Movable::onBuildComplete, this)), NULL));
        this->setVisible(false); // test
        this->attackType = ATTACK_TYPE_NONE;
        this->untouchable = true;
    }
}
void Movable::cancelAttackSchedule(){
    if (unitType == UNIT_HERO_ORC) {
        stopSkillAura();
    }
    this->stopActionByTag(attackTag);
}
void Movable::moveNew(float dt){// movenew start
    Vec2 prePos = getPosition();
    int previousAct = unitAct;
    if(unreachableTarget != nullptr && unreachableTarget == target){
        unitAct = UNIT_ACT_NONE;
    }
    if(checkAttackTargetReturnSuccess(dt)){
        return;
    }else{
        unitAct = previousAct;
    }
    
    // handle move
    if (!isBuilding && (unitAct == UNIT_ACT_ATTACK_DDANG ||
        unitAct == UNIT_ACT_ATTACK ||
        unitAct == UNIT_ACT_MOVE ||
        unitAct == UNIT_ACT_GATHER_GOLD ||
        unitAct == UNIT_ACT_GATHER_TREE)) {
        if (moveToPos == Vec2::ZERO && unitActDetail != UNIT_ACT_DETAIL_ATTACK) {
//            if(target != nullptr){
//                moveToPos = target->getApproachingPoint(getPosition());
//            }else{
                moveToPos = moveFlagPos;
//            }
            targetMoveTilePos = WORLD->getCoordinateFromPosition(moveToPos);
//            if(WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(targetMoveTilePos))){
//                stopNew();
//                return;
//            }
            Point selfPos = WORLD->getCoordinateFromPosition(this->getPosition());
            resetRoute();
            PointArray* array = PointArray::create(1);
            if (target) { // check target occupy the ground
                if(target->isBuilding){
                    for (int j = 0; j < target->buildingOccupySize.height; j++) {
                        for (int i = 0; i < target->buildingOccupySize.width; i++) {
                            GM->setPathState(target->buildingStartCoordinate.x + i, target->buildingStartCoordinate.y + j, false);
                        }
                    }
                }else if(target->unitType == UNIT_TREE || target->unitType == UNIT_TREE_FOR_BATTLE){
                    Point targetPos = WORLD->getCoordinateFromPosition(target->getPosition());
                    GM->setPathState(targetPos.x, targetPos.y, false);
                }
            }
            // check ground under the feet occupied
            bool isGroundUnderTheFeetOccupied = false;
            Vec2 pos = WORLD->getCoordinateFromPosition(getPosition(), WORLD->theMap);
            Movable* theObjectThisIsIn = nullptr;
            if (GM->tileState[(int)pos.x][(int)pos.y] > 0) {
                theObjectThisIsIn = WORLD->getGroundOwner(getPosition());
                if (theObjectThisIsIn == nullptr) {
                    stopNew();
                }else{
                    isGroundUnderTheFeetOccupied = true;
                    if(theObjectThisIsIn->isBuilding){
                        for (int j = 0; j < theObjectThisIsIn->buildingOccupySize.height; j++) {
                            for (int i = 0; i < theObjectThisIsIn->buildingOccupySize.width; i++) {
                                GM->setPathState(theObjectThisIsIn->buildingStartCoordinate.x + i, theObjectThisIsIn->buildingStartCoordinate.y + j, false);
//                                log("unlock block: %f, %f", theObjectThisIsIn->buildingStartCoordinate.x + i, theObjectThisIsIn->buildingStartCoordinate.y + j);
                            }
                        }
                    }else if(theObjectThisIsIn->unitType == UNIT_TREE || theObjectThisIsIn->unitType == UNIT_TREE_FOR_BATTLE){
                        Point targetPos = WORLD->getCoordinateFromPosition(theObjectThisIsIn->getPosition());
                        GM->setPathState(targetPos.x, targetPos.y, false);
                    }
                }
            }
            if(moveToPos != moveFlagPos){
                moveToPos = moveFlagPos;
            }
            if (isFlying || isTemporaryFlying || !WORLD->isBlockExistBetween(getPosition(), moveToPos)) {
                unitActDetail = UNIT_ACT_DETAIL_WALK_STRAIGHT_TO_TARGET;
                array = PointArray::create(1);//GM->getPath(selfPos, targetMoveTilePos);
                array->addControlPoint(moveToPos);
            }else{
                unitActDetail = UNIT_ACT_DETAIL_WALK_ROUTE;
                
                array = GM->getPath(selfPos, targetMoveTilePos);
                
                if(!array || array->count() == 0){
                    if(target){
                        unreachableTarget = target;
                    }
                    failedFindPathStart = selfPos;
                    failedFindPathEnd = targetMoveTilePos;
                    if(targetMoveTilePos == attackFlagTilePos){
                        failedAttackFlagPos = attackFlagPos;
                        attackFlagPos = Vec2::ZERO;
                    }
                    stopNew();
//                    return;
                }
            }
            if (target) { // putback target occupy the ground
                if(target->isBuilding){
                    for (int j = 0; j < target->buildingOccupySize.height; j++) {
                        for (int i = 0; i < target->buildingOccupySize.width; i++) {
                            GM->setPathState(target->buildingStartCoordinate.x + i, target->buildingStartCoordinate.y + j, true);
                        }
                    }
                }else if(target->unitType == UNIT_TREE || target->unitType == UNIT_TREE_FOR_BATTLE){
                    Point targetPos = WORLD->getCoordinateFromPosition(target->getPosition());
                    GM->setPathState(targetPos.x, targetPos.y, true);
                }
            }
            if(isGroundUnderTheFeetOccupied){ // putback theBuildingThisIsIn occupied ground
                if(theObjectThisIsIn->isBuilding){
                    for (int j = 0; j < theObjectThisIsIn->buildingOccupySize.height; j++) {
                        for (int i = 0; i < theObjectThisIsIn->buildingOccupySize.width; i++) {
                            GM->setPathState(theObjectThisIsIn->buildingStartCoordinate.x + i, theObjectThisIsIn->buildingStartCoordinate.y + j, true);
                        }
                    }
                }else if(theObjectThisIsIn->unitType == UNIT_TREE || theObjectThisIsIn->unitType == UNIT_TREE_FOR_BATTLE){
                    Point targetPos = WORLD->getCoordinateFromPosition(theObjectThisIsIn->getPosition());
                    GM->setPathState(targetPos.x, targetPos.y, true);
                }
            }
            
            Vec2 targetPos = moveToPos;
            int startI = array->count()-1;
            if(startI > 1){
                startI--;
            }
            for (int i = startI; i >= 0; i--) {
                pos = array->getControlPointAtIndex(i);
                addRoute(WORLD->getPositionFromTileCoordinate(pos.x, pos.y));
            }
            if (routePositionArray != nullptr && routePositionArray->count() > 0) {
                if(!WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(WORLD->getCoordinateFromPosition(targetPos)))){
                    routePositionArray->removeControlPointAtIndex(routePositionArray->count() - 1); // remove last tile pos
                    addRoute(targetPos); // test
                }
                if(routePositionArray->count() > 2){
                    routePositionArray->removeControlPointAtIndex(0);
                }
            }else{
                if(target != nullptr && attackType == ATTACK_TYPE_RANGE && attackRange > getPosition().distanceSquared(target->getPosition())){
                    
                }else{
                    if(!isZombie){
                        attackFlagPos = Point::ZERO;
                        stop();
                    }
                }
            }
        }
        if (unitAct == UNIT_ACT_ATTACK) {
            if(!target || target->energy <= 0){
//                attackCoolTime = 0;
                target = nullptr;
                stopNew();
                return;
            }else{
                if (moveFlagPos != target->getPosition()) {
//                    if (target->isBuilding) {
//                        moveFlagPos = target->getApproachingPoint(getPosition());
//                    }else{
                        moveFlagPos = target->getPosition();
//                    }
                    moveToPos = moveFlagPos;
                }
            }
        }
        bool isArrived = false;
        if(unitActDetail == UNIT_ACT_DETAIL_WALK_ROUTE){
            isArrived = movePointCounter >= routePositionArray->count();
        }else if(unitActDetail == UNIT_ACT_DETAIL_WALK_STRAIGHT_TO_TARGET){
            isArrived = getPosition() == moveToPos;
        }
        if (isArrived && unitActDetail != UNIT_ACT_DETAIL_ATTACK && unitAct != UNIT_ACT_GATHER_GOLD){ // arrived
            attackFlagPos = Vec2::ZERO;
            if (unitAct == UNIT_ACT_ATTACK) {
                checkAttackTargetReturnSuccess(dt);
            }else{
                stopNew();
            }
            
//            if(isGatheringTree){
//                if(isCarryingTree){
//                    EnemyBase* tank = WORLD->getNearestLumberTank(getPosition());
//                    unitName = strmake("%sWood", getName().c_str());
//                    if(tank != nullptr){
//                        returningPlace = tank;
//                        moveToTarget(tank);
////                        runMoveAnimation(DIRECTION_E);
//                    }
//                }else{
//                    EnemyBase* tree = WORLD->getNearestTree(getPosition());
//                    if(tree != nullptr){
//                        gatherTree(tree);
//                    }
//                }
//            }
            if (isEnemy && WORLD->heroArray.size() > 0) {
                EnemyBase* targetEnemy = nullptr;
                for(auto unit: WORLD->heroArray){
                    if(unit->isFlying){
                        continue;
                    }
                    targetEnemy = unit;
                    break;
                }
                if(targetEnemy != nullptr){
                    Vec2 pos = WORLD->heroArray.at(0)->getPosition();
                    if(failedAttackFlagPos != pos){
                        attackFlagPos = pos;
                        attackFlagTilePos = WORLD->getCoordinateFromPosition(attackFlagPos, WORLD->theMap);
                    }
                }
            }
        }else{
            if (isVisible() && (unitActDetail == UNIT_ACT_DETAIL_WALK_ROUTE || unitActDetail == UNIT_ACT_DETAIL_WALK_STRAIGHT_TO_TARGET)) {
                Point dest, current;
                if (unitActDetail == UNIT_ACT_DETAIL_WALK_ROUTE) {
                    dest = routePositionArray->getControlPointAtIndex(movePointCounter);
                }else{
                    dest = moveToPos;
                }
                current = this->getPosition();
                float xToMove = dest.x - current.x;
                float yToMove = dest.y - current.y;
                float xDistanceToMove = xToMove;
                if(xDistanceToMove < 0){
                    xDistanceToMove *= -1;
                }
                float yDistanceToMove = yToMove;
                if(yDistanceToMove < 0){
                    yDistanceToMove *= -1;
                }
                float totalDistance = xDistanceToMove + yDistanceToMove;
                float xWillMove = extraSpeed*0.01f*(xDistanceToMove/totalDistance)*(xToMove<0?-1:1);
                float yWillMove = extraSpeed*0.01f*(yDistanceToMove/totalDistance)*(yToMove<0?-1:1);
                if((xToMove == 0 && yToMove == 0) ||
                   (xToMove > 0 && xWillMove >= xToMove) ||
                   (xToMove < 0 && xWillMove <= xToMove) ||
                   (yToMove > 0 && yWillMove >= yToMove) ||
                   (yToMove < 0 && yWillMove <= yToMove)){
                    this->setPosition(dest);
                    movePointCounter++;
                }else{
                    this->setPosition(current + Point(xWillMove, yWillMove));
                }
                if (unitAct == UNIT_ACT_ATTACK) {
                    checkAttackTargetReturnSuccess(dt);
                }
                if (sptSelectedCircle != nullptr) {
                    sptSelectedCircle->setPosition(getEffectStartPosition());
                }
                if (ndLevelCircle != nullptr) {
                    ndLevelCircle->setPosition(getEffectStartPosition());
                }
                
                this->setFlippedX(dest.x > current.x);
                if(this->unitType == UNIT_CATAPULT){
                    int angle = 10;
                    if(dest.y > current.y){
                        this->setRotation(isTowardLeft?angle:-angle);
                    }else if(dest.y < current.y){
                        this->setRotation(isTowardLeft?-angle:angle);
                    }else{
                        this->setRotation(0);
                    }
                }
            }
        }
    }
    // handle attack
    if (unitAct == UNIT_ACT_ATTACK){
        if (!target) {
//            attackCoolTime = 0;
            target = nullptr;
            stopNew();
            return;
        }else{
            checkAttackTargetReturnSuccess(dt);
        }
    }else if (unitAct == UNIT_ACT_ATTACK_DDANG) {
        checkAttackTargetReturnSuccess(dt);
    }
    else if(unitAct == UNIT_ACT_NONE && previousAct == UNIT_ACT_ATTACK){
        checkAttackTargetReturnSuccess(dt);
    }
    if (unitActDetail == UNIT_ACT_DETAIL_ATTACK) {
        
    }else{
        if (prePos != getPosition()) {
            runAnimation(ANIMATION_TYPE_MOVE);
        }else{
            runAnimation(ANIMATION_TYPE_IDLE);
        }
    }
    
    // energy bar
    if(energyBar != nullptr){
        energyBar->setPosition(getPosition() + Point(0, energyBarY));
        energyBarContent->setPosition(energyBar->getPosition() + Point(-37, 0));
        energyBarBack->setPosition(energyBar->getPosition());
    }
    
    // gathering gold
    if (isGatheringGold) {
        if (returningPlace != nullptr && isCarryingGold && returningPlace->getBoundingBoxForIntersect().containsPoint(getPosition())){
            WORLD->addGold(40);
            this->attackType = ATTACK_TYPE_NEAR;
            isCarryingGold = false;
            this->canFindTarget = true;
            
            unitName = getName();
            gatherGold((Movable*)currentMine);
            currentAnimationType = ANIMATION_TYPE_IDLE;
            runAnimation(ANIMATION_TYPE_MOVE);
        }else if(currentMine == nullptr){
            
        }
        if(!currentMine || currentMine == nullptr){
            return;
        }
        Rect rect = currentMine->getBoundingBoxForIntersect();
        Vec2 pos = getPosition();
        if (!isCarryingGold && rect.containsPoint(pos)){
            if(currentMine->energy <= 0){
                this->stopNew();
                this->setVisible(true);
                this->untouchable = false;
                currentMine = nullptr;
                isGatheringGold = false;
                return;
            }
            if(this->isVisible()){
                this->setVisible(false);
                this->stopNew();
                this->untouchable = true;
                this->attackType = ATTACK_TYPE_NONE;
                this->unitAct = UNIT_ACT_GATHER_GOLD;
                this->unitAct = UNIT_ACT_ATTACK;
                this->canFindTarget = false;
                WORLD->deselect(this);
                int miner = currentMine->getTag();
                if (miner == 0) {
                    currentMine->setSpriteFrame("mine1.png");
                }
                miner++;
                currentMine->setTag(miner);
            }
            
            gatheringTimer -= dt;
            if (gatheringTimer < 0) {
                if(currentMine->energy > 0){
                    if(currentMine->getHitAndIsDead(40, this)){ // gold gather
                        WORLD->removeDeadUnit((EnemyBase*)currentMine);
                        currentMine = nullptr;
                    }
                    isCarryingGold = true;
                    this->setVisible(true);
                    this->untouchable = false;
                    EnemyBase* castle = WORLD->getNearestCastle(getPosition());
                    unitName = strmake("%sGold", getName().c_str());
                    if(castle != nullptr && currentMine != nullptr){
//                        moveToTarget(castle);
                        returningPlace = castle;
                        target = returningPlace;
                        moveToPos = Vec2::ZERO;
                        moveFlagPos = target->getPosition();
                        unitAct = UNIT_ACT_GATHER_GOLD;
                        unitActDetail = UNIT_ACT_DETAIL_IDLE;
                        int miner = currentMine->getTag();
                        miner--;
                        currentMine->setTag(miner);
                        if (miner == 0) {
                            currentMine->setSpriteFrame("mine.png");
                        }
                    }
                }else{
                    this->stopNew();
                    this->setVisible(true);
                    this->untouchable = false;
                    isGatheringGold = false;
                }
            }
        }
    }
    
    // gathering tree
    if (isGatheringTree) {
        if (!isCarryingTree && currentTree != nullptr && currentTree->getBoundingBoxForIntersect().intersectsRect(this->getBoundingBoxForIntersect())) {
            unitName = getName();
            attackCoolTime -= dt;
            
            if(attackCoolTime < 0){
                attackCoolTime = attackCoolTimeMax;
            }else{
                return;
            }
            
            runAnimation(ANIMATION_TYPE_ATTACK);
            unitActDetail = UNIT_ACT_DETAIL_ATTACK;
            if(canMove)
                this->setFlippedX(getPositionX() < currentTree->getPositionX());
            this->runAction(Sequence::create(DelayTime::create(attackHappenTime), CallFunc::create(CC_CALLBACK_0(Movable::attackTree, this)), nullptr));
            this->runAction(Sequence::create(DelayTime::create(0.26f), CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)), nullptr));
            return;
        }else if (returningPlace != nullptr && isGatheringTree && isCarryingTree  && returningPlace->getBoundingBox().containsPoint(getPosition())){
            isCarryingTree = false;
            stop();
            unitName = getName();
            runAnimation(ANIMATION_TYPE_IDLE);
            WORLD->addLumber(40);
            
            Movable* tree = WORLD->getNearestTree(getPosition());
            gatherTree(tree);
            
            return;
        }
    }
    
//    if ((scheduledProductTime >= 0 || scheduledAttackTime >= 0) && WORLD->gameTimer > 240 && WORLD->gameTimer < 2400) {
    if ((scheduledProductTime >= 0 || scheduledAttackTime >= 0)) {
        timeCollapse += dt;// test
//        timeCollapse += dt*20;// test 
        if(scheduledAttackTime >= 0){
            if (scheduledAttackTime < timeCollapse) {
                scheduledAttackTime = -1;
                this->wantToEli = true;
                WORLD->attackNearHero((EnemyBase*)this);
            }
        }else if(scheduledProductTime >= 0){
            if (scheduledProductTime < timeCollapse) {
                timeCollapse -= scheduledProductTime;
                for(int i = 0;i < scheduledProductUnitCount; i++){
                    int unitType = scheduledProductUnit;
                    if(unitType == UNIT_ZOMBIE_SWORDSMAN){
                        unitType = rand()%2==0?UNIT_ZOMBIE_SWORDSMAN:UNIT_ZOMBIE_ORC_AXE;
                    }
                    EnemyBase* createdUnit = WORLD->createUnit(unitType, isEnemy, false, this->getPosition() + Point(i*100, - 300), GM->getUnitName(unitType));
                    createdUnit->wantToEli = true;
                }
            }
        }
    }
    
    // movenew end
}
bool Movable::checkAttackTargetReturnSuccess(float dt){
    if(isInAttackMotion){
        if(unitActDetail == UNIT_ACT_DETAIL_ATTACK){
            return true;
        }else{
            isInAttackMotion = false;
        }
    }
    if (target != nullptr && (isEnemy != target->isEnemy || forceAttackTarget || target->unitType == UNIT_TREE_FOR_BATTLE) && attackType != ATTACK_TYPE_NONE) {
        if (attackType == ATTACK_TYPE_RANGE){
            if (target->getPosition().distanceSquared(getPosition()) < attackRange){
                // attack
                attackUpdate(dt);
                return true;
            }else{
                if (unitActDetail == UNIT_ACT_DETAIL_ATTACK) {
                    unitActDetail = UNIT_ACT_DETAIL_IDLE;
                    moveToPos = Vec2::ZERO;
                }
                if (isBuilding) {
                    target = nullptr;
                }
            }
        }else{
            if(this->getBoundingBoxForIntersect().intersectsRect(target->getBoundingBoxForIntersect())){
                attackUpdate(dt);
                return true;
            }else{
                if (unitActDetail == UNIT_ACT_DETAIL_ATTACK) {
                    unitActDetail = UNIT_ACT_DETAIL_IDLE;
                    moveToPos = Vec2::ZERO;
                }
            }
        }
    }
    return false;
}
void Movable::move(float dt){
    oneSecTimeChecker += dt;
    if (oneSec < oneSecTimeChecker) {
        oneSecTimeChecker = 0;
        if (isZombie && isVisible()) { 
            if(target == nullptr && (routePositionArray == nullptr || routePositionArray->count() == 0)){
                Point minPos;
                bool shouldMove = false;
                int minLength = 999999;
                for (int i = -1; i < 2; i++) {
                    for (int j = -1; j < 2; j++) {
                        if (i == 0 && j == 0) {
                            continue;
                        }
                        Point newPos = Point(getPositionX() + i*TILE_SIZE, getPositionY() + j*TILE_SIZE);;
                        if (minLength > abs(getPositionX() - attackFlagPos.x + i*TILE_SIZE) + abs(getPositionY() - attackFlagPos.y + j*TILE_SIZE) && !WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(WORLD->getCoordinateFromPosition(newPos))) && WORLD->isSpotAvailable(isEnemy, newPos)) {
                            minPos = newPos;
                            minLength = abs(getPositionX() - attackFlagPos.x + i*TILE_SIZE) + abs(getPositionY() - attackFlagPos.y + j*TILE_SIZE);
                            shouldMove = true;
                        }
                    }
                }
                if(shouldMove){
                    moveToTarget(minPos);
                }
            }
        }
    }
    
    if ((scheduledProductTime >= 0 || scheduledAttackTime >= 0) && WORLD->gameTimer > 240 && WORLD->gameTimer < 2400) {
//    if ((scheduledProductTime >= 0 || scheduledAttackTime >= 0) && WORLD->gameTimer > 0 && WORLD->gameTimer < 2400) { // test
        timeCollapse += dt;// test
        if(scheduledAttackTime >= 0){
            if (scheduledAttackTime < timeCollapse) {
                scheduledAttackTime = -1;
                this->wantToEli = true;
//                for(auto unit: WORLD->heroArray){
//                    if(unit->unitType == UNIT_CASTLE){
//                        WORLD->moveAndAttackTo((EnemyBase*)this, unit->getApproachingPoint(getPosition()));
//                        break;
//                    }
//                }
                WORLD->attackNearHero((EnemyBase*)this);
            }
        }else if(scheduledProductTime >= 0){
            if (scheduledProductTime < timeCollapse) {
                timeCollapse -= scheduledProductTime;
//                for(auto unit: WORLD->heroArray){
//                    if(unit->unitType == UNIT_CASTLE){
                        for(int i = 0;i < scheduledProductUnitCount; i++){
                            int unitType = scheduledProductUnit;
                            if(unitType == UNIT_ZOMBIE_SWORDSMAN){
                                unitType = rand()%2==0?UNIT_ZOMBIE_SWORDSMAN:UNIT_ZOMBIE_ORC_AXE;
                            }else if(unitType == UNIT_ORC_AXE){
                                unitType = rand()%2==0?UNIT_ORC_AXE:UNIT_ORC_SPEAR;
                            }
                            EnemyBase* createdUnit = WORLD->createUnit(unitType, isEnemy, false, this->getPosition() + Point(i*50, - 300), GM->getUnitName(unitType));
//                            WORLD->moveAndAttackTo((EnemyBase*)createdUnit, unit->getApproachingPoint(createdUnit->getPosition()));
                            createdUnit->wantToEli = true;
                        }
                        
//                        break;
//                    }
//                }
            }
        }
    }
    if(energyBar != nullptr){
        if(energyBarY < 0){
            if(spine){
                energyBarY = this->spine->getBoundingBox().size.height*0.9f;
            }else{
                energyBarY = this->getBoundingBox().size.height*0.9f;
            }
        }
        
        energyBar->setPosition(getPosition() + Point(0, energyBarY));
        energyBarContent->setPosition(energyBar->getPosition() + Point(-37, 0));
        energyBarBack->setPosition(energyBar->getPosition());
    }
    if (isGatheringGold) {
        if (returningPlace != nullptr && isCarryingGold && returningPlace->getApproachingPoint(getPosition()) == getPosition()){//returningPlace->getBoundingBox().containsPoint(this->getPosition())) {//returningPlace->getBoundingBox().intersectsRect(this->getBoundingBox())) {
            WORLD->addGold(40);
            this->attackType = ATTACK_TYPE_NEAR;
            isCarryingGold = false;
            this->canFindTarget = true;
//            this->setName("worker");
//            this->setName(getName().substr(0, getName().size() - 4));
            unitName = getName();
            gatherGold((Movable*)currentMine);
        }else if(currentMine == nullptr){
            
        }else if (!isCarryingGold && currentMine->getApproachingPoint(getPosition()) == getPosition()){
            if(currentMine->energy <= 0){
                this->stop();
                this->setVisible(true);
                this->untouchable = false;
                currentMine = nullptr;
                isGatheringGold = false;
                return;
            }
            if(this->isVisible()){
                this->setVisible(false);
                this->stop();
                this->untouchable = true;
                this->attackType = ATTACK_TYPE_NONE;
                this->canFindTarget = false;
                WORLD->deselect(this);
                int miner = currentMine->getTag();
                if (miner == 0) {
                    currentMine->setSpriteFrame("mine1.png");
                }
                miner++;
                currentMine->setTag(miner);
            }
            
            gatheringTimer -= dt;
            if (gatheringTimer < 0) {
                if(currentMine->energy > 0){
                    if(currentMine->getHitAndIsDead(40, this)){ // gold gather
                        WORLD->removeDeadUnit((EnemyBase*)currentMine);
                        currentMine = nullptr;
                    }
                    isCarryingGold = true;
                    this->setVisible(true);
                    this->untouchable = false;
                    EnemyBase* castle = WORLD->getNearestCastle(getPosition());
//                    this->setName("workerGold");
//                    this->setName(strmake("%sGold", getName().c_str()));
                    unitName = strmake("%sGold", getName().c_str());
                    if(castle != nullptr && currentMine != nullptr){
//                        isTemporaryFlying = true;
                        moveToTarget(castle);
//                        runMoveAnimation(DIRECTION_E);
                        returningPlace = castle;
                        int miner = currentMine->getTag();
                        miner--;
                        currentMine->setTag(miner);
                        if (miner == 0) {
                            currentMine->setSpriteFrame("mine.png");
                        }
                    }
                }else{
                    this->stop();
                    this->setVisible(true);
                    this->untouchable = false;
                    isGatheringGold = false;
                }
            }
        }
    }
    if (isGatheringTree) {
        if (!isCarryingTree && currentTree != nullptr && currentTree->getBoundingBox().intersectsRect(this->getBoundingBoxForIntersect())) {
//            this->setName("worker");
//            this->setName(getName().substr(0, getName().size() - 4));
            unitName = getName();
            attackCoolTime -= dt;
            if(!currentTree || ((EnemyBase*)currentTree)->energy <= 0){
                stopNew();
                return;
            }
            
            if(attackCoolTime < 0){
                attackCoolTime = attackCoolTimeMax;
            }else{
                return;
            }
            
            runAnimation(ANIMATION_TYPE_ATTACK);
            
            if(canMove)
                this->setFlippedX(getPositionX() < currentTree->getPositionX());
            this->runAction(Sequence::create(DelayTime::create(attackHappenTime), CallFunc::create(CC_CALLBACK_0(Movable::attackTree, this)), nullptr));
            this->runAction(Sequence::create(DelayTime::create(0.26f), CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)), nullptr));
            return;
        }else if (returningPlace != nullptr && isGatheringTree && isCarryingTree  && returningPlace->getApproachingPoint(getPosition()) == getPosition()){//&& returningPlace->getBoundingBox().containsPoint(getPosition())) {
            isCarryingTree = false;
            stop();
//            this->setName("worker");
//            this->setName(getName().substr(0, getName().size() - 4));
            unitName = getName();
            WORLD->addLumber(40);
            
//            WORLD->moveTo((EnemyBase*)this, lastTreePos);
            
            Movable* tree = WORLD->getNearestTree(getPosition());
            gatherTree(tree);
//            if (routePositionArray == nullptr || routePositionArray->count() <= 1) {
//                tree->unitType = UNIT_UNREACHABLE_TREE;
//                Movable* newTree = WORLD->getNearestTree(getPosition());
//                gatherTree(newTree);
//                tree->unitType = UNIT_TREE;
////                while((tree = WORLD->getNearestTree(getPosition())) != nullptr){
////
////                }
//            }
            
            return;
        }
    }
    
    if(target == nullptr && attackFlagPos == Point::ZERO){
        return;
    }
    
    if(delayFrameTimeBeforeMove > 0){
        delayFrameTimeBeforeMove--;
        return;
    }
    targetPositionCheckTime -= dt;
    if(targetPositionCheckTime < 0 && target != nullptr && target->canMove && !target->untouchable && canMove && !target->isGatheringGold && !target->isGatheringTree){
        targetPositionCheckTime = 1;
        
//        Point newTargetPos = WORLD->getCoordinateFromPosition(target->getBoundingBox().origin + Point(target->getBoundingBox().size.width/2, target->collisionBoundingBox().size.height/2), WORLD->theMap);
        Point newTargetPos;
//        if(isFlying || isTemporaryFlying){
//            newTargetPos = target->getPosition();
//        }else{
            newTargetPos = WORLD->getCoordinateFromPosition(target->getApproachingPoint(getPosition()), WORLD->theMap);
//        }
        if(newTargetPos != targetCoordinate){
            stop();
            moveToTarget();
            return;
        }
    }
    
    if (target != nullptr && (isEnemy != target->isEnemy || forceAttackTarget) && attackType != ATTACK_TYPE_NONE) {
        if (attackType == ATTACK_TYPE_RANGE){
            if (target->getPosition().distanceSquared(getPosition()) < attackRange){
                // attack
                attackUpdate(dt);
                return;
            }else{
                if (isBuilding) {
                    target = nullptr;
                }
            }
        }else{
            if(this->getBoundingBoxForIntersect().intersectsRect(target->getBoundingBoxForIntersect())){
//            if(target->getBoundingBox().containsPoint(this->getPosition())){
                attackUpdate(dt);
            }
        }
    }
    if(routePositionArray== nullptr){
        return;
    }
    if(movePointCounter >= routePositionArray->count()){ // arrived
        stop();
        unitActDetail = UNIT_ACT_DETAIL_IDLE;
        if(isGatheringTree){
            if(isCarryingTree){
                EnemyBase* tank = WORLD->getNearestLumberTank(getPosition());
//                this->setName("workerWood");
//                this->setName(strmake("%sWood", getName().c_str()));
                unitName = strmake("%sWood", getName().c_str());
                if(tank != nullptr){
                    returningPlace = tank;
//                    isTemporaryFlying = true;
                    moveToTarget(tank);
//                    runMoveAnimation(DIRECTION_E);
                }
            }else{
                EnemyBase* tree = WORLD->getNearestTree(getPosition());
                if(tree != nullptr){
                    gatherTree(tree);
                }
            }
        }
        if (isEnemy && WORLD->heroArray.size() > 0) {
            EnemyBase* targetEnemy = nullptr;
            for(auto unit: WORLD->heroArray){
                if(unit->isFlying){
                    continue;
                }
                targetEnemy = unit;
                break;
            }
            if(targetEnemy != nullptr){
                Vec2 pos = WORLD->heroArray.at(0)->getPosition();
                if(failedAttackFlagPos != pos){
                    attackFlagPos = pos;
                    attackFlagTilePos = WORLD->getCoordinateFromPosition(attackFlagPos, WORLD->theMap);
                }
            }
        }
//        log("what?");
//        targetCoordinate = Point::ZERO;
//        resetRoute();
//        moveToTarget();
    }else{
        Point dest = routePositionArray->getControlPointAtIndex(movePointCounter);
        Point current = this->getPosition();
        this->setFlippedX(dest.x > current.x);
        if(this->unitType == UNIT_CATAPULT){
            int angle = 10;
            if(dest.y > current.y){
                this->setRotation(isTowardLeft?angle:-angle);
            }else if(dest.y < current.y){
                this->setRotation(isTowardLeft?-angle:angle);
            }else{
                this->setRotation(0);
            }
        }
        
        float xToMove = dest.x - current.x;
        float yToMove = dest.y - current.y;
        float xDistanceToMove = xToMove;
        if(xDistanceToMove < 0){
            xDistanceToMove *= -1;
        }
        float yDistanceToMove = yToMove;
        if(yDistanceToMove < 0){
            yDistanceToMove *= -1;
        }
        float totalDistance = xDistanceToMove + yDistanceToMove;
        float slowRate = 1;
//        if(slowFrameTime > 0){
//            slowFrameTime--;
//            slowRate = 0.5f;
//            GameManager::getInstance()->makeAdditive(this, Color3B(100, 100, 230));
//        }
//        else{
//            GameManager::getInstance()->clearAdditive(this);
//        }
        float xWillMove = extraSpeed*0.01f*slowRate*(xDistanceToMove/totalDistance)*(xToMove<0?-1:1);
        float yWillMove = extraSpeed*0.01f*slowRate*(yDistanceToMove/totalDistance)*(yToMove<0?-1:1);
        //        log("xWillMove: %f, yWillMove: %f", xWillMove, yWillMove);
        //        xWillMove *= 0.1f;
        //        yWillMove *= 0.1f;
        
        if((xToMove == 0 && yToMove == 0) ||
           (xToMove > 0 && xWillMove >= xToMove) ||
           (xToMove < 0 && xWillMove <= xToMove) ||
           (yToMove > 0 && yWillMove >= yToMove) ||
           (yToMove < 0 && yWillMove <= yToMove)){
            this->setPosition(dest);
            movePointCounter++;
        }else{
            this->setPosition(current + Point(xWillMove, yWillMove));
            
        }
        if (sptSelectedCircle != nullptr) {
            sptSelectedCircle->setPosition(getEffectStartPosition());
        }
        if (ndLevelCircle != nullptr) {
            ndLevelCircle->setPosition(getEffectStartPosition());
        }
    }
}
Rect Movable::getBoundingBoxForIntersect(){
    if (spine || isBuilding) {
        return Sprite::getBoundingBox();
    }else{
        return Rect(getPositionX()-50, getPositionY()-30, 100, 60);
    }
}
//Rect Movable::getBoundingBox() const {
//    if (spine || isBuilding) {
//        return Sprite::getBoundingBox();
//    }else{
//        return Rect(getPositionX()-50, getPositionY()-30, 100, 60);
//    }
//}
Vec2 Movable::getEffectStartPosition(){
    if(isBuilding){
        return getPosition() + Point(0, -getContentSize().height/2 + 25);
    }else{
        return getPosition() + Point(0, 14);//Point(0, -getContentSize().height/2 + 30);
    }
}
void Movable::playTreeSound(){
//    GM->playSoundEffect(SOUND_HIT); // test
    if(WORLD->isInScreen(getPosition())){
        GM->playSoundEffect(SOUND_WOOD_HIT);
    }else{
        GM->playSoundEffect(SOUND_WOOD_HIT, 0.1f);
    }
}
void Movable::setGroundEffect(int effect){
    groundEffectType = effect;
    if(effect == GROUND_EFFECT_LIGHT){
        groundEffect = Sprite::createWithSpriteFrameName("lightPlatform.png");
    }else if(effect == GROUND_EFFECT_DARK){
        groundEffect = Sprite::createWithSpriteFrameName("darkPlatform.png");
    }
    this->getParent()->addChild(groundEffect, -1000);
    this->schedule(schedule_selector(Movable::updateGroundEffect), 0.3f);
}
void Movable::updateGroundEffect(float dt){
    if(groundEffect == nullptr){
        return;
    }
    Sprite* spt;
    for(int i = 0; i < 2; i++){
        if(groundEffectType == GROUND_EFFECT_LIGHT){
            spt = Sprite::createWithSpriteFrameName("lightPixel.png");
        }else if(groundEffectType == GROUND_EFFECT_DARK){
            spt = Sprite::createWithSpriteFrameName("darkPixel.png");
        }
        this->getParent()->addChild(spt);
        spt->setPosition(this->getPosition() + Point(-7 + rand()%16, 4 - rand()%9));
        spt->runAction(Sequence::create(MoveBy::create(1.5f, Point(0, 32)), SPT_REMOVE_FUNC, NULL));
        spt->runAction(Sequence::create(DelayTime::create(1.0f), FadeOut::create(0.5f), NULL));
    }
}

void Movable::updateEnergy(){
    if(energyBar == nullptr){
//        SpriteBatchNode* spriteBatch = WORLD->spriteBatch;
        Node* spriteBatch = WORLD->spriteBatch;
        if(this->isBuilding){
            spriteBatch = WORLD->spriteBatchBuilding;
            energyBarBack = Sprite::createWithSpriteFrameName("energyBarContentForBuilding.png");
            energyBarContent = Sprite::createWithSpriteFrameName("energyBarContentForBuilding.png");
            energyBar = Sprite::createWithSpriteFrameName("energyBarFrame0ForBuilding.png");
            GM->runAnimation(energyBar, "energyBarFrameForBuilding", true);
        }else{
            energyBarBack = Sprite::createWithSpriteFrameName("energyBarContent.png");
            energyBarContent = Sprite::createWithSpriteFrameName("energyBarContent.png");
            energyBar = Sprite::createWithSpriteFrameName("energyBarFrame0.png");
            GM->runAnimation(energyBar, "energyBarFrame", true);
        }
        
        spriteBatch->addChild(energyBarBack);
        energyBarBack->setColor(Color3B(38, 44, 52));
        
        spriteBatch->addChild(energyBarContent);
        energyBarContent->setAnchorPoint(Point(0, 0.5));
        
        spriteBatch->addChild(energyBar);

        if(energyBarY < 0){
            if(spine){
                energyBarY = this->spine->getBoundingBox().size.height*0.9f;
            }else if(isBuilding){
                energyBarY = this->getBoundingBox().size.height*0.4f;
            }else{
                energyBarY = this->getBoundingBox().size.height*0.9f;
            }
        }
        
        energyBar->setPosition(getPosition() + Point(0, energyBarY));
        energyBarContent->setPosition(energyBar->getPosition() + Point(-37, 0));
        energyBarBack->setPosition(energyBar->getPosition());
    }
    float rate = energy*1.0f/maxEnergy;
    Rect textureRect = energyBarContent->getTextureRect();
    energyBarContent->setTextureRect(Rect(textureRect.origin.x, textureRect.origin.y, 74*rate, 24));
    if(rate > 0.7f){
        energyBarContent->setColor(Color3B(141, 254, 33));
    }else if(rate > 0.3f){
        energyBarContent->setColor(Color3B(244, 236, 5));
    }else{
        energyBarContent->setColor(Color3B(244, 30, 5));
    }
}
void Movable::makingSmoke(float dt){
    Sprite* spt = Sprite::createWithSpriteFrameName("smoke0.png");
    GM->runAnimation(spt, "smoke", true);
    spt->setPosition(smokePoint);
    float dur = 5;
    spt->setScale(0.2f);
    spt->setRotation(rand()%50 - 25);
    spt->runAction(Sequence::create(ScaleTo::create(0.1f, 1), DelayTime::create(1), ScaleTo::create(3, 1.5f), nullptr));
    spt->runAction(Sequence::create(MoveBy::create(dur, Point(200, 0)), SPT_REMOVE_FUNC, NULL));
    spt->runAction(Sequence::create(EaseOut::create(MoveBy::create(dur*2/3, Point(0, 200)), 2), MoveBy::create(dur/3, Point(0, 10)), nullptr));
    spt->runAction(Sequence::create(DelayTime::create(dur*3/4), FadeOut::create(dur/4), NULL));
    this->getParent()->addChild(spt);
}
void Movable::blinkSelectedCircle(){
    showSelectedCircle(true);
    showSelectedCircle(false);
    sptSelectedCircle->runAction(Blink::create(3, 3));
}
void Movable::showSelectedCircle(bool show){
    if (show) {
        if (sptSelectedCircle == nullptr) {
            if(isBuilding){
                sptSelectedCircle = Sprite::createWithSpriteFrameName("selectedCircleBuilding.png");
                WORLD->spriteBatchBuilding->addChild(sptSelectedCircle, -20000);
            }else{
                sptSelectedCircle = Sprite::createWithSpriteFrameName("selectedCircle.png");
                WORLD->spriteBatch->addChild(sptSelectedCircle, -20000);
            }
            if(spine){
                sptSelectedCircle->setScale((this->getContentSize().width)*1.5f/sptSelectedCircle->getContentSize().width);
            }else{
                sptSelectedCircle->setScale((this->getContentSize().width)/sptSelectedCircle->getContentSize().width);
            }
            
            if(isEnemy){
                sptSelectedCircle->setColor(Color3B(244, 30, 5));
            }else{
                if (isAlli) {
                    sptSelectedCircle->setColor(Color3B(244, 236, 5));
                }else{
                    sptSelectedCircle->setColor(Color3B(141, 254, 33));
                }
            }
        }
        sptSelectedCircle->stopAllActions();
        sptSelectedCircle->setVisible(true);
        sptSelectedCircle->setPosition(getEffectStartPosition());
    }else{
        if (sptSelectedCircle != nullptr) {
            sptSelectedCircle->setVisible(false);
        }
    }
}
