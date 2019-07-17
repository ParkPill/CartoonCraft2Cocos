#ifndef __AWESOME_DIALOGBOX_SCENE_H__
#define __AWESOME_DIALOGBOX_SCENE_H__

#include "cocos2d.h"
using namespace cocos2d;
class AwesomeDialogBox : public cocos2d::Layer
{
private:
    std::vector<std::string> _talkVec;
	Label* _labelTop;
	Label* _blinkLabel;
	std::string _currentTalk;
	bool _skipToEnd;
	int _talkIndex;
	int _charIndex; 
	Layer* _callingLayer;
	FiniteTimeAction* _callback;
	Sprite* _leftSide;
	Sprite* _rightSide;
    DrawNode* _backgroundTop;
	bool _isShowing;
    Label* getLabel(const char* txt);
    cocos2d::Size size;
    
    bool isInitialized;
    int touchCount;
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    bool isWaiting;
    Sequence* callback;
    // implement the "static node()" method manually
    CREATE_FUNC(AwesomeDialogBox);
	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
	std::vector<std::string> split(const std::string &s, char delim);

    EventListenerTouchOneByOne* listener;
    void removeUsedAssets();
    
    bool onTouchBegan(Touch *touch, Event *unused_event);
    void onTouchMoved(Touch *touch, Event *unused_event);
    void onTouchEnded(Touch *touch, Event *unused_event);
	void update(float dt);
	void blink(float dt);
	void showNextTalk();
	void ShowDialog(const char* talks, Layer* layer, FiniteTimeAction* call);
};

#endif  // __AWESOME_DIALOGBOX_SCENE_H__