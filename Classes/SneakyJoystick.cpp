#include "SneakyJoystick.h"
#include "GameManager.h"
using namespace cocos2d;

#define SJ_PI 3.14159265359f
#define SJ_PI_X_2 6.28318530718f
#define SJ_RAD2DEG 180.0f/SJ_PI
#define SJ_DEG2RAD SJ_PI/180.0f
//
//SneakyJoystick::~SneakyJoystick()
//{
//}

bool SneakyJoystick::init()
{
	bool pRet = false;
	//if(Sprite::init()){
		stickPosition = Vec2::ZERO;
		degrees = 0.0f;
		velocity = Vec2::ZERO;
		autoCenter = true;
		isDPad = false;
		hasDeadzone = false;
		numberOfDirections = 4;
		
		this->setJoystickRadius(100);
		this->setThumbRadius(32.0f);
		this->setDeadRadius(0.0f);

    
        addListener();
    
		//Cocos node stuff
		setPosition(Vec2(0,0));
		pRet = true;
	//}
    size = Director::getInstance()->getWinSize();
	return pRet;
}

void SneakyJoystick::addListener(){
    touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(SneakyJoystick::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(SneakyJoystick::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(SneakyJoystick::onTouchEnded, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(SneakyJoystick::onTouchCancelled, this);
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}
void SneakyJoystick::onEnterTransitionDidFinish()
{
//    auto dispatcher = Director::getInstance()->getEventDispatcher();
//    addListener();
}

//void SneakyJoystick::onExit()
//{
//    auto dispatcher = Director::getInstance()->getEventDispatcher();
//    dispatcher->removeEventListenersForTarget(this);
////	CCDirector::getInstance()->getTouchDispatcher()->removeDelegate(this);
//}

float SneakyJoystick::round(float r) {
    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

void SneakyJoystick::updateVelocity(Vec2 point)
{
	// Calculate distance and angle from the center.
	float dx = point.x;
	float dy = point.y;
	float dSq = dx * dx + dy * dy;
	
	if(dSq <= deadRadiusSq){
		velocity = Vec2::ZERO;
		degrees = 0.0f;
		stickPosition = point;
        
        
        if (stickType == STICK_LEFT) {
            GameManager::getInstance()->buttonUp(BUTTON_LEFT);
            GameManager::getInstance()->buttonUp(BUTTON_RIGHT);
            sptBack->setRotation(0);
        }else if(stickType == STICK_RIGHT){
//            GameManager::getInstance()->getHudLayer()->cursorLayer->isMouseLeftDown = false;
        }
        
        GameManager::getInstance()->buttonUp(BUTTON_LEFT);
        GameManager::getInstance()->buttonUp(BUTTON_RIGHT);
        GameManager::getInstance()->buttonUp(BUTTON_UP);
        GameManager::getInstance()->buttonUp(BUTTON_DOWN);
        GM->getHudLayer()->onLeftRelease();
        GM->getHudLayer()->onRightRelease();
        GM->getHudLayer()->onUpRelease();
        GM->getHudLayer()->onDownRelease();
        sptBack->loadTexture("4direction_pad.png");
		return;
	}

	float angle = atan2f(dy, dx); // in radians
	if(angle < 0){
		angle		+= SJ_PI_X_2;
	}
    
	float cosAngle;
	float sinAngle;
	
	if(isDPad){
		float anglePerSector = 360.0f / numberOfDirections * SJ_DEG2RAD;
		angle = round(angle/anglePerSector) * anglePerSector;
	}
	
	cosAngle = cosf(angle);
	sinAngle = sinf(angle);
    
    angleRad = angle;
	
	// NOTE: Velocity goes from -1.0 to 1.0.
	if (dSq > joystickRadiusSq || isDPad) {
		dx = cosAngle * joystickRadius;
		dy = sinAngle * joystickRadius;
	}
	
	velocity = Vec2(dx/joystickRadius, dy/joystickRadius);
	degrees = angle * SJ_RAD2DEG;
	
	// Update the thumb's position
	stickPosition = Vec2(dx, dy);
    int vel = 5;
    if (stickType == STICK_LEFT) {
        if(abs(dx) < vel && abs(dy) < vel){
            GameManager::getInstance()->buttonUp(BUTTON_LEFT);
            GameManager::getInstance()->buttonUp(BUTTON_RIGHT);
            GameManager::getInstance()->buttonUp(BUTTON_UP);
            GameManager::getInstance()->buttonUp(BUTTON_DOWN);
            sptBack->loadTexture("4direction_pad.png");
        }else if (degrees < 45 || degrees >= 315) {
            GameManager::getInstance()->buttonUp(BUTTON_LEFT);
            GameManager::getInstance()->buttonDown(BUTTON_RIGHT);
            GameManager::getInstance()->buttonUp(BUTTON_UP);
            GameManager::getInstance()->buttonUp(BUTTON_DOWN);
            GM->getHudLayer()->onRightPress();
            sptBack->loadTexture("4direction_pad_right.png");
//            sptBack->setFlippedX(false);
        }else if (degrees >= 135 && degrees < 225) {
            GameManager::getInstance()->buttonDown(BUTTON_LEFT);
            GameManager::getInstance()->buttonUp(BUTTON_RIGHT);
            GameManager::getInstance()->buttonUp(BUTTON_UP);
            GameManager::getInstance()->buttonUp(BUTTON_DOWN);
            GM->getHudLayer()->onLeftPress();
            sptBack->loadTexture("4direction_pad_left.png");
//            sptBack->setFlippedX(true);
        }else if (degrees >= 45 && degrees < 135) {
            GameManager::getInstance()->buttonUp(BUTTON_LEFT);
            GameManager::getInstance()->buttonUp(BUTTON_RIGHT);
            GameManager::getInstance()->buttonDown(BUTTON_UP);
            GameManager::getInstance()->buttonUp(BUTTON_DOWN);
            GM->getHudLayer()->onUpPress();
            sptBack->loadTexture("4direction_pad_up.png");
        }else if (degrees >= 225 && degrees < 315) {
            GameManager::getInstance()->buttonUp(BUTTON_LEFT);
            GameManager::getInstance()->buttonUp(BUTTON_RIGHT);
            GameManager::getInstance()->buttonUp(BUTTON_UP);
            GameManager::getInstance()->buttonDown(BUTTON_DOWN);
            GM->getHudLayer()->onDownPress();
            sptBack->loadTexture("4direction_pad_down.png");
        }
    }else if(stickType == STICK_RIGHT){
        if (dx > vel || dy > vel || dx < vel || dy < vel) {
//            GameManager::getInstance()->getHudLayer()->cursorLayer->isMouseLeftDown = isForceTouched;
        }else{
//            GameManager::getInstance()->getHudLayer()->cursorLayer->isMouseLeftDown = false;
        }
        Vec2 location = Vec2(size.width/2 + dx*6, size.height/2 + dy*6);
//        GameManager::getInstance()->getHudLayer()->cursorLayer->cursor->setPosition(location);
//        GameManager::getInstance()->getHudLayer()->cursorLayer->cursor->setRotation(degrees*3);
    }
}

void SneakyJoystick::setIsDPad(bool b)
{
	isDPad = b;
	if(isDPad){
		hasDeadzone = true;
		this->setDeadRadius(10.0f);
	}
}

void SneakyJoystick::setJoystickRadius(float r)
{
	joystickRadius = r;
	joystickRadiusSq = r*r;
}

void SneakyJoystick::setThumbRadius(float r)
{
	thumbRadius = r;
	thumbRadiusSq = r*r;
}

void SneakyJoystick::setDeadRadius(float r)
{
	deadRadius = r;
	deadRadiusSq = r*r;
}

bool SneakyJoystick::onTouchBegan(Touch *touch, Event *unused_event)
{
    if(!this->isVisible()){
        return false;
    }
	Vec2 location = CCDirector::getInstance()->convertToGL(touch->getLocationInView());
	//if([background containsPoint:[background convertToNodeSpace:location]]){
	location = this->convertToNodeSpace(location);
	//Do a fast rect check before doing a circle hit check:
	if(location.x < -joystickRadius || location.x > joystickRadius || location.y < -joystickRadius || location.y > joystickRadius){
		return false;
	}else{
		float dSq = location.x*location.x + location.y*location.y;
		if(joystickRadiusSq > dSq){
			this->updateVelocity(location);
			return true;
		}
	}
    isForceTouched = touch->getCurrentForce() > 3;
	return false;
}

void SneakyJoystick::onTouchMoved(Touch *touch, Event *unused_event)
{
    if(!this->isVisible()){
        return;
    }
	Vec2 location = CCDirector::getInstance()->convertToGL(touch->getLocationInView());
	location = this->convertToNodeSpace(location);
	this->updateVelocity(location);
    if (touch->getCurrentForce() > 0) {
        
    }
//    log("force: %f", touch->getCurrentForce());
    isForceTouched = touch->getCurrentForce() > 3;
}

void SneakyJoystick::onTouchEnded(Touch *touch, Event *unused_event)
{
    if(!this->isVisible()){
        return;
    }
    Vec2 location = Vec2::ZERO;
	if(!autoCenter){
		Vec2 location = CCDirector::getInstance()->convertToGL(touch->getLocationInView());
		location = this->convertToNodeSpace(location);
	}
	this->updateVelocity(location);
    isForceTouched = touch->getCurrentForce() > 3;
}

void SneakyJoystick::onTouchCancelled(Touch *touch, Event *unused_event)
{
	this->onTouchEnded(touch, unused_event);
}

void SneakyJoystick::removeListener(){
    Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
    touchListener = nullptr;
}
