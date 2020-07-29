#include "LegendDaryButton.h"
#include "GameManager.h"
LegendDaryButton::LegendDaryButton()
{
    size = Director::getInstance()->getWinSize();
//    isOrigianlScaleSet = false;
//    this->addListener();
}

LegendDaryButton::~LegendDaryButton()
{
    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
}
void LegendDaryButton::addListener(){
    listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    
    listener->onTouchBegan = [&](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        if (GameManager::getInstance()->getWorld() && !GameManager::getInstance()->getWorld()->isSetStageDone) {
            return true;
        }
        Vec2 p = touch->getLocation();
        cocos2d::Rect rect = this->getBoundingBox();
        
        if(rect.containsPoint(p))
//        if(rect.getMinX() <= p.x && p.x <= rect.getMaxX())
        {
            this->onTouchBegan(touch, event);
            return true; // to indicate that we have consumed it.
        }
        
        return false; // we did not consume this event, pass thru.
    };
    
    listener->onTouchMoved = [=](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        if (!GameManager::getInstance()->getWorld()->isSetStageDone) {
            return;
        }
        this->onTouchMoved(touch, event);
    };
    
    listener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        if (!GameManager::getInstance()->getWorld()->isSetStageDone) {
            return;
        }
        this->onTouchEnded(touch, event);
    };
    
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 30);
}
cocos2d::Rect LegendDaryButton::rect()
{
    cocos2d::Size s = getTexture()->getContentSize();
    s = cocos2d::Size(s.width*this->getScale(), s.height*this->getScale());
    return cocos2d::Rect(-(s.width / 2)*1.3f, -(s.height/2)*1.3f, s.width*1.3f, s.height*1.3f);
}
LegendDaryButton* LegendDaryButton::buttonWithTexture(Texture2D* aTexture, int buttonType)
{
    LegendDaryButton* pButton = new LegendDaryButton();
    pButton->enabled = true;
    pButton->setButtonType(buttonType);
    pButton->initWithTexture( aTexture );
//    pButton->autorelease();
    pButton->normalTexture = aTexture;
//    aTexture->retain();
    
    CCLOG("originalScale set!");
    
    CCLOG("originalScale set complete!");
    
    return pButton;
}

void LegendDaryButton::removeUsedAssets(){
    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
}
/*
LegendDaryButton* LegendDaryButton::buttonWithTexture(Texture2D* aTexture, Texture2D* leftDown, Texture2D* rightDown, int buttonType)
{
    LegendDaryButton* pButton = new LegendDaryButton();
    pButton->enabled = true;
    pButton->setButtonType(buttonType);
    pButton->initWithTexture( aTexture );
    pButton->autorelease();
    pButton->leftDownTexture = leftDown;
    leftDown->retain();
    pButton->rightDownTexture = rightDown;
    rightDown->retain();
    pButton->normalTexture = aTexture;
    aTexture->retain();
    return pButton;
}

LegendDaryButton* LegendDaryButton::buttonWithTexture(Texture2D* aTexture, Texture2D* down, int buttonType)
{
    LegendDaryButton* pButton = new LegendDaryButton();
    pButton->enabled = true;
    pButton->setButtonType(buttonType);
    pButton->initWithTexture( aTexture );
    pButton->autorelease();
    pButton->downTexture = down;
    down->retain();
    pButton->normalTexture = aTexture;
    aTexture->retain();
    return pButton;
}
*/
bool LegendDaryButton::initWithTexture(Texture2D* aTexture)
{
    if( Sprite::initWithTexture(aTexture) ) 
    {
        m_state = kButtonStateUngrabbed;
    }
    
    return true;
}

void LegendDaryButton::setButtonType(int buttonType)
{
    btnType = buttonType;
}

/*void LegendDaryButton::onEnter()
{
//    Director* pDirector = Director::getInstance();
//    pDirector->getEventDispatcher()->addTargetedDelegate(this, 0, true);
//    Sprite::onEnter();
}

void LegendDaryButton::onExit()
{
//    Director* pDirector = Director::getInstance();
//    pDirector->getEventDispatcher()->removeDelegate(this);
//    Sprite::onExit();
}*/

bool LegendDaryButton::containsTouchLocation(Touch* touch)
{
    return rect().containsPoint(convertTouchToNodeSpaceAR(touch));
}

