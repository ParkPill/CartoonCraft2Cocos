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
#include "MultiplayManager.h"
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
    velocity = Vec2(0,-100);
    jumped = false;
    blinkingTime = 0;
    freezed = false;
    immortal = false;
    centerPosition = Vec2(0,0);
    
    untouchable = false;
    onGround = true;
    memset(cargoGrid, 0, sizeof(cargoGrid));
//    wantsToStopHorizontal = false;
//    wantsToStopVertical = false;
    wantsToGoLeft = false;
    wantsToGoRight = false;
    wantsToJump = false;
    weight = 1;
    target = NULL;
    effectType = 0;
    iWasInWater = false;
    if(routePositionArray == nullptr){
        routePositionArray = PointArray::create(200);
        routePositionArray->retain();

    }else{

    }
}
void Movable::init(int unit, int eng, float extraSpd, const char* sptName)
{
    if(unit == UNIT_HERO_BATMONSTER){
        energyBarY += 50;
    }
    std::string strSpineFileName = GM->getSpineFileName(unit);
//    if (unit >= UNIT_HERO_ORC && unit <= UNIT_HERO_SANTADOG) {
    if(strSpineFileName.length() > 0){
        spine = GM->getHeroSpine(unit);
    }else{
        this->Sprite::initWithSpriteFrameName(sptName);
//        log("init sprite: %s", sptName);
    }
    
    if (spine) {
//        if(WORLD){
//            WORLD->addChild(spine, 1);
//        }else{
//        WORLD->addChild(spine, WORLD->batch->getLocalZOrder()+1);
//        }
        
        spine->setAnimation(0, "idle", true);
        spine->setScale(0.5f);
        
        this->Sprite::initWithSpriteFrameName("shadow.png");
        if(WORLD){
            WORLD->batch->addChild(spine, 1);
        }else{
            spine->removeFromParent();
        }
    }
    if(WORLD){
        setAnchorPoint(Vec2(0.5, (14/getBoundingBox().size.height)*WORLD->imageScale));
    }else{
        setAnchorPoint(Vec2(0.5, (14/getBoundingBox().size.height)*0.5f));
    }
    
//    this->getTexture()->setAliasTexParameters();
    
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
    imgTalkBalloon->setAnchorPoint(Vec2(0.5, 0));
    imgTalkBalloon->setScale9Enabled(true);
    imgTalkBalloon->setCapInsets(cocos2d::Rect(12, 4, 1, 4));
    imgTalkBalloon->setOpacity(0);
    imgTalkBalloon->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0), NULL));
    
    lblTalk = Label::createWithTTF(text, GameManager::getInstance()->getFont(FONT_VISITOR), 36);
    imgTalkBalloon->addChild(lblTalk);
    lblTalk->setColor(Color3B(104, 85, 84));
    lblTalk->setScale(0.3);
    lblTalk->setOpacity(0);
    lblTalk->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0), NULL));
    float gap = 3;
    lblTalk->setPosition(Vec2(lblTalk->getBoundingBox().size.width/2 + gap, imgTalkBalloon->getBoundingBox().size.height/2 + 1.5));
    float width = lblTalk->getBoundingBox().size.width + gap*2;
    if (width < imgTalkBalloon->getContentSize().width) {
        width = imgTalkBalloon->getContentSize().width;
    }
    imgTalkBalloon->setContentSize(cocos2d::Size(width, imgTalkBalloon->getContentSize().height));
    
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
    imgTalkBalloon->setPosition(this->getPosition() + Vec2(0, talkBalloonY));
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
        spine->setScaleX(flip?-0.5:0.5);
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
    
    Vec2 vlct;
    
    if (this->freezed) {
        vlct = Vec2(inWater?vlct.x*0.9:vlct.x * (freezed?0.95:0.30), vlct.y);
    }else{
//        if (wantsToJump){
//            if(inWater || (!jumped && (onGround || jumpAllowed))) {
//                Vec2 jumpForce = Vec2(0.0, inWater?(onGround?waterAccelYP+10:waterAccelYP):1850.0);
//                vlct = vlct + jumpForce;
//                jumped = true;
//                onGround = false;
//                wantsToJump = false;
//                onJumped();
//
//            }else if (doubleJump && !tumbled && (jumped || !onGround) && !iWasInWater) {
//                Vec2 jumpForce = Vec2(0.0, inWater?(onGround?waterAccelYP+60:waterAccelYP):1850.0);
//                vlct = Vec2(vlct.x, 230);
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
        
        vlct = vlct + Vec2(horizontalMove*dt, verticalMove*dt);
    }
    
//    Vec2 minMovement = Vec2(inWater?-waterMaxX:-50.0-extraSpeed, inWater?-waterMinY:-280.0 - extraJump);
//    Vec2 maxMovement = Vec2(inWater?waterMaxX:50.0+extraSpeed, 280.0 + extraJump);
    Vec2 minMovement = Vec2(inWater?-waterMaxX:-280.0, inWater?-waterMinY:-280.0 - extraJump);
    Vec2 maxMovement = Vec2(inWater?waterMaxX:280.0, 280.0 + extraJump);
    
    // Clamp allows you limit a hero's speed while Moving
    vlct = vlct.getClampPoint(minMovement, maxMovement);
    Vec2 stepVelocity = vlct * dt;
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
        movingAngle = -(-GameManager::getInstance()->getAngle(this->getPosition(), this->target->getPosition() + Vec2(0, TILE_SIZE/2)) + 180);
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
    Vec2 vlct;
    float baseSpeed = 50;
    
    vlct = Vec2((baseSpeed+extraSpeed)*cos(movingAngle*3.14/180),
                 (baseSpeed+extraSpeed)*sin(movingAngle*3.14/180));
    
    Vec2 minMovement = Vec2(-baseSpeed-extraSpeed, -280.0);
    Vec2 maxMovement = Vec2(baseSpeed+extraSpeed, 280.0);
    
    // Clamp allows you limit a hero's speed while Moving
    vlct = vlct.getClampPoint(minMovement, maxMovement);
    Vec2 stepVelocity = vlct * dt;

    //this->setPosition(ccpAdd(getPosition(), stepVelocity));
    if (freezed) {
        stepVelocity = Vec2::ZERO;
    }
    desiredPosition = getPosition() + stepVelocity;
    this->velocity = vlct;
}

void Movable::updatePositionToFlyToFollowTarget(float dt)
{
//    baseSpeed+extraSpeed
    Vec2 tPos = target->getPosition();
    Vec2 pos = getPosition();
    float diff = sqrtf((tPos.x - pos.x)*(tPos.x-pos.x)+(tPos.y - pos.y)*(tPos.y - pos.y));
    float toMove = dt*50 + dt*extraSpeed;
    
    float toX = (tPos.x - pos.x)*(toMove/diff);
    float toY = (tPos.y - pos.y)*(toMove/diff);
    desiredPosition = Vec2(pos.x + toX, pos.y + toY);
    velocity = Vec2(0, this->getPositionY() >= desiredPosition.y?-1:1);
}

void Movable::updatePositionForBouncing(float dt)
{
    Vec2 vlct;
    float baseSpeed = 50;
    
    Vec2 gravity = Vec2(0.0, -850.0);
    if (onGround) {
        gravity = Vec2(0.0, 0);
//        if (state == STATE_JUMPING) {
//            state = STATE_STANDING;
//        }
    }
    Vec2 gravityStep = gravity * dt;
    
    vlct = this->velocity + gravityStep;
    
    Vec2 minMovement = Vec2(-baseSpeed-extraSpeed, -280.0);
    Vec2 maxMovement = Vec2(baseSpeed+extraSpeed, 280.0);
    
    vlct = vlct.getClampPoint(minMovement, maxMovement);
    Vec2 stepVelocity = vlct * dt;
    
    desiredPosition = getPosition() + stepVelocity;
    this->velocity = vlct;
}

cocos2d::Rect Movable::collectBoundingBox()
{
    return cocos2d::Rect(desiredPosition.x - TILE_SIZE/2, desiredPosition.y - getContentSize().height/2, TILE_SIZE, TILE_SIZE*2);
}
cocos2d::Rect Movable::collisionBoundingBox()
{
    float width = getContentSize().width/2;
    float height = getContentSize().height*getScaleY();
    if (width <= TILE_SIZE/2) {
        width = TILE_SIZE/2;
    }
    if (height <= 0) {
        height = TILE_SIZE/2;
    }
//    return cocos2d::Rect(desiredPosition.x - TILE_SIZE/2, desiredPosition.y - getContentSize().height/2, width, height);//TILE_SIZE, TILE_SIZE);
    return cocos2d::Rect(desiredPosition.x - width/2, desiredPosition.y - height/2, width, TILE_SIZE/2);
    cocos2d::Rect bounding = boundingBox();
   
    if (bounding.size.width < TILE_SIZE*2 && bounding.size.height < TILE_SIZE*2) {
        cocos2d::Rect collisionBox = RectInset(bounding, 1, 0);
        Vec2 diff = desiredPosition - getPosition();
        cocos2d::Rect returnBoundingBox = RectOffset(collisionBox, diff.x, diff.y);
        return returnBoundingBox;
    }else{
        Vec2 pos = getPosition();
        cocos2d::Rect rect = cocos2d::Rect(pos.x - TILE_SIZE/2, pos.y - getContentSize().height/2, TILE_SIZE, getContentSize().height);
        cocos2d::Rect collisionBox = RectInset(rect, 0, 0);
        Vec2 diff = desiredPosition - getPosition();
        cocos2d::Rect returnBoundingBox = RectOffset(collisionBox, diff.x, diff.y);
        return returnBoundingBox;
    }
}
cocos2d::Rect Movable::damageBoundingBox()
{
    cocos2d::Rect bounding = getBoundingBox();
    cocos2d::Rect collisionBox = RectInset(bounding, 3, 3);
//    Vec2 diff = 0;//desiredPosition - getPosition();
//    cocos2d::Rect returnBoundingBox = cocos2d::RectOffset(collisionBox, diff.x, diff.y);
//    return returnBoundingBox;
    return collisionBox;
}

Vec2 Movable::getGravityPosition()
{
    return Vec2(getPosition().x, getPosition().y-getContentSize().height/2 + TILE_SIZE/2);
}

/*cocos2d::Rect Movable::currentBoundingBox()
{
    cocos2d::Rect bounding = boundingBox();
    
    if (vehicleType == VEHICLE_GOLIATH) {
        
        Vec2 pos = getPosition();
        cocos2d::Rect collisionBox = cocos2d::Rect(pos.x - TILE_SIZE, pos.y - 28, TILE_SIZE*2, TILE_SIZE*2);
        return collisionBox;
    }
    
    if (bounding.size.width < TILE_SIZE*2 && bounding.size.height < TILE_SIZE*2) {
        cocos2d::Rect collisionBox = cocos2d::RectInset(bounding, 0, 0);
//        Vec2 diff = ccpSub(desiredPosition, getPosition());
//        cocos2d::Rect returnBoundingBox = cocos2d::RectOffset(collisionBox, diff.x, diff.y);
        return collisionBox;
    }else{
        Vec2 pos = getPosition();
        cocos2d::Rect rect = cocos2d::Rect(pos.x - TILE_SIZE/2, pos.y - TILE_SIZE + 1, TILE_SIZE, TILE_SIZE*2 - 2);
        cocos2d::Rect collisionBox = cocos2d::RectInset(rect, 0, 0);
//        Vec2 diff = ccpSub(desiredPosition, getPosition());
//        cocos2d::Rect returnBoundingBox = cocos2d::RectOffset(collisionBox, diff.x, diff.y);
        return collisionBox;
    }
}*/

cocos2d::Rect Movable::RectOffset(cocos2d::Rect rect, float x, float y)
{
    return cocos2d::Rect(rect.origin.x + x, rect.origin.y + y, rect.size.width, rect.size.height);
}

