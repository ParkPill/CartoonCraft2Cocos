#ifndef __SNEAKY_JOYSTICK_H__
#define __SNEAKY_JOYSTICK_H__
#include "ui/UIImageView.h"
#include "cocos2d.h"
using namespace cocos2d;
#define STICK_LEFT 0
#define STICK_RIGHT 1
class SneakyJoystick : public cocos2d::Node
{
public:
    int stickType = 0;
    CREATE_FUNC(SneakyJoystick);
    cocos2d::Size size;
    EventListenerTouchOneByOne* touchListener = nullptr;
    void removeListener();
    cocos2d::ui::ImageView* sptBack = nullptr; // sppark
    bool isForceTouched = false;
    void addListener();
protected:
	float joystickRadiusSq;
	float thumbRadiusSq;
	float deadRadiusSq;
    
	CC_SYNTHESIZE_READONLY(cocos2d::Point, stickPosition, StickPosition);
	CC_SYNTHESIZE_READONLY(float, degrees, Degrees);
	CC_SYNTHESIZE_READONLY(cocos2d::Point, velocity, Velocity);
	CC_SYNTHESIZE(bool, autoCenter, AutoCenter);
	CC_SYNTHESIZE_READONLY(bool, isDPad, IsDPad);
	CC_SYNTHESIZE(bool, hasDeadzone, HasDeadzone);
	CC_SYNTHESIZE(int, numberOfDirections, NumberOfDirections);

	CC_SYNTHESIZE_READONLY(float, joystickRadius, JoystickRadius);
	CC_SYNTHESIZE_READONLY(float, thumbRadius, ThumbRadius);
	CC_SYNTHESIZE_READONLY(float, deadRadius, DeadRadius);
    CC_SYNTHESIZE_READONLY(float, angleRad, AngleInRadian);
    
//	virtual ~SneakyJoystick();

	virtual bool init();
	virtual void onEnterTransitionDidFinish();
//	virtual void onExit();
	void setIsDPad(bool b);
	void setJoystickRadius(float r);
	void setThumbRadius(float r);
	void setDeadRadius(float r);
//	virtual bool ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event);
//	virtual void ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent *event);
//	virtual void ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event);
//	virtual void ccTouchCancelled(cocos2d::CCTouch *touch, cocos2d::CCEvent *event);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(Touch *touch, Event *unused_event);
    float round(float r) ;

private:
	void updateVelocity(cocos2d::Point point);
	void setTouchRadius();
    
};
#endif
