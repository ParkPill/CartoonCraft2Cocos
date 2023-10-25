#ifndef _Button_H_
#define _Button_H_

#include "cocos2d.h"

USING_NS_CC;

typedef enum tagButtonState 
{
    kButtonStateGrabbed,
    kButtonStateUngrabbed
} ButtonState; 

class LegendDaryButton : public Sprite//, public TargetedTouchDelegate
{
    ButtonState        m_state;
    int btnType;
    cocos2d::Size size;
//    bool isOrigianlScaleSet;
public:
    
    float originalScale;
    bool enabled;
//    Texture2D* leftDownTexture;
//    Texture2D* rightDownTexture;
//    Texture2D* downTexture;
    Texture2D* normalTexture;
    LegendDaryButton();
    ~LegendDaryButton();

    cocos2d::Rect rect();
    bool initWithTexture(Texture2D* aTexture);

    void enableButton(bool enable);
    void setButtonType(int buttonType);
    void addListener();
//    virtual void onEnter();
//    virtual void onExit();
    EventListenerTouchOneByOne* listener;
    void removeUsedAssets();
    bool containsTouchLocation(Touch* touch);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
//    virtual Ref* copyWithZone(CCZone *pZone);

    virtual void touchDelegateRetain();
    virtual void touchDelegateRelease();

    static LegendDaryButton* buttonWithTexture(Texture2D* aTexture, int buttonType);
//    static LegendDaryButton* buttonWithTexture(Texture2D* aTexture, Texture2D* down, int buttonType);
//    static LegendDaryButton* buttonWithTexture(Texture2D* aTexture, Texture2D* leftDown, Texture2D* rightDown, int buttonType);
};

#endif