bool LegendDaryButton::onTouchBegan(Touch *touch, Event *unused_event)
{
    if (!enabled) {
        return false;
    }
    //if (m_state != kButtonStateUngrabbed) return false;
//    if ( !containsTouchLocation(touch) ) return false;
    
    m_state = kButtonStateGrabbed;
   
    
    if (btnType == BUTTON_LEFT_RIGHT) {
        Vec2 touchPoint = touch->getLocation();
        
        //setPosition( Vec2(touchPoint.x, getPosition().y) );
//        cocos2d::Rect first = cocos2d::Rect(getPosition().x - getContentSize().width/2,
//                                  0,//getPosition().y - getContentSize().height/2,
//                                  getContentSize().width/2,
//                                  size.height);//getContentSize().height);
                          
        
//        cocos2d::Rect second = cocos2d::Rect(getPosition().x,// - getContentSize().width/2,
//                                   0, //getPosition().y - getContentSize().height/2,
//                                   getContentSize().width/2,
//                                   size.height);//getContentSize().height);
        if (touchPoint.x < getPosition().x){//first.containsPoint(touchPoint)) {
            GameManager::getInstance()->buttonDown(BUTTON_LEFT);
//            this->setTexture(leftDownTexture);
            this->setRotationSkewY(-3);
        }else{// if (second.containsPoint(touchPoint)) {
            GameManager::getInstance()->buttonDown(BUTTON_RIGHT);
//            this->setTexture(rightDownTexture);
            this->setRotationSkewY(3);
        }
    }else{
        GameManager::getInstance()->buttonDown(btnType);
//        this->setTexture(downTexture);
        this->setScale(originalScale*1.1f);
    }
    GameManager::getInstance()->getWorld()->isTouchStarted = false;
    
    return true;
}

void LegendDaryButton::onTouchMoved(Touch *touch, Event *unused_event)
{
    if (!enabled) {
        return;
    }

    if (btnType == BUTTON_LEFT_RIGHT) {
        Vec2 touchPoint = touch->getLocation();
        
        //setPosition( Vec2(touchPoint.x, getPosition().y) );
        cocos2d::Rect first = cocos2d::Rect(getPosition().x - getContentSize().width/2,
                                  getPosition().y - getContentSize().height/2,
                                  getContentSize().width/2,
                                  getContentSize().height);
        
        cocos2d::Rect second = cocos2d::Rect(getPosition().x,// - getContentSize().width/2,
                                   getPosition().y - getContentSize().height/2,
                                   getContentSize().width/2,
                                   getContentSize().height);
        if (touchPoint.x >= first.origin.x && touchPoint.x < first.getMaxX()) {
//            this->setTexture(leftDownTexture);
            this->setRotationSkewY(-3);
            GameManager::getInstance()->buttonDown(BUTTON_LEFT);
            GameManager::getInstance()->buttonUp(BUTTON_RIGHT);
        }else if (touchPoint.x >= second.origin.x && touchPoint.x < second.getMaxX()) {
//            this->setTexture(rightDownTexture);
            this->setRotationSkewY(3);
            GameManager::getInstance()->buttonDown(BUTTON_RIGHT);
            GameManager::getInstance()->buttonUp(BUTTON_LEFT);
        }
    }
}


void LegendDaryButton::onTouchEnded(Touch *touch, Event *unused_event)
{
    if (!enabled) {
        return;
    }
//    CCAssert(m_state == kButtonStateGrabbed, "Button - Unexpected state!");
    
    m_state = kButtonStateUngrabbed;
    
    if (btnType == BUTTON_LEFT_RIGHT) {
        GameManager::getInstance()->buttonUp(BUTTON_LEFT);
        GameManager::getInstance()->buttonUp(BUTTON_RIGHT);
    }else{
        GameManager::getInstance()->buttonUp(btnType);
    }
//    CCLOG("originalScale: %f", originalScale);
    this->setRotationSkewY(0);
    this->setScale(originalScale);
    this->setTexture(normalTexture);
} 

void LegendDaryButton::touchDelegateRetain()
{
    this->retain();
}

void LegendDaryButton::touchDelegateRelease()
{
    this->release();
}

void LegendDaryButton::enableButton(bool enable)
{
    
    m_state = kButtonStateUngrabbed;
    
    if (btnType == BUTTON_LEFT_RIGHT) {
        GameManager::getInstance()->buttonUp(BUTTON_LEFT);
        GameManager::getInstance()->buttonUp(BUTTON_RIGHT);
    }else{
        GameManager::getInstance()->buttonUp(btnType);
    }
    
    this->setTexture(normalTexture);
    this->enabled = enable;
}