cocos2d::Rect Movable::RectInset(cocos2d::Rect rect, float x, float y)
{
    return cocos2d::Rect(rect.origin.x + x, rect.origin.y + y, rect.size.width - x*2, rect.size.height - y*2);
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
    Vec2 pos = this->getPosition();
    Vec2 tPos = tar->getPosition();
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
//    isTemporaryFlying = false;
//    this->forceAttackTarget = false;
//    resetRoute();
//    targetCoordinate = Vec2::ZERO;
//    isArrived = true;
//    target = nullptr;
//    canFindTarget = attackType != ATTACK_TYPE_NONE;
//    if(isGoingToBuild){
//        isGoingToBuild = false;
//        Movable* unit = WORLD->buildTheBuilding(builderBuildingIndex, builderCoordinate.x, builderCoordinate.y, builderSize.width, builderSize.height, builderSpriteName, alliSide);
//        if(unit == nullptr){ // condition not met
//            return;
//        }
//        // progress
//        Sprite* spt = Sprite::create("uiBox.png");
//        spt->setPosition(unit->getPosition() + Vec2(50, unit->getContentSize().height/2 + 10));
//        WORLD->addChild(spt, 1000);
//        auto timer = ProgressTimer::create(WORLD->getSpriteForUnit(unit->unitType));
//        processTimer = timer;
//        timer->setType(ProgressTimer::Type::RADIAL);
//        spt->addChild(timer);
//        timer->setName("timer");
//        spt->setScale(0.3f);
//        timer->setPosition(spt->getContentSize()/2);
//        timer->setPercentage(0);
//        timer->runAction(Sequence::create(ProgressTo::create(WORLD->getUnitCreateTime(unit->unitType), 100), CallFunc::create(CC_CALLBACK_0(Movable::resetProcessTimer, this)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), nullptr));
//        // progress end
//
//        if(WORLD->selectedArray.find((EnemyBase*)this) != WORLD->selectedArray.end()){
//            WORLD->deselect(this);
//        }
//        builderBuilding = unit;
//        unit->setRotation(0);
//        unit->buildingCompleteTime = WORLD->getUnitCreateTime(builderBuildingIndex);
//        unit->setSpriteFrame("underConstrunction0.png");
//        GM->runAnimation(unit, "underConstrunction", true);
//        this->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
//                                         DelayTime::create(0.2f),
//                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
//                                         DelayTime::create(0.2f),
//                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
//                                         DelayTime::create(0.2f),
//                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
//                                         DelayTime::create(0.2f),
//                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
//                                         DelayTime::create(0.2f),
//                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
//                                         DelayTime::create(0.2f),
//                                         CallFunc::create(CC_CALLBACK_0(Movable::playTreeSound, this)),
//                                         DelayTime::create(0.2f),
//                                         NULL));
//        this->runAction(Sequence::create(DelayTime::create(unit->buildingCompleteTime), CallFunc::create(CC_CALLBACK_0(Movable::onBuildComplete, this)), NULL));
//        this->setVisible(false); // test
//        this->attackType = ATTACK_TYPE_NONE;
//        this->untouchable = true;
//    }
//    if (attackFlagPos != Vec2::ZERO && target == nullptr && search) {
//        if(attackFlagPos == getPosition()){
//            attackFlagPos = Vec2::ZERO;
//        }else{
//            moveToTarget(attackFlagPos);
//            canFindTarget = true;
//            return;
//        }
//    }
//
//    if (isEnemy && wantToEli && search) {
//        WORLD->attackNearHero((EnemyBase*)this);
//    }else if (!isGatheringGold && !WORLD->isThisSpotAvailable(this) && canMove && !isFlying) {
//        isTemporaryFlying = true;
//        Vec2 coordinate = WORLD->getCoordinateFromPosition(getPosition());
//        coordinate = Vec2(coordinate.x + rand()%3 - 1, coordinate.y + rand()%3 - 1);
//        if(coordinate.x + 1 >= WORLD->mapSize.width){
//            coordinate.x = WORLD->mapSize.width - 1;
//        }else if(coordinate.x < 0){
//            coordinate.x = 0;
//        }
//        if(coordinate.y + 1 >= WORLD->mapSize.height){
//            coordinate.y = WORLD->mapSize.height - 1;
//        }else if(coordinate.y < 0){
//            coordinate.y = 0;
//        }
//        Vec2 newPos = WORLD->getPositionFromTileCoordinate(coordinate.x, coordinate.y);
////        Vec2 nowPos = getPosition();
//        WORLD->moveTo((EnemyBase*)this, newPos);
////        runMoveAnimation(DIRECTION_E);
//    }
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
void Movable::removeBuildProgressIcon(){
    if (processTimer != nullptr) {
        processTimer->stopAllActions();
        processTimer = nullptr;
    }
    if (buildProgressBtn != nullptr) {
        buildProgressBtn->removeFromParent();
        buildProgressBtn = nullptr;
    }
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
        spt->setScale(1/WORLD->imageScale);
        WORLD->addGold(800);
        WORLD->addLumber(400);
    }else if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 7) {
        HUD->tutorialIndex++;
        HUD->talkIndex = 0;
        HUD->talkText = LM->getText("tutorial 8");
        Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
        spt->stopAllActions();
        spt->setSpriteFrame("swordmanStand0.png");
        spt->setScale(1/WORLD->imageScale);
        WORLD->addGold(300);
    }
    WORLD->setAfterBuildingProcess(builderBuilding);
    removeBuildProgressIcon();
    
    this->untouchable = false;
    this->setVisible(true); // test
    if(!this->isCarryingGold && !this->isCarryingTree){
        this->attackType = ATTACK_TYPE_NEAR;
    }
    isBuildingABuilding = false;
    stopNew();

    // If this builder is a ship unit and its current tile is now inside the
    // completed water building, move it to the nearest free water tile.
    bool isShipBuilder = (unitType == UNIT_HUMAN_SHUTTLE ||
                          unitType == UNIT_ORC_SHUTTLE ||
                          unitType == UNIT_HUMAN_SHIP ||
                          unitType == UNIT_ORC_SHIP ||
                          unitType == UNIT_HUMAN_BATTLE_SHIP ||
                          unitType == UNIT_HUMAN_OIL_SHIP ||
                          unitType == UNIT_ORC_OIL_SHIP);
    if (isShipBuilder && builderBuilding != nullptr) {
        EnemyBase *bldg = (EnemyBase *)builderBuilding;
        Vec2 tile = WORLD->getCoordinateFromPosition(getPosition());
        Vec2 st = bldg->buildingStartCoordinate;
        Size sz = bldg->buildingOccupySize;
        if ((int)tile.x >= (int)st.x && (int)tile.x < (int)(st.x + sz.width) &&
            (int)tile.y >= (int)st.y && (int)tile.y < (int)(st.y + sz.height)) {
            Vec2 freePos = WORLD->findEmptyWaterSpawnTile(
                (int)st.x, (int)st.y, (int)sz.width, (int)sz.height);
            if (freePos.x >= 0) {
                moveFlagPos = freePos;
                moveToPos   = freePos;
                unitAct     = UNIT_ACT_MOVE;
                unitActDetail = UNIT_ACT_DETAIL_IDLE;
            }
        }
    }
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
            if(isSkillOn){
                spine->setAnimation(0, "skill", false);
            }else{
                spine->setAnimation(0, "attack", false);
            }
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
    if(!target || target->energy <= 0 || target->untouchable || (target->alliSide == WHICH_SIDE_MUTUAL && unitType != UNIT_WIZARD)){
        target = nullptr;
//        stopNew();
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
    
    if (rand()%100 <= skillRate) { // test
        isSkillOn = true;
        if (unitType == UNIT_HERO_REAPER) {
            attackCoolTime += 23.0f/30;
        }else if(unitType == UNIT_HERO_CATINBOOTS){
            attackCoolTime = 65.0f/30;
        }
    }
//    isSkillOn = true;  // test 
    
    runAnimation(ANIMATION_TYPE_ATTACK);
    
    if (isSkillOn){
        if (GM->getElement(unitType) == ELEMENT_LIGHTNING){
            startSkillAuraAndSkillEffect(AURA_LIGHTNING); // test
        }else if (GM->getElement(unitType) == ELEMENT_GROUND){
            startSkillAuraAndSkillEffect(AURA_GREEN);
        }else if (GM->getElement(unitType) == ELEMENT_WATER){
            startSkillAuraAndSkillEffect(AURA_ICE);
        }else if (GM->getElement(unitType) == ELEMENT_FIRE){
            startSkillAuraAndSkillEffect(AURA_FIRE);
        }else if (GM->getElement(unitType) == ELEMENT_LIGHT){
            startSkillAuraAndSkillEffect(AURA_LIGHT);
        }
    }
    if(canMove)
        this->setFlippedX(getPositionX() < target->getPositionX());
    isInAttackMotion = true;
    float happenTime = attackHappenTime;
    if (isSkillOn) {
        if (unitType == UNIT_HERO_ORC) {
            happenTime = 39.0f/30;
        }else if (unitType == UNIT_HERO_GOBLIN) {
            happenTime = 26.0f/30;
        }else if (unitType == UNIT_HERO_LIZARDMAN) {
            happenTime = 40.0f/30;
        }else if (unitType == UNIT_HERO_WEREWOLF) {
            happenTime = 33.0f/30;
        }else if (unitType == UNIT_HERO_SPEARMAN) {
            happenTime = 21.0f/30;
//            for (int i = 0; i < 3; i++) {
                Sequence* seq = Sequence::create(DelayTime::create(25.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)),
                                                 DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)),
                                                 DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)), nullptr);
                seq->setTag(attackTag);
                this->runAction(seq);
//            }
        }else if (unitType == UNIT_HERO_ARCHER){
            happenTime = 48.0f/30;
        }else if (unitType == UNIT_HERO_MONK){
            happenTime = 27.0f/30;
        }else if (unitType == UNIT_HERO_FIGHTER){
            happenTime = 36.0f/30;
//            for (int i = 0; i < 3; i++) {
                Sequence* seq = Sequence::create(DelayTime::create(40.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)),
                                                 DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)),
                                                 DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)), nullptr);
                seq->setTag(attackTag);
                this->runAction(seq);
