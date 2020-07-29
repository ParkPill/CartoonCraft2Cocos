//
//  UrlSprite.h
//  LegendDaryKakao
//
//  Created by SUNG PILL PACK on 14. 2. 13..
//
//

#ifndef __LegendDaryKakao__UrlSprite__
#define __LegendDaryKakao__UrlSprite__

#include <iostream>
#include "cocos2d.h"
//#include "cocos-ext.h"
#include <queue>
#include <pthread.h>
#include <errno.h>
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"
#include "network/HttpClient.h"

using namespace cocos2d;
//using namespace cocos2d::extension;
using namespace cocos2d::network;

class CCUrlSprite : public cocos2d::Sprite
{
public:
    bool isReady = false;
    std::string imgDir;
    float timeOutTime = 30;
    void checkTimeOut(float dt);
    void startTimeOutCheck();
    static CCUrlSprite *create(const char *pUrl);
    cocos2d::Size imgSize;
    Sprite* urlImage;
    
    bool isFailed =false;
    void* ThreadFunction(void* arg);
    void CreateThread();
    void startDownload(const char* pUrl);
    void onRequestImgCompleted(HttpClient *sender, HttpResponse *response);
};

#endif /* defined(__LegendDaryKakao__UrlSprite__) */