//            }
        }else if (unitType == UNIT_HERO_BEAR) {
            happenTime = 32.0f/30;
        }else if (unitType == UNIT_HERO_KNIGHT) {
            happenTime = 21.0f/30;
            Sequence* seq = Sequence::create(DelayTime::create(25.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)),
                                             DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)),
                                             DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)), nullptr);
            seq->setTag(attackTag);
            this->runAction(seq);
        }else if (unitType == UNIT_HERO_ELF_SWORDMAN) {
            happenTime = 45.0f/30;
        }else if (unitType == UNIT_HERO_ASSASSIN){
            happenTime = 42.0f/30;
        }else if (unitType == UNIT_HERO_LION) {
            happenTime = 19.0f/30;
            Sequence* seq = Sequence::create(DelayTime::create(30.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)),
                                             DelayTime::create(11.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)),
                                             DelayTime::create(0), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)), nullptr);
            seq->setTag(attackTag);
            this->runAction(seq);
        }else if (unitType == UNIT_HERO_WIZARD){
            happenTime = 26.0f/30;
        }else if (unitType == UNIT_HERO_TANKER){
            happenTime = 25.0f/30;
        }else if (unitType == UNIT_HERO_SKELETON){
            happenTime = 42.0f/30;
            Sequence* seq = Sequence::create(DelayTime::create(46.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)),
                                             DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)),
                                             DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)), nullptr);
            seq->setTag(attackTag);
            this->runAction(seq);
        }else if (unitType == UNIT_HERO_REAPER){
            happenTime = 25.0f/30;
            Sequence* seq = Sequence::create(DelayTime::create(30.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)),
                                             DelayTime::create(5.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)),
                                             DelayTime::create(5.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)), nullptr);
            seq->setTag(attackTag);
            this->runAction(seq);
        }else if (unitType == UNIT_HERO_ENT){
            happenTime = 31.0f/30;
        }else if (unitType == UNIT_HERO_SANTA){
            happenTime = 28.0f/30;
        }else if (unitType == UNIT_HERO_RUDOLPH){
            happenTime = 30.0f/30;
        }else if (unitType == UNIT_HERO_SANTADOG){
            happenTime = 28.0f/30;
        }else if (unitType == UNIT_HERO_SALAMANDER){
            happenTime = 34.0f/30;
        }else if (unitType == UNIT_HERO_UNDINE){
            happenTime = 24.0f/30;
        }else if (unitType == UNIT_HERO_PENGUIN){
            happenTime = 24.0f/30;
        }else if (unitType == UNIT_HERO_LADY_WEREWOLF){
            happenTime = 25.0f/30;
        }else if (unitType == UNIT_HERO_CATINBOOTS){
            happenTime = 44.0f/30;
        }else if (unitType == UNIT_HERO_LADY_BEAR){
            happenTime = 23.0f/30;
        }else if (unitType == UNIT_HERO_MOLE){
            happenTime = 29.0f/30;
        }else if (unitType == UNIT_HERO_LADY_LION){
            happenTime = 20.0f/30;
        }else if (unitType == UNIT_HERO_TOYMOUSE){
            happenTime = 57.0f/30;
        }else if (unitType == UNIT_HERO_BATMONSTER){
            happenTime = 45.0f/30;
        }else if (unitType == UNIT_HERO_SAVAGEARCHER){
            happenTime = 30.0f/30;
            Sequence* seq = Sequence::create(DelayTime::create(34.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)),
                                             DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)),
                                             DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)), nullptr);
            seq->setTag(attackTag);
            this->runAction(seq);
        }else if (unitType == UNIT_HERO_MEMEAT){
            happenTime = 40.0f/30;
        }else if (unitType == UNIT_HERO_PARASITE){
            happenTime = 34.0f/30;
        }else if (unitType == UNIT_HERO_WATERMELON){
            happenTime = 32.0f/30;
        }else if (unitType == UNIT_HERO_BABYMINO){
            happenTime = 23.0f/30;
        }else if (unitType == UNIT_HERO_MINO){
            happenTime = 38.0f/30;
        }else if (unitType == UNIT_HERO_KERBEROS){
            happenTime = 24.0f/30;
        }else if (unitType == UNIT_HERO_LAMIA){
            happenTime = 41.0f/30;
        }else if (unitType == UNIT_HERO_CHUNJA){
            happenTime = 30.0f/30;
        }else if (unitType == UNIT_HERO_GOLEM){
            happenTime = 46.0f/30;
        }
    }
    Sequence* seq = Sequence::create(DelayTime::create(happenTime), CallFunc::create(CC_CALLBACK_0(Movable::attack, this)), nullptr);
    seq->setTag(attackTag);
    this->runAction(seq);
}
void Movable::healNearFriend(){
    WORLD->healHeroNearPoint(this, getPosition(), 100);
}
void Movable::startSkillAuraAndSkillEffect(int aura){
    selectedSkillAura = aura;
    skillTarget = target;
    updateSkillAura(0);
    float interval = 0.2f;
    if(aura == AURA_LIGHTNING){
        interval = 0.3f;
    }else if(aura == AURA_FIRE){
        interval = 0.2f;
    }else if(aura == AURA_ICE){
        interval = 0.2f;
    }
    this->schedule(schedule_selector(Movable::updateSkillAura), interval);
    
    if (unitType == UNIT_HERO_ORC) {
        this->runAction(Sequence::create(DelayTime::create(39.0f/30 - 0.06f*6), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_LIGHTNING;
    }else if(unitType == UNIT_HERO_GOBLIN){
        this->runAction(Sequence::create(DelayTime::create(25.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_FIREBOMB;
    }else if (unitType == UNIT_HERO_LIZARDMAN) {
        this->runAction(Sequence::create(DelayTime::create(40.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_ICE_RAISE;
    }else if (unitType == UNIT_HERO_WEREWOLF) {
        this->runAction(Sequence::create(DelayTime::create(33.0f/30 - 0.1f), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_BLUE_TEETH;
    }else if (unitType == UNIT_HERO_SPEARMAN) {
        this->runAction(Sequence::create(DelayTime::create(20.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),
                                         DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),
                                         DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),
                                         DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_BLUE_SLASH;
    }else if (unitType == UNIT_HERO_ARCHER) {
        this->runAction(Sequence::create(DelayTime::create(48.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),
                                         DelayTime::create(2.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),
                                         DelayTime::create(2.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),
                                         DelayTime::create(2.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),nullptr));
        skillEffectIndex = EFFECT_BLUE_SLASH;
        if(skillTarget){
            for (int i = 0; i < 4; i++) {
                float x = rand()%100-50;
                WORLD->createMissile("heroArrow.png", "arrowBuried.png", skillTarget->getPosition() + Vec2(x, 200), skillTarget->getPosition() + Vec2(x, 0), 0.1f, ap, isEnemy, -90, this, 2.8 + i*0.06f);
            }
        }
    }else if (unitType == UNIT_HERO_MONK) {
        this->runAction(Sequence::create(DelayTime::create(27.0f/30 - 0.1f), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_HIT_WITH_CIRCLE;
    }else if (unitType == UNIT_HERO_FIGHTER ||
              unitType == UNIT_HERO_MOLE) {
        this->runAction(Sequence::create(DelayTime::create(29.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_HIT_WITH_CIRCLE;
    }else if (unitType == UNIT_HERO_BEAR) {
        this->runAction(Sequence::create(DelayTime::create(32.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_RED_TEETH;
    }else if (unitType == UNIT_HERO_HEALER) {
        this->runAction(Sequence::create(DelayTime::create(35.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_HEAL;
    }else if (unitType == UNIT_HERO_KNIGHT) {
        this->runAction(Sequence::create(DelayTime::create(19.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),
                                         DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),
                                         DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),
                                         DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),nullptr));
        skillEffectIndex = EFFECT_BLUE_SLASH;
    }else if (unitType == UNIT_HERO_ELF_SWORDMAN) {
        this->runAction(Sequence::create(DelayTime::create(45.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_GREEN_HIT;
    }else if (unitType == UNIT_HERO_ASSASSIN) {
        this->runAction(Sequence::create(DelayTime::create(45.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_BLUE_SLASH;
    }else if (unitType == UNIT_HERO_CATINBOOTS) {
        this->runAction(Sequence::create(DelayTime::create(43.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_BLUE_SLASH;
    }else if (unitType == UNIT_HERO_LION) {
        this->runAction(Sequence::create(DelayTime::create(19.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),
                                         DelayTime::create(11.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),
                                         DelayTime::create(11.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),nullptr));
        skillEffectIndex = EFFECT_HIT_WITH_CIRCLE;
    }else if (unitType == UNIT_HERO_TANKER ||
              unitType == UNIT_HERO_SANTADOG ||
              unitType == UNIT_HERO_PENGUIN) {
        this->runAction(Sequence::create(DelayTime::create(25.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_HIT_WITH_CIRCLE_ON_GROUND;
    }else if (unitType == UNIT_HERO_REAPER) {
        this->runAction(Sequence::create(DelayTime::create(25.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), DelayTime::create(5.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), DelayTime::create(5.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), DelayTime::create(5.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_PURPLE_SLASH;
    }else if (unitType == UNIT_HERO_LADY_WEREWOLF) {
        skillEffectIndex = EFFECT_HIT_WITH_CIRCLE_ON_GROUND;
    }else if (unitType == UNIT_HERO_UNDINE) {
        skillEffectIndex = EFFECT_WATER_SPLASH;
    }else if (unitType == UNIT_HERO_ENT) {
        this->runAction(Sequence::create(DelayTime::create(31.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_HIT_WITH_CIRCLE_ON_GROUND;
    }else if (unitType == UNIT_HERO_RUDOLPH) {
        this->runAction(Sequence::create(DelayTime::create(27.0f/30 - 0.1f), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_HIT_WITH_CIRCLE;
    }else if (unitType == UNIT_HERO_SALAMANDER) {
        this->runAction(Sequence::create(DelayTime::create(34.0f/30 - 0.1f), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_FIREBOMB;
    }else if (unitType == UNIT_HERO_LADY_LION) {
        this->runAction(Sequence::create(DelayTime::create(20.0f/30 - 0.1f), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_FIREBALL_EXPLOSION;
    }else if (unitType == UNIT_HERO_TOYMOUSE) {
        this->runAction(Sequence::create(DelayTime::create(57.0f/30 - 0.1f), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_FIREBALL_EXPLOSION;
    }else if (unitType == UNIT_HERO_BATMONSTER) {
        this->runAction(Sequence::create(DelayTime::create(45.0f/30 - 0.1f), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_FIREBALL_EXPLOSION;
    }else if (unitType == UNIT_HERO_SAVAGEARCHER) {
        this->runAction(Sequence::create(DelayTime::create(30.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),
                                         DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),
                                         DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)),
                                         DelayTime::create(4.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_BLUE_SLASH;
    }else if (unitType == UNIT_HERO_MEMEAT) {
        this->runAction(Sequence::create(DelayTime::create(40.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_HIT_WITH_CIRCLE_ON_GROUND;
    }else if (unitType == UNIT_HERO_PARASITE) {
        this->runAction(Sequence::create(DelayTime::create(32.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_BLUE_SLASH;
    }else if (unitType == UNIT_HERO_WATERMELON) {
        this->runAction(Sequence::create(DelayTime::create(32.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_FIREBALL_EXPLOSION;
    }else if (unitType == UNIT_HERO_BABYMINO) {
        this->runAction(Sequence::create(DelayTime::create(22.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_BLUE_SLASH;
    }else if (unitType == UNIT_HERO_MINO) {
        this->runAction(Sequence::create(DelayTime::create(39.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_FIREBALL_EXPLOSION;
    }else if (unitType == UNIT_HERO_KERBEROS) {
        this->runAction(Sequence::create(DelayTime::create(24.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_FIREBOMB;
    }else if (unitType == UNIT_HERO_LAMIA) {
        this->runAction(Sequence::create(DelayTime::create(20.0f/30), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_LASER;
    }else if (unitType == UNIT_HERO_CHUNJA) {
        this->runAction(Sequence::create(DelayTime::create(30.0f/30 - 0.1f), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_HIT_WITH_CIRCLE;
    }else if (unitType == UNIT_HERO_GOLEM) {
        this->runAction(Sequence::create(DelayTime::create(46.0f/30 - 0.1f), CallFunc::create(CC_CALLBACK_0(Movable::showSkillEffect, this)), nullptr));
        skillEffectIndex = EFFECT_FIREBALL_EXPLOSION;
    }
}
void Movable::updateSkillAura(float dt){
    if(!WORLD){
        log("world is not valied");
        return;
    }
    int diff = 50*WORLD->imageScale;
    if(selectedSkillAura == AURA_LIGHTNING){
        Sprite* spt = Sprite::createWithSpriteFrameName("lightningAura0.png");
        GM->runAnimation(spt, "auraLightning", false, true);
        WORLD->batch->addChild(spt, 2);
        spt->setRotation(rand()%360);
        if (spine) {
            spt->setPosition(spine->getPosition() + Vec2(diff - rand()%(diff*2), spine->getBoundingBox().size.height/2 + diff - rand()%(diff*2)));
        }else{
            spt->setPosition(this->getPosition() +Vec2(getContentSize().width/2 - diff + rand()%(diff*2), getContentSize().height/2 - diff + rand()%(diff*2)));
        }
        spt->setScale(1/getScaleX());
    }else if(selectedSkillAura == AURA_FIRE){
        for(int i = 0; i < 2; i++){
            Sprite* spt = Sprite::createWithSpriteFrameName("whiteCircle.png");
            spt->runAction(Sequence::create(TintTo::create(0.02, Color3B::BLACK), TintTo::create(0.02, Color3B::WHITE), TintTo::create(0.02, Color3B(255, 233, 0)), TintTo::create(0.08, Color3B::RED), TintTo::create(0.08, Color3B::GRAY), TintTo::create(0.08, Color3B::BLACK), NULL));
            spt->runAction(Sequence::create(DelayTime::create(0.18f), FadeOut::create(0.14f), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
            spt->runAction(MoveBy::create(0.3, Vec2(0, 60*WORLD->imageScale)));
            spt->setScale(0.2f + (rand()%10)*0.01f);
            spt->runAction(Sequence::create(DelayTime::create(0.2f), ScaleTo::create(0.1f, spt->getScale()*1.4f), NULL));
            WORLD->batch->addChild(spt, 2);
            if (spine) {
                spt->setPosition(spine->getPosition() + Vec2(diff - rand()%(diff*2), rand()%30));
            }else{
                spt->setPosition(this->getPosition() +Vec2(getContentSize().width/2 - diff + rand()%(diff*2), rand()%30));
            }
        }
    }else if(selectedSkillAura == AURA_ICE){
        for(int i = 0; i < 2; i++){
            Sprite* spt = Sprite::createWithSpriteFrameName("whiteBigCircle.png");
            spt->runAction(Sequence::create(TintTo::create(3, Color3B(12, 175, 239)), NULL));
            spt->runAction(Sequence::create(DelayTime::create(2), FadeOut::create(1), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
            float x = rand()%100 - 50;
            spt->setOpacity(150);
            spt->setBlendFunc(BlendFunc::ADDITIVE);
            spt->runAction(MoveBy::create(3, Vec2(-x/2, 0)));
            spt->runAction(JumpBy::create(2, Vec2::ZERO, 20, 1));
            spt->setScale(0.2f + (rand()%10)*0.01f);
            spt->runAction(Sequence::create(DelayTime::create(2), ScaleTo::create(1, spt->getScale()*1.4f), NULL));
            WORLD->batch->addChild(spt, 2);
            if (spine) {
                spt->setPosition(spine->getPosition() + Vec2(diff - rand()%(diff*2), rand()%10));
            }else{
                spt->setPosition(this->getPosition() +Vec2(getContentSize().width/2 - diff + rand()%(diff*2), rand()%30));
            }
            
            spt = Sprite::createWithSpriteFrameName(strmake("iceCrystalPart%d.png", rand()%2));
            WORLD->batch->addChild(spt);
            spt->setPosition(this->getPosition() +getPosition() + Vec2(diff - rand()%(diff*2), rand()%10));
            spt->runAction(Sequence::create(EaseOut::create(MoveBy::create(0.3f, Vec2(rand()%60-30, rand()%60-30)), 2), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
            spt->runAction(Sequence::create(DelayTime::create(0.2f), FadeOut::create(0.1f), NULL));
            spt->setOpacity(150);
            spt->runAction(RotateBy::create(0.3f, rand()%200-100));
        }
    }else if(selectedSkillAura == AURA_GREEN){
        for(int i = 0; i < 3; i++){
            Sprite* spt = Sprite::createWithSpriteFrameName("whiteBigCircle.png");
            spt->setColor(Color3B(6, 129, 48));
            spt->setBlendFunc(BlendFunc::ADDITIVE);
            spt->runAction(Sequence::create(TintTo::create(0.3, Color3B(121, 255, 137)), NULL));
            spt->runAction(Sequence::create(DelayTime::create(0.2f), FadeOut::create(0.1f), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
            float x = diff - rand()%(diff*2);
            spt->setOpacity(200);
            spt->setPosition(spine->getPosition() + Vec2(x, rand()%30));
            spt->runAction(MoveBy::create(0.3, Vec2(rand()%2==0?-50:50, 60)));
            spt->setScale(0.2f + (rand()%10)*0.01f);
            WORLD->batch->addChild(spt, 2);
            
        }
    }else if(selectedSkillAura == AURA_LIGHT){
        Sprite* spt = Sprite::createWithSpriteFrameName("yellowBall.png");
        WORLD->batch->addChild(spt);
        spt->runAction(Sequence::create(FadeIn::create(0.18f), FadeOut::create(0.14f), SPT_REMOVE_FUNC, NULL));
        float widthScale = getContentSize().width*1.3f/spt->getContentSize().width;
        spt->setScale(widthScale, widthScale*0.3f);
        spt->setPosition(spine->getPosition());
        for(int i = 0; i < 3; i++){
            spt = Sprite::createWithSpriteFrameName("yellowBall.png");
            spt->runAction(Sequence::create(DelayTime::create(0.18f), FadeOut::create(0.14f), SPT_REMOVE_FUNC, NULL));
            spt->runAction(MoveBy::create(0.3, Vec2(0, 60)));
            spt->setScale(0.8f + (rand()%6)*0.1f);
            spt->runAction(Sequence::create(DelayTime::create(0.2f), ScaleTo::create(0.1f, spt->getScale()*1.4f), NULL));
            WORLD->batch->addChild(spt, 2);
            spt->setPosition(spine->getPosition() + Vec2(diff - rand()%(diff*2), rand()%30));
        }
    }
}
void Movable::stopSkillAura(){
    this->unschedule(schedule_selector(Movable::updateSkillAura));
}
void Movable::showSkillEffect(){
    if(!skillTarget) return;
    if (skillEffectIndex == EFFECT_LIGHTNING ){
        WORLD->runEffect(skillEffectIndex, skillTarget->getPosition());
    }else if (skillEffectIndex == EFFECT_FIREBOMB ){
        WORLD->runEffect(skillEffectIndex, this->getPosition() + Vec2(isFlippedX()?100:-100, 100)*WORLD->imageScale, 180);
    }else if (skillEffectIndex == EFFECT_ICE_RAISE ){
        Sprite* spt;
        for (int i = 0; i < 6; i++) {
            Vec2 extraPos = Vec2(isFlippedX()?100:-100, 0);
            if(i == 0){
                extraPos = Vec2::ZERO;
            }else if(i > 0){
                float angle = 360*i/5;
                float theta = angle*3.14f/180;
                extraPos = Vec2(cos(theta)*100*WORLD->imageScale, sin(theta)*50/WORLD->getScaleY());
            }
            for (int j = 0; j < 2; j++) {
                spt = Sprite::createWithSpriteFrameName(strmake("iceCrystalPart%d.png", rand()%2));
                WORLD->batch->addChild(spt);
                spt->setPosition(skillTarget->getPosition() + extraPos);
                spt->runAction(Sequence::create(EaseOut::create(MoveBy::create(0.3f, Vec2(rand()%100-50, rand()%100-50)*WORLD->imageScale), 2), SPT_REMOVE_FUNC, NULL));
                spt->runAction(Sequence::create(DelayTime::create(0.2f), FadeOut::create(0.1f), NULL));
                spt->runAction(RotateBy::create(0.3f, rand()%200-100));
            }
            
            spt = Sprite::createWithSpriteFrameName("iceCrystal.png");
            spt->runAction(Sequence::create(EaseOut::create(MoveBy::create(0.4f + (rand()%10)*0.01f, Vec2(0, 150)), 2), FadeOut::create(0.3f), SPT_REMOVE_FUNC, NULL));
            WORLD->batch->addChild(spt);
            spt->setRotation(-43);
            spt->setPosition(skillTarget->getPosition() + extraPos);
        }
    }else if (skillEffectIndex == EFFECT_BLUE_TEETH && skillTarget){
        WORLD->runEffect(skillEffectIndex, skillTarget->getPosition() + Vec2(0, 50)*WORLD->imageScale, 180);
    }else if (skillEffectIndex == EFFECT_BLUE_SLASH){
        if (unitType == UNIT_HERO_ARCHER) {
            WORLD->runEffect(skillEffectIndex, getPosition() + Vec2(rand()%100-50, 100)*WORLD->imageScale, -90 + rand()%10-5);
        }else{
            WORLD->runEffect(skillEffectIndex, skillTarget->getPosition() + Vec2(0, rand()%100-50)*WORLD->imageScale, (isFlippedX()?0:180) + rand()%30-15);
        }
    }else if (skillEffectIndex == EFFECT_EXPLODE_MIDDLE && skillTarget){
        WORLD->runEffect(skillEffectIndex, skillTarget->getPosition() + Vec2(0, 30)*WORLD->imageScale, 180);
    }else if (skillEffectIndex == EFFECT_FIRE_SLASH && skillTarget){
        WORLD->runEffect(skillEffectIndex, skillTarget->getPosition() + Vec2(0, 30)*WORLD->imageScale, 30 + rand()%30);
    }else if (skillEffectIndex == EFFECT_LASER && skillTarget){
        Vec2 pos = getPosition();
        pos = Vec2(pos.x + 20*(isFlippedX()?1:-1), pos.y + 70);
        WORLD->showLaser(pos, skillTarget->getPosition());
    }else if (skillEffectIndex == EFFECT_HEAL && skillTarget){
    //        healNearFriend();
            WORLD->healHeroesNearPoint(this, getPosition(), ap*10, 3);
    }else{
        if(skillTarget){
            WORLD->runEffect(skillEffectIndex, skillTarget->getPosition() + Vec2(0, 30)*WORLD->imageScale, 180);
        }
    }
}
// bug - video random unit pick
void Movable::attack(){
    resetRoute();
    isInAttackMotion = false;
    if(!this || !target || target == nullptr || target->untouchable || target->energy <= 0 || target->isDead) {
        target = nullptr;
        if (unitAct == UNIT_ACT_ATTACK) {
            stopNew();
        }
        return;
    }
    if(target == nullptr || target->getParent() == nullptr){
        stopNew();
        return;
    }
    if(unitType == UNIT_GOBLIN_BOMB){
        WORLD->runEffect(EFFECT_EXPLODE_MIDDLE, getPosition());
        WORLD->runEffect(EFFECT_FIREBALL_EXPLOSION, getPosition());
        WORLD->splashDamage(getPosition(), 15500, ap, isEnemy, this);
        WORLD->removeDeadUnit((EnemyBase*)this);
        if (isEnemy) {
            MM->killedUnit(unitID, -1, -1, -1);
        }else{
            MM->deadUnit(unitID, -1, -1, -1);
        }
        return;
    }
    if (isBuilding && !isBuildingComplete) {
        return;
    }
    // skip attack
    if( isSkillOn &&
       (unitType == UNIT_HERO_ARCHER ||
        unitType == UNIT_HERO_HEALER)){
        stopSkillAura();
        isSkillOn = false;
           
        return;
    }
    if (isSkillOn){
        if (spine){
            stopSkillAura(); // test
        }
        if(skillTarget){
            if (unitType == UNIT_HERO_ORC ||
                unitType == UNIT_HERO_GOBLIN ||
                unitType == UNIT_HERO_LIZARDMAN ||
                unitType == UNIT_HERO_RUDOLPH ||
                unitType == UNIT_HERO_SANTADOG ||
                unitType == UNIT_HERO_ENT ||
                unitType == UNIT_HERO_GOLEM){
                WORLD->splashDamage(skillTarget->getPosition(), 10500, ap*1.5f, isEnemy, this);
            }else if (unitType == UNIT_HERO_SALAMANDER ||
                      unitType == UNIT_HERO_TANKER ||
                      unitType == UNIT_HERO_MOLE ||
                      unitType == UNIT_HERO_PENGUIN ){
                WORLD->splashDamage(skillTarget->getPosition(), 10500, ap*1.5f, isEnemy, this);
            }else if (unitType == UNIT_HERO_LADY_LION ||
                      unitType == UNIT_HERO_TOYMOUSE ||
                      unitType == UNIT_HERO_BATMONSTER ||
                      unitType == UNIT_HERO_WATERMELON ||
                      unitType == UNIT_HERO_MINO){
                WORLD->splashDamage(skillTarget->getPosition(), 14500, ap*2.0f, isEnemy, this);
            }else if(unitType == UNIT_HERO_WEREWOLF ||
                     unitType == UNIT_HERO_BEAR ||
                     unitType == UNIT_HERO_MONK ||
                     unitType == UNIT_HERO_ELF_SWORDMAN){
                WORLD->splashDamage(skillTarget->getPosition(), 100, ap*3, isEnemy, this);
            }else if(unitType == UNIT_HERO_ASSASSIN ||
                     unitType == UNIT_HERO_LAMIA){
                if(skillTarget && skillTarget->getHitAndIsDead(ap*6, this)){
                    WORLD->removeDeadUnit((EnemyBase*)target);
                    stopNew();
                }
            }else if(unitType == UNIT_HERO_CATINBOOTS){
                if(skillTarget && skillTarget->getHitAndIsDead(ap*4, this)){
                    WORLD->removeDeadUnit((EnemyBase*)target);
                    stopNew();
                }
            }
            WORLD->shakeScreenOnce();
        }
    }
    if (target == nullptr) {
        return;
    }
    if (unitType == UNIT_HERO_REAPER) {
        if(skillTarget){
            WORLD->runEffect(skillEffectIndex, skillTarget->getPosition() + Vec2(0, 30), 180);
        }else if(target){
            WORLD->runEffect(skillEffectIndex, target->getPosition() + Vec2(0, 30), 180);
        }
    }
    if (attackType == ATTACK_TYPE_NEAR) {
        if(unitType == UNIT_HERO_LADY_LION){
            WORLD->runEffect(EFFECT_HIT_WITH_CIRCLE_ON_GROUND, target->getPosition());
            WORLD->splashDamage(target->getPosition(), 10500, ap, isEnemy, this);
        }else if(unitType == UNIT_HERO_TOYMOUSE){
            WORLD->runEffect(EFFECT_FIREBALL_EXPLOSION, target->getPosition());
            WORLD->splashDamage(target->getPosition(), 10500, ap, isEnemy, this);
        }else{
            if (target->getHitAndIsDead(ap, this)){
                WORLD->removeDeadUnit((EnemyBase*)target);
                stopNew();
            }
        }
    }else if(attackType == ATTACK_TYPE_RANGE){
        float speed = 1000;
        float apValue = ap;
        if(unitType == UNIT_ARCHER || unitType == UNIT_WATCHERTOWER || unitType == UNIT_HERO_ARCHER){
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, ap, 0, "arrow.png");
            if(unitType == UNIT_WATCHERTOWER){
                speed = 1500;
            }
        }else if(unitType == UNIT_ORC_BUNKER || unitType == UNIT_ORC_HQ || unitType == UNIT_ORC_SPEAR){
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, ap, 0, "spear.png");
            speed = 1500;
        }else if(unitType == UNIT_HUMAN_SHIP || unitType == UNIT_ORC_SHIP){
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, ap, 0, "canonBall.png");
            missile->runAction(RotateBy::create(1.5f, 540));
        }else if(unitType == UNIT_HUMAN_BATTLE_SHIP){
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, ap, 0, "battleCanonBall.png");
            missile->runAction(RepeatForever::create(RotateBy::create(1.0f, 360)));
            missile->effectType = MISSILE_EFFECT_RED_SMOKE;
            missile->attackTargetType = ATTACK_TARGET_TYPE_SPLASH;
            speed = 350;
        }else if(unitType == UNIT_CATAPULT || unitType == UNIT_HELICOPTER){
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, ap, 0, "boulder.png");
            missile->runAction(RotateBy::create(2, 720));
            missile->runAction(ScaleTo::create(0.5f, 1.5f));
        }else if(unitType == UNIT_WIZARD){
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, ap, 0, "fireBall0.png");
            speed = 300;
        }else if(unitType == UNIT_HERO_WIZARD){
            if(isSkillOn){
                apValue *= 2;
            }
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, apValue, 0, "fireBall0.png");
            if(isSkillOn){
                missile->setScale(2);
            }
            speed = 600;
        }else if(unitType == UNIT_HERO_UNDINE){
            if(isSkillOn){
                apValue *= 1.8f;
            }
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, apValue, 0, "waterSplash0.png");
            if(isSkillOn){
                missile->setScale(1);
            }else{
                missile->setScale(0.5f);
            }
            speed = 600;
        }else if(unitType == UNIT_HERO_LADY_WEREWOLF){
            if(isSkillOn){
                apValue *= 1.8f;
            }
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, apValue, 0, "werewolfFurball.png");
            if(isSkillOn){
                missile->setScale(1);
            }else{
                missile->setScale(0.5f);
            }
            speed = 600;
        }else if(unitType == UNIT_HERO_LADY_BEAR){
            if(isSkillOn){
                apValue *= 1.8f;
            }
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, apValue, 0, "heart.png");
            if(isSkillOn){
                missile->setScale(2);
            }else{
                missile->setScale(1);
            }
            speed = 600;
        }else if(unitType == UNIT_HERO_HEALER){
//            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, ap, 0, "yellowBall.png");
//            speed = 300;
            healNearFriend();
            return;
        }else if(unitType == UNIT_HERO_REAPER){
            if(target->getHitAndIsDead(ap, this)){
                WORLD->removeDeadUnit((EnemyBase*)target);
                stopNew();
            }
            if (target != nullptr) {
                WORLD->runEffect(EFFECT_PURPLE_SLASH, target->getPosition());
            }
            
            isSkillOn = false;
            return;
        }else if(unitType == UNIT_HERO_SANTA){
            if(isSkillOn){
                apValue *= 2;
            }
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, apValue, 0, "present.png");
            if(isSkillOn){
                missile->setScale(2);
            }
            speed = 600;
        }else if(unitType == UNIT_HERO_PARASITE){
            if(isSkillOn){
                apValue *= 2;
            }
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, apValue, 0, "flyingWorm.png");
            if(isSkillOn){
                missile->setScale(2);
            }
            speed = 600;
        }else if(unitType == UNIT_HERO_BABYMINO){
            if(isSkillOn){
                apValue *= 1.5F;
            }
            missile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, apValue, 0, "babyMinoAxe.png");
            missile->runAction(RotateBy::create(1, 1000));
            if(isSkillOn){
                missile->setScale(1.5f);
            }
            speed = 600;
        }else{
            return;
        }
        if(WORLD->isInScreen(getPosition())){
            GM->playSoundEffect(SOUND_ARROW);
        }else{
            GM->playSoundEffect(SOUND_ARROW, 0.1f);
        }
        
        missile->ap = apValue;
        missile->target = target;
        missile->attackTargetType = attackTargetType;
        if(isSkillOn){
            if(unitType == UNIT_HERO_LADY_WEREWOLF ||
               unitType == UNIT_HERO_LADY_BEAR ||
               unitType == UNIT_HERO_PARASITE ||
               unitType == UNIT_HERO_BABYMINO ||
               unitType == UNIT_HERO_GOBLIN ||
               unitType == UNIT_HERO_KERBEROS){
                missile->attackTargetType = ATTACK_TARGET_TYPE_SPLASH;
            }
        }
        WORLD->batch->addChild(missile, 100);
//        this->getParent()->addChild(missile, 100);
        float distance = getPosition().distance(target->getPosition());
        float dur = distance/speed;
        float height = 600*dur;
        if(height > 250){
            height = 250;
        }
        bool fixAngle = true;
        if(unitType == UNIT_ARCHER || unitType == UNIT_ORC_SPEAR || unitType == UNIT_HERO_ARCHER){
            missile->runAction(Sequence::create(EaseOut::create(MoveBy::create(dur/2, Vec2(height*(isTowardLeft?-1:1), 0)), 2), EaseIn::create(MoveBy::create(dur/2, Vec2(-height*(isTowardLeft?-1:1), 0)), 2), NULL));
        }
        Vec2 extraPos = Vec2::ZERO;
        if(unitType == UNIT_WATCHERTOWER || unitType == UNIT_ORC_BUNKER || unitType == UNIT_ORC_HQ){ // straight
            if(unitType == UNIT_WATCHERTOWER || unitType == UNIT_HELICOPTER){
                extraPos = Vec2(0, 100);
            }
            missile->setRotation(-GM->getAngle(getPosition(), target->getPosition()) - 90);
        }else if(unitType == UNIT_WIZARD || unitType == UNIT_HERO_SANTA ||  unitType == UNIT_HERO_PARASITE || (unitType == UNIT_HERO_LADY_BEAR && !isSkillOn) || unitType == UNIT_HUMAN_BATTLE_SHIP){
            if(unitType == UNIT_HERO_SANTA){
                extraPos = Vec2(isFlippedX()?70:-70, 70);
            }
            fixAngle = false;
            // straight not rotated
        }else{ // curve
            missile->runAction(Sequence::create(EaseOut::create(MoveBy::create(dur/2, Vec2(0, height)), 2), EaseIn::create(MoveBy::create(dur/2, Vec2(0, -height)), 2), NULL));
            missile->setRotation(isTowardLeft?-45:45);
            if(unitType == UNIT_HERO_LADY_BEAR){
                fixAngle = false;
                missile->setRotation(0);
            }
        }
        if(fixAngle){
            missile->schedule(schedule_selector(Movable::updateMissileAngle), 0.05f);
            missile->updateMissileAngle(0);
        }
        missile->setPosition(getPosition() + extraPos);
        missile->untouchable = true;
        missile->shooter = this;
        missile->isEnemy = isEnemy;
        missile->runAction(Sequence::create(MoveTo::create(dur, target->getPosition()), CallFunc::create(CC_CALLBACK_0(Movable::onMissileMoveDone, missile)), NULL));
    }
    
    if (isSkillOn) {
        isSkillOn = false;
    }
}
void Movable::updateMissileAngle(float dt){
    if(lastMissilePosition != Vec2::ZERO){
        this->setRotation(-GM->getAngle(lastMissilePosition, getPosition()) - 90);
    }
    lastMissilePosition = getPosition();
//    this->setRotation(90);
}
void Movable::onMissileMoveDone(){
    removeFromParent();
    if (attackTargetType == ATTACK_TARGET_TYPE_SPLASH) {
        if(this->shooter && (this->shooter->unitType == UNIT_WIZARD || this->shooter->unitType == UNIT_HERO_WIZARD || this->shooter->unitType == UNIT_HERO_SANTA)){
            WORLD->runEffect(EFFECT_FIREBALL_EXPLOSION, getPosition());
        }else if(this->shooter && this->shooter->unitType == UNIT_HERO_UNDINE){
            WORLD->runEffect(EFFECT_WATER_SPLASH, getPosition());
        }else if(this->shooter && (this->shooter->unitType == UNIT_HERO_LADY_WEREWOLF
                                   || this->shooter->unitType == UNIT_HERO_BABYMINO)){
            WORLD->runEffect(EFFECT_HIT_WITH_CIRCLE_ON_GROUND, getPosition());
        }else if(this->shooter && this->shooter->unitType == UNIT_HERO_LADY_BEAR){
            WORLD->runEffect(EFFECT_HIT_WITH_CIRCLE_ON_GROUND, getPosition());
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
    Vec2 currentPos = getPosition();
    if (isBuilding || unitType == UNIT_TREE) {
        int offset = 2;
        Vec2 startPos = WORLD->getPositionFromTileCoordinate(buildingStartCoordinate.x, buildingStartCoordinate.y + buildingOccupySize.height) + Vec2(-TILE_SIZE/2, TILE_SIZE/2);
        approachingPoints[0] = startPos + Vec2(-offset, -offset);
        approachingPoints[1] = startPos + Vec2(buildingOccupySize.width*TILE_SIZE + offset, -offset);
        approachingPoints[2] = startPos + Vec2(-offset, buildingOccupySize.height*TILE_SIZE + offset);
        approachingPoints[3] = startPos + Vec2(buildingOccupySize.width*TILE_SIZE + offset, buildingOccupySize.height*TILE_SIZE + offset);
        index = 4;
        for (int i = 0; i < buildingOccupySize.width; i++) {
            approachingPoints[index] = startPos + Vec2(TILE_SIZE/2 + TILE_SIZE*i, -offset);
            approachingPoints[index + 1] = startPos + Vec2(TILE_SIZE/2 + TILE_SIZE*i, buildingOccupySize.height*TILE_SIZE + offset);
            index += 2;
        }
        for (int i = 0; i < buildingOccupySize.height; i++) {
            approachingPoints[index] = startPos + Vec2(-offset, TILE_SIZE/2 + TILE_SIZE*i);
            approachingPoints[index + 1] = startPos + Vec2(buildingOccupySize.width*TILE_SIZE + offset, TILE_SIZE/2 + TILE_SIZE*i);
            index += 2;
        }
    }else{
        int bottomY = getBoundingBox().origin.y;
        for (int x = -1; x < 2; x++) {
            for (int y = 0; y < 3; y++) {
                if(x == 0 && y == 1){
                    continue;
                }
                approachingPoints[index] = Vec2(currentPos.x + x*50, bottomY + y*50);
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

//        Sprite* spt = Sprite::createWithSpriteFrameName("workerAxeStand1.png");
//        WORLD->spriteBatchBuilding->addChild(spt);
//        spt->setPosition(approachingPoints[i]);
        
    }
}
// bug - AUBase.cpp:832:DispatchSetProperty:  ca_require: ValidFormat(inScope, inElement, newDesc) InvalidFormat
// bug - Thread 1: EXC_BAD_ACCESS (code=1, address=0x0)
cocos2d::Vec2 Movable::getApproachingPoint(cocos2d::Vec2 from){
//    if (lastApprochingPointCheckPosition != getPosition()) { // calculate points
//        refreshApproachPoints();
//    }
    Vec2 pos = this->getPosition();
    lastApprochingPointCheckPosition = pos;
    int minLength = 90000;
//    int pickedIndex = -1;
//    PointArray* array;
    Vec2 minPos;
    Vec2 coordinate, destCoordinate;
    
    for (int i = 0; i < approachPointCount; i++) {
        coordinate = WORLD->getCoordinateFromPosition(from, WORLD->theMap);
        destCoordinate = WORLD->getCoordinateFromPosition(approachingPoints[i], WORLD->theMap);
//        if(!WORLD->isCoordinateValid(destCoordinate) || WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(destCoordinate)) || !WORLD->isCoordinateValid(coordinate) || WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(coordinate))){
        if(WORLD->isOccupied(destCoordinate)){
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
// bug - enemy castle check. orc to human
bool Movable::getHitAndIsDead(int ap, Movable* attacker){
    if(energy <= 0 || isDead) return true;
//    if(WORLD->testSuper){
//        if(attacker != nullptr && attacker->alliSide == WHICH_SIDE_HERO){
//            ap = 10000;
//        }else{
//            ap = 0;
//        }
//    }
//    if (isEnemy) {
//        ap = 999999; // test 
//    }
    // Foundry upgrade bonus applies to ship units only.
    auto isShipType = [](int t) -> bool {
        return t == UNIT_HUMAN_SHUTTLE || t == UNIT_ORC_SHUTTLE ||
               t == UNIT_HUMAN_SHIP   || t == UNIT_ORC_SHIP    ||
               t == UNIT_HUMAN_BATTLE_SHIP ||
               t == UNIT_HUMAN_OIL_SHIP    || t == UNIT_ORC_OIL_SHIP;
    };
    if (attacker != nullptr && !attacker->isEnemy && isShipType(attacker->unitType)) {
        if (WORLD->isOrcTypeUnit(attacker->unitType))
            ap += WORLD->orcAttackLevel * 5;
        else
            ap += WORLD->humanAttackLevel * 5;
    }
    if (!isEnemy && isShipType(unitType)) {
        int armorLevel = WORLD->isOrcTypeUnit(unitType) ? WORLD->orcDefenseLevel : WORLD->humanDefenseLevel;
        ap = std::max(1, ap - armorLevel * 2);
    }
    energy -= ap;
    if(WORLD->isMultiplay){
        if(isEnemy){
//            if (energy <= 0) {
//                MM->killedUnit(unitID);
//            }
//            MultiplayManager::getInstance()->attackUnit(Value(attacker->unitID).asString(), this->unitID, ap);
        }else{
//            if (energy <= 0) {
//                energy = 1;
//            }
//            MM->killedUnit(unitID);
        }
    }
//    else{
//
//    }
    
    // b
    updateEnergy();
    if (isBuilding && energy < maxEnergy/2 && !isOnFire) {
        isOnFire = true;
        Sprite* spt = Sprite::createWithSpriteFrameName("fire0.png");
        WORLD->batch->addChild(spt);
        spt->setName("sptFire");
        spt->setPosition(getPosition() + Vec2(getContentSize().width/6, getContentSize().height/5));
        GM->runAnimation(spt, "fire", true);
        childrenSprite.pushBack(spt);
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
            if(canAttack && unitAct != UNIT_ACT_MOVE){
//                if (WORLD->isMultiplay && isEnemy) {
//
//                }else{
                    WORLD->revengeAttack(this, attacker);
                UnreachableUnitList.clear();
//                }
            }
        }
    }
    if(HUD->isRaid || unitType != UNIT_MINE){
        WORLD->runEffect(EFFECT_HIT, getPosition(), isTowardLeft?0:180);
    }
    
    if(WORLD->isMultiplay && energy <= 0){
        if(isEnemy){
            if (attacker == nullptr) {
                MM->killedUnit(unitID, -1, -1, -1);
            }else{
                MM->killedUnit(unitID, attacker->unitID, attacker->getPositionX(), attacker->getPositionY());
            }
            
        }else{
            if (attacker == nullptr) {
                MM->deadUnit(unitID, -1, -1, -1);
            }else{
                MM->deadUnit(unitID, attacker->unitID, attacker->getPositionX(), attacker->getPositionY());
            }
        }
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
            WORLD->batch->addChild(sptWing, 100);
            sptWing->setFlippedX(i==1);
            sptWing->setAnchorPoint(Vec2(i==0?0:1, 0));
            sptWing->setOpacity(0);
            sptWing->setScale(0.5f);
            sptWing->setPosition(Vec2(i==0?this->getContentSize().width + 3:-3, -this->getContentSize().height));
            float fadeInTime = 0.1f;
            float moveTime = 1;
            float fadeOutTime = 0.3f;
            sptWing->runAction(Sequence::create(DelayTime::create(delay), MoveBy::create(0.2f, Vec2(i==0?5:-5, 0)), nullptr));
            sptWing->runAction(Sequence::create(DelayTime::create(delay), EaseInOut::create(ScaleTo::create(0.2f, 1, 1), 2), nullptr));
            sptWing->runAction(Sequence::create(DelayTime::create(delay), EaseInOut::create(MoveBy::create(0.2f, Vec2(0, 15)), 2),  nullptr));
            sptWing->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0.1f), DelayTime::create(moveTime - fadeInTime - fadeOutTime), FadeOut::create(fadeOutTime), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sptWing)), nullptr));
            
        }
    }else if(effect == EFFECT_BOTTOM_RECT_GLOW){
        Sprite* spt = Sprite::createWithSpriteFrameName("glowRect.png");
        WORLD->batch->addChild(spt, -1);
        spt->setAnchorPoint(Vec2(0.5, 0.5));
        spt->setPosition(Vec2(getContentSize().width/2, getContentSize().height/2));
        spt->setOpacity(0);
        spt->setScale(1);
        spt->runAction(Sequence::create(DelayTime::create(delay), ScaleTo::create(0.2f, 2.5), nullptr));
        spt->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0.1f), DelayTime::create(1), FadeOut::create(0.3f), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
    }else if(effect == EFFECT_RISING_PARTICLE){
        float extraDelay = 0;
        for (int i = 0; i < 6; i++) { // sequence
            for (int j = 0; j < 3; j++) {   // particle at once
                Sprite* spt = Sprite::createWithSpriteFrameName("glowParticle.png");
                WORLD->batch->addChild(spt);
                spt->setOpacity(0);
                float fadeInTime = 0.1f;
                float moveTime = 0.4f;
                float fadeOutTime = 0.2f;
                
                spt->setPosition(Vec2(rand()%(int)this->getContentSize().width, rand()%(int)getContentSize().height + (rand()%10)*0.1f));
                spt->runAction(Sequence::create(DelayTime::create(delay + extraDelay), FadeIn::create(fadeInTime), DelayTime::create(moveTime - fadeInTime - fadeOutTime), FadeOut::create(fadeOutTime), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
                spt->runAction(Sequence::create(DelayTime::create(delay + extraDelay), MoveBy::create(moveTime, Vec2(0, 10)), nullptr));
            }
            extraDelay += 0.2f;
        }
    }else if(effect == EFFECT_PARTICLE_TORNADO){
        float extraDelay = 0;
        for (int i = 0; i < 5; i++) { // sequence
            extraDelay = i*0.07f;
            for (int j = 0; j < 16; j++) {   // particle at once
                Sprite* spt = Sprite::createWithSpriteFrameName("glowParticle.png");
                WORLD->batch->addChild(spt);
                spt->setPosition(Vec2(-3, i*3));
                spt->setOpacity(0);
//                spt->setScale(0.5f);
                spt->runAction(Sequence::create(DelayTime::create(delay + extraDelay), FadeIn::create(0.05f), DelayTime::create(0.1f), FadeOut::create(0.05f), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
                spt->runAction(Sequence::create(DelayTime::create(delay + extraDelay), MoveBy::create(0.2f, Vec2(this->getContentSize().width + 6, 0)), NULL));
                spt->runAction(Sequence::create(DelayTime::create(delay + extraDelay), EaseInOut::create(MoveBy::create(0.2f, Vec2(0, 5)), 2), nullptr));
                extraDelay += 0.08f;
            }
//            extraDelay += 0.1f;
        }
    }
}
void Movable::moveToTarget(Movable* unit){
    resetRoute();
    if(unit->isBuilding){
        moveFlagPos = unit->getApproachingPoint(getPosition());
    }else{
        moveFlagPos = unit->getPosition();
    }
    
    log("target to unit moveFlagPos: %f, %f, %d, %d", moveFlagPos.x, moveFlagPos.y, unit->unitType, unit->unitID);
    moveToPos = Vec2::ZERO;
    unitActDetail = UNIT_ACT_DETAIL_IDLE;
    if(this->unitType != UNIT_WORKER){
        if (WORLD->canAttack(this, unit)) {
            log("target can attack");
            target = unit;
        }else{
            log("target can't attack");
            target = nullptr;
        }
    }else{
        log("target to unit");
        target = unit;
    }
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
                WORLD->batch->addChild(dummyTarget);
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
//    moveFlagPos = pos;
//
//    moveToPos = Vec2::ZERO;
//    attackDdangPos = pos;
//    unitAct = UNIT_ACT_ATTACK_DDANG;
//    unitActDetail = UNIT_ACT_DETAIL_IDLE;
//    runAnimation(ANIMATION_TYPE_IDLE);
    
    // new
    moveToPos = Vec2::ZERO;
    moveFlagPos = pos;
    target = nullptr;
//    unitAct = UNIT_ACT_MOVE;
    canRevengeAttack = true;
    unitActDetail = UNIT_ACT_DETAIL_IDLE;
    cancelAttackSchedule();
    
    attackFlagPos = moveFlagPos;
    unitAct = UNIT_ACT_ATTACK_DDANG;
    target = nullptr;
//    log("attack ddang to %d", isEnemy);
}
void Movable::moveToTarget(cocos2d::Vec2 targetPos){
    Vec2 selfPos = WORLD->getCoordinateFromPosition(this->getPosition());
    targetMoveTilePos = WORLD->getCoordinateFromPosition(targetPos);
    log("movetotarget pos : %f, %f", targetPos.x, targetPos.y);
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
        
        Vec2 pos;
        int startI = (int)array->count()-1;
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
                    attackFlagPos = Vec2::ZERO;
                    stop();
                }
            }
        }
    }
}
void Movable::moveToTarget(){
    if(!canMove || target == nullptr){
        log("canmove: %d", (int)canMove);
        return;
    }
//    if(target == nullptr){
//        return;
//    }
//
    Vec2 newTargetPos;
    Vec2 currentPos = getPosition();
    if (lastValidPoint != Vec2::ZERO) {
//        currentPos = lastValidPoint;
    }
    
    Vec2 approachPos = target->getApproachingPoint(currentPos);
//    if(isFlying || isTemporaryFlying){
//        newTargetPos = target->getPosition();
//    }else{
        newTargetPos = WORLD->getCoordinateFromPosition(approachPos, WORLD->theMap);
//    }
    
    if(newTargetPos == targetCoordinate){
        return;
    }
    
    targetCoordinate = newTargetPos;
    Vec2 selfPos = WORLD->getCoordinateFromPosition(currentPos, WORLD->theMap);
    if(newTargetPos != targetMoveTilePos){
        log("moveToTaret set tilepos");
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
//            Vec2 nextPos = target->getPosition();
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
            
            Vec2 pos;
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
//                        lastValidPoint = Vec2::ZERO;
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
    isRouteSet = false;
    while(routePositionArray != nullptr && routePositionArray->count() > 0){
        routePositionArray->removeControlPointAtIndex(0);
    }
}
void Movable::addRoute(cocos2d::Vec2 dest){
    isArrived = false;
    movePointCounter = 0;

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
    this->isTemporaryFlying = true;
    
    this->unitAct = UNIT_ACT_GATHER_GOLD;
    this->unitActDetail = UNIT_ACT_DETAIL_IDLE;
    this->moveToPos = Vec2::ZERO;
    this->target = mine;
    this->moveFlagPos = mine->getApproachingPoint(getPosition());
//    this->moveToTarget(mine); // old
    this->currentMine = mine;
    this->gatheringTimer = 3;
//    runMoveAnimation(DIRECTION_E);
    
}

void Movable::gatherOil(Movable* extractor) {
    if (extractor == nullptr) {
        isGatheringOil = false;
        stop();
        return;
    }
    isGatheringOil = true;
    isGatheringGold = false;
    isGatheringTree = false;
    isInExtractor = false;
    isCarryingOil = false;
    oilCarryAmount = 0;
    unitAct = UNIT_ACT_MOVE;
    unitActDetail = UNIT_ACT_DETAIL_IDLE;
    moveToPos = Vec2::ZERO;
    target = extractor;
    moveFlagPos = extractor->getApproachingPoint(getPosition());
    currentOilExtractor = extractor;
    oilGatheringTimer = 5.0f;
}

void Movable::gatherTree(Movable* tree){
//    log("movable gather tree");
    
    this->currentTree = tree;
    if(tree == nullptr) {
        log("tree is null");
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

//    this->moveToTarget(tree);
//    unitAct = UNIT_ACT_GATHER_TREE;
    

        this->unitAct = UNIT_ACT_GATHER_TREE;
        this->unitActDetail = UNIT_ACT_DETAIL_IDLE;
        this->moveToPos = Vec2::ZERO;
        this->target = tree;
    this->moveFlagPos = tree->getApproachingPoint(getPosition());//tree->getPosition();
    //    this->moveToTarget(mine); // old
        
}
void Movable::attackTree(){
    if(currentTree != nullptr){
        Movable* tree = (Movable*)currentTree;
        if(tree->energy <= 0){ // someone took it
            gatherTree(WORLD->getNearestTree(getPosition(), ExcludeTreeList));
            return;
        }
        ExcludeTreeList.clear();
        tree->energy -= ap;
        if (WORLD->isMultiplay && isEnemy && tree->energy <= 0) {
            tree->energy = 1;
        }
        if(tree->energy <= 0){
            Vec2 treePos = WORLD->getCoordinateFromPosition(tree->getPosition(), WORLD->theMap);//Vec2((int)currentTree->getPositionX()/TILE_SIZE, (int)currentTree->getPositionY()/TILE_SIZE);
            WORLD->decoLayer->setTileGID(0, treePos);
            WORLD->updateMiniMapForNonMoving();
            WORLD->resetPathState();
            
            isCarryingTree = true;
            EnemyBase* tank = WORLD->getNearestLumberTank(getPosition(), isEnemy);
            unitName = strmake("%sWood", getName().c_str());
            if(tank != nullptr){
                returningPlace = tank;
//                isTemporaryFlying = true;
                
                moveToTarget(tank);
                log("tank pos: %f, %f", tank->getPositionX(), tank->getPositionY());
                unitAct = UNIT_ACT_GATHER_TREE;
                
                nextTree = WORLD->getNearestTree(getPosition(), ExcludeTreeList);
//                runMoveAnimation(DIRECTION_E);
            }else{
                runAnimation(ANIMATION_TYPE_IDLE);
            }
            WORLD->mutualArray.eraseObject((EnemyBase*)currentTree);
            
//            for(auto spt: tree->childrenSprite){
                tree->setSpriteFrame(strmake("treeTrunk%d.png", rand()%3));
                tree->setLocalZOrder(-10000);
            tree->runAction(Sequence::create(FadeOut::create(5), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, tree)), NULL));
//            }
//            tree->setLocalZOrder(-10000);
            tree->setLocalZOrder(-this->getBoundingBox().origin.y - 100);
            tree->childrenSprite.clear();
            if (WORLD->isMultiplay) {
                int tankID = -1;
                if(tank){
                    tankID = tank->unitID;
                }
                MM->cutTree(tree->unitID, unitID, getPositionX(), getPositionY(), tankID);
            }
//            Vec2 pos;
//            for(auto unit: WORLD->mutualArray){
//                pos = Vec2((int)unit->getPositionX()/TILE_SIZE, (int)unit->getPositionY()/TILE_SIZE);
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
    btn->setScale(0.4f*WORLD->imageScale);
    Sprite* sptBuilding = WORLD->getSpriteForUnit(index);
    btn->addClickEventListener(CC_CALLBACK_1(Movable::cancelProduct, this));
    btn->addChild(sptBuilding);
    sptBuilding->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height/2));
    sptBuilding->setOpacity(50);
    
    sptBuilding = WORLD->getSpriteForUnit(index);
    auto timer = ProgressTimer::create(sptBuilding);
    timer->setType(ProgressTimer::Type::RADIAL);
    btn->addChild(timer);
    timer->setName("timer");
    timer->setScale(2);
    //        timer->setReverseProgress(true);
    timer->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height/2));
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
//            cocos2d::Size size = Director::getInstance()->getWinSize();
//            spt->runAction(RepeatForever::create(Sequence::create(MoveTo::create(0, Vec2(size.width/2 - 750 - 100, 330 + 50)), MoveBy::create(0.5f, Vec2(150, -200)), DelayTime::create(1), NULL)));
//        }
        
        if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 8) {
            HUD->tutorialIndex++;
            HUD->tutorialIndex++;
            HUD->talkIndex = 0;
            HUD->talkText = LM->getText("tutorial 10");
            Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
            Sprite* sptTemp = Sprite::create("handIcon.png");
            spt->setScale(1);
            spt->setSpriteFrame(sptTemp->getSpriteFrame());
            cocos2d::Size size = Director::getInstance()->getWinSize();
            spt->stopAllActions();
            spt->runAction(RepeatForever::create(Sequence::create(MoveTo::create(0, Vec2(size.width/2 - 750, 330 + 50)), MoveBy::create(0.1f, Vec2(0, -100)), MoveBy::create(0.1f, Vec2(0, 100)), MoveBy::create(0.1f, Vec2(0, -100)), MoveBy::create(0.15f, Vec2(0, 100)), DelayTime::create(1), NULL)));
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
    if (isResearching) {
        researchTimer -= dt;
        if (researchTimer <= 0) {
            isResearching = false;
            WORLD->applyResearch(researchType);
            researchType = -1;
            // Remove research progress button
            if (!btns.empty()) {
                Button *resBtn = btns.at(0);
                btns.eraseObject(resBtn);
                resBtn->removeFromParent();
                updateProductButtons();
            }
            // Refresh HUD if this building is currently selected
            if (WORLD->selectedArray.size() == 1 &&
                (Movable *)WORLD->selectedArray.at(0) == this) {
                WORLD->updateMenu();
            }
        }
    }
    if(!isResearching && btns.size() > 0){
        productCompleteTimer -= dt;
        if(productCompleteTimer <= 0 && WORLD->foodMax >= WORLD->foodInUse+ GM->getFoodUseForUnit(btns.at(0)->getTag())){
            Button* btn = btns.at(0);
            int index = btn->getTag();
            std::string spriteName = WORLD->getSpriteNameForUnit(index);

            bool isShipUnit = (index == UNIT_HUMAN_SHUTTLE || index == UNIT_ORC_SHUTTLE ||
                               index == UNIT_HUMAN_SHIP || index == UNIT_ORC_SHIP ||
                               index == UNIT_HUMAN_BATTLE_SHIP || index == UNIT_ORC_BATTLE_SHIP ||
                               index == UNIT_HUMAN_OIL_SHIP || index == UNIT_ORC_OIL_SHIP);

            Vec2 spawnPos = approachingPoints[0];
            if (isShipUnit) {
                Vec2 tile = WORLD->getCoordinateFromPosition(getPosition());
                Vec2 emptyTile = WORLD->findEmptyWaterSpawnTile(
                    (int)buildingStartCoordinate.x, (int)buildingStartCoordinate.y,
                    (int)buildingOccupySize.width, (int)buildingOccupySize.height);
                if (emptyTile.x >= 0)
                    spawnPos = emptyTile;
            }

            EnemyBase* unit = WORLD->createUnit(index, isEnemy?WHICH_SIDE_ENEMY:WHICH_SIDE_HERO, ITS_NOT_BUILDING, spawnPos, GM->getUnitName(index), 1, spriteName.c_str());
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
            if (WORLD->isMultiplay && !isEnemy) {
                MM->createdUnit(unit->unitID, unit->unitType, (int)spawnPos.x, (int)spawnPos.y, WORLD->gameFrameTimer + LATANCY);
                unit->setVisible(false);
                unit->runAction(Sequence::create(DelayTime::create(LATANCY), CallFunc::create(CC_CALLBACK_0(Movable::visibleLater, unit)), NULL));
            }
        }
    }
}
void Movable::visibleLater(){
    setVisible(true);
}
void Movable::cancelProduct(Ref* ref){
    Button* btn = (Button*)ref;
    int index = btn->getTag();
    if(btns.at(0) == btn && btns.size() > 1 && btns.at(1) != nullptr){
        productCompleteTimer = WORLD->getUnitCreateTime(btns.at(1)->getTag());
    }
    if (!isEnemy) {
        WORLD->addGold(WORLD->getGoldPriceForUnit(index)*0.7f);
        WORLD->addLumber(WORLD->getLumberPriceForUnit(index)*0.7f);
    }
    btns.eraseObject(btn);
    btn->removeFromParent();
    updateProductButtons();
}

void Movable::cancelBuildingConstruction(Ref* ref){
    if (!isBuildingABuilding || builderBuilding == nullptr || isEnemy) {
        return;
    }
    isBuildingABuilding = false;
    Movable* building = builderBuilding;
    builderBuilding = nullptr;
    int unitType = building->unitType;

    stopAllActions();

    removeBuildProgressIcon();

    WORLD->addGold(WORLD->getGoldPriceForUnit(unitType));
    WORLD->addLumber(WORLD->getLumberPriceForUnit(unitType));

    building->stopAllActions();
    WORLD->removeIncompleteBuilding((EnemyBase*)building);

    untouchable = false;
    setVisible(true);
    if (!isCarryingGold && !isCarryingTree) {
        attackType = ATTACK_TYPE_NEAR;
    }
    stopNew();
}

void Movable::updateProductButtons(){
    int counter = 0;
    for(auto btn: btns){
//        int index = btn->getTag();
        btn->setPosition(this->getPosition() + Vec2(-buildingOccupySize.width*TILE_SIZE/2 + counter*80*WORLD->imageScale, TILE_SIZE));
        counter++;
    }
}
void Movable::stopNew(){
    resetRoute();
    stopSkillAura();
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
        return;
    }
    
    if (attackDdangPos != Vec2::ZERO) {
        attackDdangTo(attackDdangPos);
        return;
    }
    if (isGatheringTree) {
        isGatheringTree = false;
    }
//    if (isGatheringGold) {
//        isGatheringGold = false;
//    }
    // check multiple units in the same spot
    bool isShipUnit2 = (unitType == UNIT_HUMAN_SHUTTLE || unitType == UNIT_ORC_SHUTTLE ||
                         unitType == UNIT_HUMAN_SHIP || unitType == UNIT_ORC_SHIP ||
                         unitType == UNIT_HUMAN_BATTLE_SHIP || unitType == UNIT_HUMAN_OIL_SHIP ||
                         unitType == UNIT_ORC_OIL_SHIP);
    if (!isGatheringGold && !WORLD->isThisSpotAvailable(this) && canMove && !isFlying && !isShipUnit2) {
        Vec2 coordinate = WORLD->getCoordinateFromPosition(getPosition());
        Vec2 originalCoordinate = coordinate;
        isTemporaryFlying = !WORLD->isThisSpotAvailable(coordinate);
        bool goRandom = false;
        if (true || !isTemporaryFlying) {
            // get connected empty position
            int startX = 0;
            int max=0;
            int x=startX;
            int y=max;
            int direction = rand()%8;//GM->directionStatic;
//            GM->directionStatic++;
//            GM->directionStatic = GM->directionStatic%8;
//            bool changeDirection = false;
            int counter = 0;
            Vec2 pos;
            do{
                counter++;
                if(counter > 8){
                    goRandom = true;
                    break;
                }
                if (direction == 0) {
                    y++;
                }else if (direction == 1) {
                    x++;
                    y++;
                }else if (direction == 2) {
                    x++;
                }else if (direction == 3) {
                    x++;
                    y--;
                }else if (direction == 4) {
                    y--;
                }else if (direction == 5) {
                    x--;
                    y--;
                }else if (direction == 6) {
                    x--;
                }else if (direction == 7) {
                    x--;
                    y++;
                }
                if(originalCoordinate.y + x < 0) x = 0;
                if(originalCoordinate.y + y < 0) y = 0;
                if(originalCoordinate.y + x >= WORLD->mapSize.width) {
                    x = 0;
                }
                if(originalCoordinate.y + y >= WORLD->mapSize.height) {
                    y = 0;
                }
                pos = originalCoordinate + Vec2(x, y);
                if(GM->tileState[(int)pos.x][(int)pos.y] == 0 && counter > 1){
//                if(!WORLD->isOccupied(originalCoordinate + Vec2(x, y)) && counter > 1){
                    PointArray* array = PointArray::create(1);//GM->getPath(originalCoordinate, originalCoordinate + Vec2(x, y));
                    array->addControlPoint(originalCoordinate + Vec2(x, y));
                    if(array && array->count() > 0){
                        coordinate = originalCoordinate + Vec2(x, y);
                        break;
                    }
                }
                x = 0;
                y = 0;
            }while(true);
            
        }
//        else{
//            goRandom = true;
//        }
        if(goRandom){
            coordinate = Vec2(coordinate.x + rand()%3 - 1, coordinate.y + rand()%3 - 1);
        }
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
        
        Vec2 newPos = WORLD->getPositionFromTileCoordinate(coordinate.x, coordinate.y);
        //        WORLD->moveTo((EnemyBase*)this, newPos);
        
//        if (WORLD->isMultiplay) {
//            if (!isEnemy) {
//                unitAct = UNIT_ACT_MOVE;
//                moveToPos = Vec2::ZERO;
//                moveFlagPos = newPos;
//                MM->moveUnit(Value(unitID).asString(), (int)moveFlagPos.x, (int)moveFlagPos.y);
//            }
//        }
//        else
        {
            unitAct = UNIT_ACT_MOVE;
            moveToPos = Vec2::ZERO;
            moveFlagPos = newPos;
        }
    }
    
    if(isGoingToBuild){
        isGoingToBuild = false;
        log("builder building start");
        bool isFree = false;
        if(isEnemy){
            isFree = true;
        }
        Movable* unit = WORLD->buildTheBuilding(builderBuildingIndex, builderCoordinate.x, builderCoordinate.y, builderSize.width, builderSize.height, builderSpriteName, alliSide, isFree);
        if(unit == nullptr){ // condition not met
            return;
        }
        // progress

        float dur = WORLD->getUnitCreateTime(unit->unitType);
        if (!isEnemy) {
            Button* btn = Button::create("uiBox.png");
            btn->setPosition(unit->getPosition() + Vec2(50, unit->getContentSize().height/2 + 10));
            btn->setScale(0.3f);
            btn->setTag(unit->unitType);
            btn->addClickEventListener(CC_CALLBACK_1(Movable::cancelBuildingConstruction, this));
            WORLD->addChild(btn, 1000);
            buildProgressBtn = btn;
            auto timer = ProgressTimer::create(WORLD->getSpriteForUnit(unit->unitType));
            processTimer = timer;
            timer->setType(ProgressTimer::Type::RADIAL);
            btn->addChild(timer);
            timer->setName("timer");
            timer->setPosition(btn->getContentSize()/2);
            timer->setPercentage(0);
            log("building dur %f", dur);
            timer->runAction(Sequence::create(ProgressTo::create(dur, 100), CallFunc::create(CC_CALLBACK_0(Movable::resetProcessTimer, this)), CallFunc::create(CC_CALLBACK_0(Movable::removeBuildProgressIcon, this)), nullptr));
        }// progress end
        
        if(WORLD->selectedArray.find((EnemyBase*)this) != WORLD->selectedArray.end()){
            WORLD->deselect(this);
        }
        builderBuilding = unit;
        unit->setRotation(0);
        unit->buildingCompleteTime = dur;
        unit->checkVisible();
        unit->setSpriteFrame("underConstrunction0.png");
        GM->runAnimation(unit, "underConstrunction", true);
        log("builer building");
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
        this->isBuildingABuilding = true;
        this->attackType = ATTACK_TYPE_NONE;
        this->untouchable = true;
//        if (WORLD->isMultiplay && !isEnemy) {
//            MM->workerBuilding(unitID, builderBuildingIndex, builderCoordinate.x, builderCoordinate.y);
//        }
    }
}
void Movable::cancelAttackSchedule(){
    if (isSkillOn) {
        stopSkillAura();
    }
    this->stopActionByTag(attackTag);
}
void Movable::moveNew(float dt){// movenew start
    attackCoolTime -= dt;
    Vec2 prePos = getPosition();
    int previousAct = unitAct;
    if(unreachableTarget != nullptr && unreachableTarget == target){
        unitAct = UNIT_ACT_NONE;
    }
    if(checkAttackTargetReturnSuccess(dt)){
        return;
    }
    else{
        unitAct = previousAct;
    }
    
    bool isMoveToTargetFailed = false;

        // gathering gold
        if (isGatheringGold) {
            if (returningPlace != nullptr && isCarryingGold && returningPlace->getBoundingBoxForIntersect().intersectsRect(getBoundingBoxForIntersect())){
                if (!isEnemy) {
                    WORLD->addGold(40);
                } else {
                    WORLD->addEnemyGold(40);
                }
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
            cocos2d::Rect rect = currentMine->getBoundingBoxForIntersect();
            Vec2 pos = getPosition();
            
            if (!isCarryingGold && currentMine->getBoundingBoxForIntersect().intersectsRect(getBoundingBoxForIntersect())){
                if(currentMine->energy <= 0){
                    this->stopNew();
                    this->setVisible(true);
                    checkVisible();
                    this->untouchable = false;
                    currentMine = nullptr;
                    isGatheringGold = false;
                    isInMine = false;
                    return;
                }
                if(isVisible()){
                    this->setVisible(false);
                    this->stopNew();
                    this->isInMine = true;
                    this->untouchable = true;
                    this->attackType = ATTACK_TYPE_NONE;
                    this->unitAct = UNIT_ACT_GATHER_GOLD;
    //                this->unitAct = UNIT_ACT_ATTACK;
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
                        isInMine = false;
                        this->untouchable = false;
                        EnemyBase* castle = WORLD->getNearestCastle(getPosition(), isEnemy);
                        unitName = strmake("%sGold", getName().c_str());
                        if(castle != nullptr && currentMine != nullptr){
    //                        moveToTarget(castle);
                            returningPlace = castle;
                            target = returningPlace;
                            moveToPos = Vec2::ZERO;
                            moveFlagPos = target->getApproachingPoint(getPosition());//target->getPosition();
                            unitAct = UNIT_ACT_GATHER_GOLD;
                            unitActDetail = UNIT_ACT_DETAIL_IDLE;
                            int miner = currentMine->getTag();
                            miner--;
                            currentMine->setTag(miner);
                            if (miner == 0) {
                                currentMine->setSpriteFrame("mine.png");
                            }
    //                        MM->returnLumber(Value(unitID).asString(), returningPlace->unitID);
                        }
                        
                    }else{
                        this->stopNew();
                        this->setVisible(true);
                        isInMine = false;
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
            }else if (returningPlace != nullptr && isGatheringTree && isCarryingTree  && returningPlace->getBoundingBoxForIntersect().intersectsRect( getBoundingBoxForIntersect())){
                isCarryingTree = false;
                stop();
                unitName = getName();
                runAnimation(ANIMATION_TYPE_IDLE);
                if (!isEnemy) {
                    WORLD->addLumber(40);
                } else {
                    WORLD->addEnemyLumber(40);
                }

                Movable* tree;
                if (nextTree != nullptr) {
                    tree = nextTree;
                }else{
                    tree = WORLD->getNearestTree(getPosition(), ExcludeTreeList);
                }
                
                if(tree != nullptr){
                    gatherTree(tree);
                }
                
                return;
            }
        }
    // handle move
    if (!isBuilding && (unitAct == UNIT_ACT_MOVE ||
        unitAct == UNIT_ACT_ATTACK_DDANG ||
        unitAct == UNIT_ACT_ATTACK ||
        unitAct == UNIT_ACT_GATHER_GOLD ||
        unitAct == UNIT_ACT_GATHER_TREE) && unitActDetail != UNIT_ACT_DETAIL_ATTACK) {
        if (isTemporaryFlying) {
            if (!WORLD->isOccupied(WORLD->getCoordinateFromPosition(getPosition()))) {
                isTemporaryFlying = false;
                moveToPos = Vec2::ZERO;
                isRouteSet = false;
            }
        }
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
            Vec2 selfPos = WORLD->getCoordinateFromPosition(this->getPosition());
            
//            resetRoute();
            
//            if (target) { // check target occupy the ground
//                if(target->isBuilding){
//                    for (int j = 0; j < target->buildingOccupySize.height; j++) {
//                        for (int i = 0; i < target->buildingOccupySize.width; i++) {
//                            GM->setPathState(target->buildingStartCoordinate.x + i, target->buildingStartCoordinate.y + j, false);
//                        }
//                    }
//                }else if(target->unitType == UNIT_TREE || target->unitType == UNIT_TREE_FOR_BATTLE){
//                    Vec2 targetPos = WORLD->getCoordinateFromPosition(target->getPosition());
//                    GM->setPathState(targetPos.x, targetPos.y, false);
//                }
//            }
            // check ground under the feet occupied
//            bool isGroundUnderTheFeetOccupied = false;
//            Vec2 pos = WORLD->getCoordinateFromPosition(getPosition(), WORLD->theMap);
//            Movable* theObjectThisIsIn = nullptr;
//            if (GM->tileState[(int)pos.x][(int)pos.y] > 0) {
//                theObjectThisIsIn = WORLD->getGroundOwner(getPosition());
//                if (theObjectThisIsIn == nullptr) {
//                    stopNew();
//                }else{
//                    isGroundUnderTheFeetOccupied = true;
//                    if(theObjectThisIsIn->isBuilding){
//                        for (int j = 0; j < theObjectThisIsIn->buildingOccupySize.height; j++) {
//                            for (int i = 0; i < theObjectThisIsIn->buildingOccupySize.width; i++) {
//                                GM->setPathState(theObjectThisIsIn->buildingStartCoordinate.x + i, theObjectThisIsIn->buildingStartCoordinate.y + j, false);
////                                log("unlock block: %f, %f", theObjectThisIsIn->buildingStartCoordinate.x + i, theObjectThisIsIn->buildingStartCoordinate.y + j);
//                            }
//                        }
//                    }else if(theObjectThisIsIn->unitType == UNIT_TREE || theObjectThisIsIn->unitType == UNIT_TREE_FOR_BATTLE){
//                        Vec2 targetPos = WORLD->getCoordinateFromPosition(theObjectThisIsIn->getPosition());
//                        GM->setPathState(targetPos.x, targetPos.y, false);
//                    }
//                }
//            }
            PointArray* array;// = PointArray::create(1);
            bool isShipUnit = (unitType == UNIT_HUMAN_SHUTTLE || unitType == UNIT_ORC_SHUTTLE ||
                               unitType == UNIT_HUMAN_SHIP || unitType == UNIT_ORC_SHIP ||
                               unitType == UNIT_HUMAN_BATTLE_SHIP || unitType == UNIT_HUMAN_OIL_SHIP ||
                               unitType == UNIT_ORC_OIL_SHIP);
            if (isFlying || isTemporaryFlying) {
                unitActDetail = UNIT_ACT_DETAIL_WALK_STRAIGHT_TO_TARGET;
                array = PointArray::create(1);
                array->addControlPoint(moveToPos);
            } else if (isShipUnit) {
                // Ships use A* over water tiles to navigate around land and water buildings.
                array = GM->getPathForShip(selfPos, targetMoveTilePos);
                if (array == nullptr || array->count() == 0) {
                    // No water path found — fall back to straight line.
                    unitActDetail = UNIT_ACT_DETAIL_WALK_STRAIGHT_TO_TARGET;
                    array = PointArray::create(1);
                    array->addControlPoint(moveToPos);
                } else {
                    unitActDetail = UNIT_ACT_DETAIL_WALK_ROUTE;
                }
            } else if (!WORLD->isBlockExistBetween(getPosition(), moveToPos)) {
                unitActDetail = UNIT_ACT_DETAIL_WALK_STRAIGHT_TO_TARGET;
                array = PointArray::create(1);
                array->addControlPoint(moveToPos);
            } else {
                unitActDetail = UNIT_ACT_DETAIL_WALK_ROUTE;

                array = GM->getPath(selfPos, targetMoveTilePos);
                if(array == nullptr || !array || array->count() == 0){
                    if (isGatheringTree && !isCarryingTree) {
                        ExcludeTreeList.pushBack(currentTree);
                        gatherTree(WORLD->getNearestTree(getPosition(), ExcludeTreeList));
                        return;
                    }
                    if(target){
                        unreachableTarget = target;
                        if (alliSide == WHICH_SIDE_HERO) {
                            UnreachableUnitList.pushBack(target);
                            WORLD->findTargetEnemy(this);
                            target = nullptr;
                            return;
                        }
                    }
                    isMoveToTargetFailed = true;
                    stopNew();
                    failedFindPathStart = selfPos;
                    failedFindPathEnd = targetMoveTilePos;
                    if(targetMoveTilePos == attackFlagTilePos){
                        failedAttackFlagPos = attackFlagPos;
                        attackFlagPos = Vec2::ZERO;
                    }
                }
            }
//            if (target) { // putback target occupy the ground
//                if(target->isBuilding){
//                    for (int j = 0; j < target->buildingOccupySize.height; j++) {
//                        for (int i = 0; i < target->buildingOccupySize.width; i++) {
//                            GM->setPathState(target->buildingStartCoordinate.x + i, target->buildingStartCoordinate.y + j, true);
//                        }
//                    }
//                }else if(target->unitType == UNIT_TREE || target->unitType == UNIT_TREE_FOR_BATTLE){
//                    Vec2 targetPos = WORLD->getCoordinateFromPosition(target->getPosition());
//                    GM->setPathState(targetPos.x, targetPos.y, true);
//                }
//            }
//            if(isGroundUnderTheFeetOccupied){ // putback theBuildingThisIsIn occupied ground
//                if(theObjectThisIsIn->isBuilding){
//                    for (int j = 0; j < theObjectThisIsIn->buildingOccupySize.height; j++) {
//                        for (int i = 0; i < theObjectThisIsIn->buildingOccupySize.width; i++) {
//                            GM->setPathState(theObjectThisIsIn->buildingStartCoordinate.x + i, theObjectThisIsIn->buildingStartCoordinate.y + j, true);
//                        }
//                    }
//                }else if(theObjectThisIsIn->unitType == UNIT_TREE || theObjectThisIsIn->unitType == UNIT_TREE_FOR_BATTLE){
//                    Vec2 targetPos = WORLD->getCoordinateFromPosition(theObjectThisIsIn->getPosition());
//                    GM->setPathState(targetPos.x, targetPos.y, true);
//                }
//            }
            
            Vec2 targetPos = moveToPos;
            int startI = array->count()-1;
            if(startI > 1){
                startI--;
            }

            if (isCarryingTree && returningPlace != nullptr) {
                array->addControlPoint(WORLD->getCoordinateFromPosition( returningPlace->getPosition()));
            }
            Vec2 pos;
            for (int i = startI; i >= 0; i--) {
                pos = array->getControlPointAtIndex(i);
                addRoute(WORLD->getPositionFromTileCoordinate(pos.x, pos.y));
            }
            if (routePositionArray != nullptr && routePositionArray->count() > 0) {
                Vec2 coordinate = WORLD->getCoordinateFromPosition(targetPos);
                if(!WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(coordinate)) &&
                   !WORLD->isSoilBlock(WORLD->soilLayer->getTileGIDAt(coordinate))){
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
                        attackFlagPos = Vec2::ZERO;
                        stopNew();
                    }
                }
            }
        }

        if (unitAct == UNIT_ACT_ATTACK) {
            if(!target || target->energy <= 0){
//                attackCoolTime = 0;
                target = nullptr;
                stopNew();
//                MM->stopUnit(Value(unitID).asString());
                return;
            }else{
                if (moveFlagPos != target->getPosition()) {
//                    if (target->isBuilding) {
//                        moveFlagPos = target->getApproachingPoint(getPosition());
//                    }else{
                        moveFlagPos = target->getPosition();
//                    }
                    
                    moveToPos = moveFlagPos;
//                    MM->moveUnit(Value(target->unitID).asString(), moveFlagPos.x, moveFlagPos.y);
                }
            }
        }
        bool isAlreadyArrived = isArrived;
        if(attackDdangPos == Vec2::ZERO && routePositionArray != nullptr){
            if(unitActDetail == UNIT_ACT_DETAIL_WALK_ROUTE){
                isArrived = movePointCounter >= routePositionArray->count();
            }else if(unitActDetail == UNIT_ACT_DETAIL_WALK_STRAIGHT_TO_TARGET){
                isArrived = getPosition() == moveToPos;
            }
        }else{
            isArrived = getPosition() == attackDdangPos;
        }
        
        if (!isAlreadyArrived && isArrived && unitActDetail != UNIT_ACT_DETAIL_ATTACK && unitAct != UNIT_ACT_GATHER_GOLD){ // arrived
            if(attackDdangPos != Vec2::ZERO){
                attackDdangPos = Vec2::ZERO;
            }
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
//            if (!WORLD->isMultiplay) {
//                if (isEnemy && WORLD->heroArray.size() > 0) {
//                    EnemyBase* targetEnemy = nullptr;
//                    for(auto unit: WORLD->heroArray){
//                        if(unit->isFlying){
//                            continue;
//                        }
//                        targetEnemy = unit;
//                        break;
//                    }
//                    if(targetEnemy != nullptr){
//                        Vec2 pos = WORLD->heroArray.at(0)->getPosition();
//                        if(failedAttackFlagPos != pos){
//                            attackFlagPos = pos;
//                            attackFlagTilePos = WORLD->getCoordinateFromPosition(attackFlagPos, WORLD->theMap);
//                        }
//                    }
//                }
//            }
        }else{
            // Naval units (UNIT_HUMAN_SHUTTLE/UNIT_ORC_SHUTTLE) only - water-only
            // movement + ship-vs-ship collision. Re-registering the current
            // tile every frame (cheap no-op once already present) covers an
            // idle ship too, not just the actively-moving case below.
            bool isShip = (unitType == UNIT_HUMAN_SHUTTLE || unitType == UNIT_ORC_SHUTTLE ||
                           unitType == UNIT_HUMAN_SHIP || unitType == UNIT_ORC_SHIP ||
                           unitType == UNIT_HUMAN_BATTLE_SHIP || unitType == UNIT_HUMAN_OIL_SHIP ||
                           unitType == UNIT_ORC_OIL_SHIP);
            Vec2 tileBeforeThisFrame;
            if (isShip) {
                tileBeforeThisFrame = WORLD->getCoordinateFromPosition(getPosition());
                WORLD->shipOccupiedTiles.insert(std::make_pair((int)tileBeforeThisFrame.x, (int)tileBeforeThisFrame.y));
            }
            if ((unitActDetail == UNIT_ACT_DETAIL_WALK_ROUTE || unitActDetail == UNIT_ACT_DETAIL_WALK_STRAIGHT_TO_TARGET)) {
                Vec2 dest, current;
                if (unitActDetail == UNIT_ACT_DETAIL_WALK_ROUTE) {
                    if(routePositionArray->count() <= movePointCounter){
                        return;
                    }
                    dest = routePositionArray->getControlPointAtIndex(movePointCounter);
                }else{
                    dest = moveToPos;
                }
//                log("dest: %f, %f", dest.x, dest.y);
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
                    bool shipBlocked = false;
                    if (isShip) {
                        Vec2 t = WORLD->getCoordinateFromPosition(dest);
                        shipBlocked = !WORLD->isWaterTileAt((int)t.x, (int)t.y) ||
                                      WORLD->isShipTileBlocked((int)t.x, (int)t.y, this);
                    }
                    if (!shipBlocked) {
                        this->setPosition(dest);
                        movePointCounter++;
                    }
                }else{
                    Vec2 nextPos = current + Vec2(xWillMove, yWillMove);
                    bool shipBlocked = false;
                    if (isShip) {
                        Vec2 t = WORLD->getCoordinateFromPosition(nextPos);
                        shipBlocked = !WORLD->isWaterTileAt((int)t.x, (int)t.y) ||
                                      WORLD->isShipTileBlocked((int)t.x, (int)t.y, this);
                    }
                    if (!shipBlocked) {
                        this->setPosition(nextPos);
                    }
                }
                if (isShip) {
                    Vec2 tileAfter = WORLD->getCoordinateFromPosition(getPosition());
                    if ((int)tileAfter.x != (int)tileBeforeThisFrame.x || (int)tileAfter.y != (int)tileBeforeThisFrame.y) {
                        WORLD->shipOccupiedTiles.erase(std::make_pair((int)tileBeforeThisFrame.x, (int)tileBeforeThisFrame.y));
                        WORLD->shipOccupiedTiles.insert(std::make_pair((int)tileAfter.x, (int)tileAfter.y));
                    }
                }
                if (unitAct == UNIT_ACT_ATTACK) {
                    checkAttackTargetReturnSuccess(dt);
                }
                if(spine){
                    spine->setPosition(getPosition());
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
        energyBar->setPosition(getPosition() + Vec2(0, energyBarY));
        energyBarContent->setPosition(energyBar->getPosition() + Vec2(-18.5f, 0));
        energyBarBack->setPosition(energyBar->getPosition());
    }
    
    
//    if ((scheduledProductTime >= 0 || scheduledAttackTime >= 0) && WORLD->gameTimer > 240 && WORLD->gameTimer < 2400) {
    if ((scheduledProductTime >= 0 || scheduledAttackTime >= 0)) {
        
        if (WORLD->difficultyMode == DIFFICULTY_MODE_EASY && isEnemy) {
            scheduledProductTime = -1;
            scheduledAttackTime = -1;
        }
        
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
                log("move new");
                for(int i = 0;i < scheduledProductUnitCount; i++){
                    int unitType = scheduledProductUnit;
                    if(unitType == UNIT_ZOMBIE_SWORDSMAN){
                        unitType = rand()%2==0?UNIT_ZOMBIE_SWORDSMAN:UNIT_ZOMBIE_ORC_AXE;
                    }else if(unitType == UNIT_ORC_AXE){
                        unitType = rand()%2==0?UNIT_ORC_AXE:UNIT_ORC_SPEAR;
                    }
                    EnemyBase* createdUnit = WORLD->createUnit(unitType, isEnemy, false, this->getPosition() + Vec2(i*100*WORLD->imageScale, - 200*WORLD->imageScale), GM->getUnitName(unitType));
                    createdUnit->wantToEli = true;
                }
            }
        }
    }
    if (isMoveToTargetFailed) {
        unitAct = UNIT_ACT_RESTING_FOR_NEXT_TARGET_SEARCH;
        target = nullptr;
        restingTime = 0.5f;
        if (isGatheringTree) {
            gatherTree(WORLD->getNearestTree(getPosition(), ExcludeTreeList));
        }
    }
//    if (WORLD->isMultiplay) {
//        if (!isEnemy) {
//            if (target) {
//                if (target != targetSent) {
//                    targetSent = target;
//                    MM->targetChanged(unitID, target->unitID);
//                }
//            }else{
//                targetSent = nullptr;
//                if (moveFlagPos != moveFlagPosSent) {
//                    moveFlagPosSent = moveFlagPos;
//                    MM->moveFlagChanged(unitID, moveFlagPos.x, moveFlagPos.y);
//                }
//            }
//        }
//    }
    // movenew end
}
bool Movable::checkAttackTargetReturnSuccess(float dt){
    if(unitAct == UNIT_ACT_GATHER_GOLD || unitAct == UNIT_ACT_GATHER_TREE){
        return false;
    }
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
            if (unitType == UNIT_GOBLIN_BOMB) {
                if(target->getBoundingBoxForIntersect().containsPoint(getPosition())){
                    attackUpdate(dt);
                    return true;
                }else{
                    if (unitActDetail == UNIT_ACT_DETAIL_ATTACK) {
                        unitActDetail = UNIT_ACT_DETAIL_IDLE;
                        moveToPos = Vec2::ZERO;
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
    }
    return false;
}
void Movable::move(float dt){
    oneSecTimeChecker += dt;
    if (oneSec < oneSecTimeChecker) {
        oneSecTimeChecker = 0;
        if (isZombie) {
            if(target == nullptr && (routePositionArray == nullptr || routePositionArray->count() == 0)){
                Vec2 minPos;
                bool shouldMove = false;
                int minLength = 999999;
                for (int i = -1; i < 2; i++) {
                    for (int j = -1; j < 2; j++) {
                        if (i == 0 && j == 0) {
                            continue;
                        }
                        Vec2 newPos = Vec2(getPositionX() + i*TILE_SIZE, getPositionY() + j*TILE_SIZE);;
                        if (minLength > abs(getPositionX() - attackFlagPos.x + i*TILE_SIZE) + abs(getPositionY() - attackFlagPos.y + j*TILE_SIZE) && !WORLD->isDecoBlock(WORLD->decoLayer->getTileGIDAt(WORLD->getCoordinateFromPosition(newPos))) && WORLD->isThisSpotAvailable(newPos)) {
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
                log("move");
//                for(auto unit: WORLD->heroArray){
//                    if(unit->unitType == UNIT_CASTLE){
                        for(int i = 0;i < scheduledProductUnitCount; i++){
                            int unitType = scheduledProductUnit;
                            if(unitType == UNIT_ZOMBIE_SWORDSMAN){
                                unitType = rand()%2==0?UNIT_ZOMBIE_SWORDSMAN:UNIT_ZOMBIE_ORC_AXE;
                            }else if(unitType == UNIT_ORC_AXE){
                                unitType = rand()%2==0?UNIT_ORC_AXE:UNIT_ORC_SPEAR;
                            }
                            EnemyBase* createdUnit = WORLD->createUnit(unitType, isEnemy, false, this->getPosition() + Vec2(i*50, - 300), GM->getUnitName(unitType));
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
        
        energyBar->setPosition(getPosition() + Vec2(0, energyBarY));
        energyBarContent->setPosition(energyBar->getPosition() + Vec2(-18.5f, 0));
        energyBarBack->setPosition(energyBar->getPosition());
    }
    if (isGatheringGold) {
        if (returningPlace != nullptr && isCarryingGold && returningPlace->getApproachingPoint(getPosition()) == getPosition()){//returningPlace->getBoundingBox().containsPoint(this->getPosition())) {//returningPlace->getBoundingBox().intersectsRect(this->getBoundingBox())) {
            if(!isEnemy){
                WORLD->addGold(40);
            }
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
                isInMine = false;
                return;
            }
            if(isVisible()){
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
                    isInMine = false;
                    this->untouchable = false;
                    EnemyBase* castle = WORLD->getNearestCastle(getPosition(), isEnemy);
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
                    isInMine = false;
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
        }else if (returningPlace != nullptr && isGatheringTree && isCarryingTree  && returningPlace->getBoundingBoxForIntersect().intersectsRect( getBoundingBox())){//&& returningPlace->getBoundingBox().containsPoint(getPosition())) {
            isCarryingTree = false;
            stop();
//            this->setName("worker");
//            this->setName(getName().substr(0, getName().size() - 4));
            unitName = getName();
            WORLD->addLumber(40);
            
//            WORLD->moveTo((EnemyBase*)this, lastTreePos);
            
            Movable* tree = WORLD->getNearestTree(getPosition(), ExcludeTreeList);
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

    // Oil gathering (OilShip <-> OilExtractor)
    if (isGatheringOil) {
        if (isCarryingOil) {
            // Deliver oil: check bounding-box (Shipyard, water) or proximity to
            // approaching point (OilRefinery, land — ship stays on water).
            bool atDepot = false;
            if (returningPlace != nullptr) {
                if (returningPlace->getBoundingBoxForIntersect().intersectsRect(getBoundingBoxForIntersect())) {
                    atDepot = true;
                } else {
                    Vec2 approach = returningPlace->getApproachingPoint(getPosition());
                    if (approach.distanceSquared(getPosition()) < (float)(TILE_SIZE * TILE_SIZE)) {
                        atDepot = true;
                    }
                }
            }
            if (atDepot) {
                if (!isEnemy) {
                    WORLD->addOil(oilCarryAmount);
                }
                isCarryingOil = false;
                oilCarryAmount = 0;
                unitName = getName();
                this->attackType = ATTACK_TYPE_NEAR;
                this->canFindTarget = true;
                if (currentOilExtractor != nullptr) {
                    gatherOil(currentOilExtractor);
                }
            } else if (returningPlace != nullptr) {
                // Continuously update path toward depot (same as workers returning to castle)
                this->moveFlagPos = returningPlace->getApproachingPoint(getPosition());
                this->unitAct = UNIT_ACT_MOVE;
                this->unitActDetail = UNIT_ACT_DETAIL_IDLE;
                this->target = returningPlace;
            }
        } else if (currentOilExtractor != nullptr) {
            // Moving toward extractor; hide inside when reached
            if (currentOilExtractor->getBoundingBoxForIntersect().intersectsRect(getBoundingBoxForIntersect())) {
                if (!isInExtractor) {
                    isInExtractor = true;
                    this->setVisible(false);
                    this->stop();
                    this->untouchable = true;
                    this->attackType = ATTACK_TYPE_NONE;
                    this->canFindTarget = false;
                    WORLD->deselect(this);
                    oilGatheringTimer = 5.0f;
                }
                oilGatheringTimer -= dt;
                if (oilGatheringTimer < 0) {
                    // Compute carry amount: base 40, +30% if any OilRefinery exists
                    int baseAmount = 40;
                    bool hasRefinery = false;
                    for (auto unit : WORLD->heroArray) {
                        if (unit && (unit->unitType == UNIT_HUMAN_OIL_REFINERY ||
                                    unit->unitType == UNIT_ORC_OIL_REFINERY)) {
                            hasRefinery = true;
                            break;
                        }
                    }
                    oilCarryAmount = hasRefinery ? (int)(baseAmount * 1.3f) : baseAmount;
                    isCarryingOil = true;
                    isInExtractor = false;
                    this->setVisible(true);
                    this->untouchable = false;
                    this->attackType = ATTACK_TYPE_NEAR;
                    unitName = strmake("%sOil", getName().c_str());
                    EnemyBase* depot = WORLD->getNearestOilDepot(getPosition());
                    if (depot != nullptr) {
                        returningPlace = depot;
                        // Navigate like workers returning to castle
                        target = depot;
                        moveFlagPos = depot->getApproachingPoint(getPosition());
                        unitAct = UNIT_ACT_MOVE;
                        unitActDetail = UNIT_ACT_DETAIL_IDLE;
                        moveToPos = Vec2::ZERO;
                    }
                }
            }
        }
    }

    if(target == nullptr && attackFlagPos == Vec2::ZERO){
        return;
    }

    if(delayFrameTimeBeforeMove > 0){
        delayFrameTimeBeforeMove--;
        return;
    }
    targetPositionCheckTime -= dt;
    if(targetPositionCheckTime < 0 && target != nullptr && target->canMove && !target->untouchable && canMove && !target->isGatheringGold && !target->isGatheringTree){
        targetPositionCheckTime = 1;
        
//        Vec2 newTargetPos = WORLD->getCoordinateFromPosition(target->getBoundingBox().origin + Vec2(target->getBoundingBox().size.width/2, target->collisionBoundingBox().size.height/2), WORLD->theMap);
        Vec2 newTargetPos;
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
                EnemyBase* tank = WORLD->getNearestLumberTank(getPosition(), isEnemy);
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
                EnemyBase* tree = WORLD->getNearestTree(getPosition(), ExcludeTreeList);
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
//        targetCoordinate = Vec2::ZERO;
//        resetRoute();
//        moveToTarget();
    }else{
        Vec2 dest = routePositionArray->getControlPointAtIndex(movePointCounter);
        Vec2 current = this->getPosition();
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
            this->setPosition(current + Vec2(xWillMove, yWillMove));
            
        }
        if (sptSelectedCircle != nullptr) {
            sptSelectedCircle->setPosition(getEffectStartPosition());
        }
        if (ndLevelCircle != nullptr) {
            ndLevelCircle->setPosition(getEffectStartPosition());
        }
    }
}
cocos2d::Rect Movable::getBoundingBoxForIntersect(){
    if (isBuilding || unitType == UNIT_TREE) {
        return Sprite::getBoundingBox();
    }else if(spine){
        return cocos2d::Rect(getPositionX()-50, getPositionY()-30, 100, 130);
    }else{
        return cocos2d::Rect(getPositionX()-30, getPositionY()-30, 60, 60);
    }
}
//cocos2d::Rect Movable::getBoundingBox() const {
//    if (spine || isBuilding) {
//        return Sprite::getBoundingBox();
//    }else{
//        return cocos2d::Rect(getPositionX()-50, getPositionY()-30, 100, 60);
//    }
//}
Vec2 Movable::getEffectStartPosition(){
    if(isBuilding){
        if (unitType == UNIT_WATCHERTOWER) {
            return getPosition() + Vec2(0, (-80)*WORLD->imageScale);
        }else if (unitType == UNIT_AIRPORT) {
            return getPosition() + Vec2(0, (-80)*WORLD->imageScale);
        }else if (unitType == UNIT_BARRACKS) {
            return getPosition() + Vec2((-10)*WORLD->imageScale, (-54)*WORLD->imageScale);
        }else{
            return getPosition() + Vec2(0, (-getContentSize().height/2 + 25)*WORLD->imageScale);
        }
        
//        return getPosition();
    }else{
//        return getPosition() + Vec2(0, 14*WORLD->imageScale);//Vec2(0, -getContentSize().height/2 + 30);
        return getPosition() + Vec2(0, 0);
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
        spt->setPosition(this->getPosition() + Vec2(-7 + rand()%16, 4 - rand()%9));
        spt->runAction(Sequence::create(MoveBy::create(1.5f, Vec2(0, 32)), SPT_REMOVE_FUNC, NULL));
        spt->runAction(Sequence::create(DelayTime::create(1.0f), FadeOut::create(0.5f), NULL));
    }
}

void Movable::updateEnergy(){
    if(energyBar == nullptr){
//        batchNode* batch = WORLD->batch;
        Node* batch = WORLD->batch;
        if(this->isBuilding){
            batch = WORLD->batch;
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
        
        batch->addChild(energyBarBack);
        energyBarBack->setColor(Color3B(38, 44, 52));
        
        batch->addChild(energyBarContent);
        energyBarContent->setAnchorPoint(Vec2(0, 0.5));
        
        batch->addChild(energyBar);

        if(energyBarY < 0){
            if(spine){
                energyBarY = this->spine->getBoundingBox().size.height*0.9f;
            }else if(isBuilding){
                energyBarY = 60*WORLD->imageScale;
            }else{
                energyBarY = this->getBoundingBox().size.height*0.9f;
            }
        }
        
        energyBar->setPosition(getPosition() + Vec2(0, energyBarY));
        energyBarContent->setPosition(energyBar->getPosition() + Vec2(-18.5f, 0));
        energyBarBack->setPosition(energyBar->getPosition());
    }
    float rate = energy*1.0f/maxEnergy;
    cocos2d::Rect textureRect = energyBarContent->getTextureRect();
    energyBarContent->setTextureRect(cocos2d::Rect(textureRect.origin.x, textureRect.origin.y, 37*rate, 12));
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
    spt->runAction(Sequence::create(MoveBy::create(dur, Vec2(200, 0)), SPT_REMOVE_FUNC, NULL));
    spt->runAction(Sequence::create(EaseOut::create(MoveBy::create(dur*2/3, Vec2(0, 200)), 2), MoveBy::create(dur/3, Vec2(0, 10)), nullptr));
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
                WORLD->spriteBatchBuilding->addChild(sptSelectedCircle, -20000);
            }
            if(spine){
                sptSelectedCircle->setScale((this->getContentSize().width)*1.5f/sptSelectedCircle->getContentSize().width);
            }else{
                if(isBuilding){
                    sptSelectedCircle->setScale((this->getContentSize().width)*1.3f/sptSelectedCircle->getContentSize().width);
                }else{
                    sptSelectedCircle->setScale((this->getContentSize().width)/sptSelectedCircle->getContentSize().width);
                }
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
        isInMine = false;
    }else{
        if (sptSelectedCircle != nullptr) {
            sptSelectedCircle->setVisible(false);
        }
    }
}
void Movable::setPosition(const Vec2 &position){
    Sprite::setPosition(position);
    if(spine){
        spine->setPosition(position);
    }
}

void Movable::checkVisible(){
    if (isInMine || isBuildingABuilding || isInExtractor || isInShuttle) {
//        if(isBuildingABuilding)
//            log("isbuilding %d", rand()%3);
    }else{
        this->setVisible(!isUnderFog && isInScreen);
        if(spine){
            spine->setVisible(!isUnderFog && isInScreen);
        }
    }
}
void Movable::setVisible(bool visible){
    Sprite::setVisible(visible);
    if(energyBar != nullptr){
        energyBar->setVisible(visible);
        energyBarContent->setVisible(visible);
        energyBarBack->setVisible(visible);
    }
}
