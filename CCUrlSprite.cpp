//
//  UrlSprite.cpp
//  LegendDaryKakao
//
//  Created by SUNG PILL PACK on 14. 2. 13..
//
//

#include "CCUrlSprite.h"
//#include "curl/curl.h"
//#include "cocos-ext.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"
#include "network/HttpClient.h"

USING_NS_CC;
using namespace cocos2d::network;
/* A cURL write function for writing to file.
 * http://curl.haxx.se/libcurl/c/curl_easy_setopt.html#CURLOPTWRITEFUNCTION
 */
//size_t curl_write(void *ptr, size_t size, size_t nmemb, FILE *stream) {
//    return fwrite(ptr, size, nmemb, stream);
//}
static int imgCount = 0;
CCUrlSprite *CCUrlSprite::create(const char *pUrl) {
    CCUrlSprite* sprite = new CCUrlSprite();
//    sprite->initWithFile("photoFrame.png");
    sprite->init();
    sprite->urlImage = NULL;
    sprite->startDownload(pUrl);
    sprite->startTimeOutCheck();
    return sprite;
    /*
    std::string dir = CCFileUtils::sharedFileUtils()->getWritablePath();
    dir.append(CCString::createWithFormat("profile_%d.png", imgCount)->getCString());
    imgCount++;
    
    CURL *curl = curl_easy_init();
    if(curl) {
        // Open up writable temp graphic.
        FILE *file = fopen(dir.c_str(), "wb");
        
        // Generate URL.
        char url[300];
        sprintf(url, "%s", pUrl);
        CCLog("ccurl purl: %s, dir: %s", pUrl, dir.c_str());
        
        // Allow follow location for "302 File Found" redirect.
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        
        CCLog("11");
        // Write the image to the file.
        curl_easy_perform(curl);
        
        CCLog("2");
        fclose(file);
        CCLog("3");
    }
    
    curl_easy_cleanup(curl);
    
    CCLog("4");
    // Create and return an auto-released CCUrlSprite.
    CCUrlSprite *pSpriteProfile = new CCUrlSprite();
    if (pSpriteProfile && pSpriteProfile->initWithFile(dir.c_str())) {
        CCLog("5");
        pSpriteProfile->autorelease();
        return pSpriteProfile;
    }
    CCLog("6");
    
    // Otherwise return NULL on fail.
    CC_SAFE_DELETE(pSpriteProfile);
    
    return NULL;
     */
}

void CCUrlSprite::startTimeOutCheck(){
    this->schedule(schedule_selector(CCUrlSprite::checkTimeOut), 0.1);
}
void CCUrlSprite::checkTimeOut(float dt){
//    log("checking time out;");
    timeOutTime -= dt;
    if (isFailed || isReady) {
        this->unschedule(schedule_selector(CCUrlSprite::checkTimeOut));
    }
    if (timeOutTime <= 0) {
        MessageBox("time out", "TIMEOUT");
        this->unschedule(schedule_selector(CCUrlSprite::checkTimeOut));
        isFailed = true;
    }
}
void CCUrlSprite::startDownload(const char* pUrl){
    log("onHttpRequest In the request");
    HttpRequest* request = new (std::nothrow) HttpRequest();
    request->setUrl(pUrl);
    request->setRequestType(HttpRequest::Type::POST);
    request->setResponseCallback(CC_CALLBACK_2(CCUrlSprite::onRequestImgCompleted, this));
    request->setTag("CCUrlSprite-tag");
    HttpClient::getInstance()->send(request);
//    MessageBox(pUrl, "Requesting");
    request->release();
}

void CCUrlSprite::onRequestImgCompleted(HttpClient *sender, HttpResponse *response)
{
    log("AppDelegate::onHttpRequestCompleted - onHttpRequestCompleted BEGIN");
    if (isFailed) {
        return;
    }
//    MessageBox("Respond", "onRequestImgCompleted");
    if (!response)
    {
        log("onHttpRequestCompleted - No Response");
        MessageBox("onHttpRequestCompleted - No Response", "Ads loading Failed");
        isFailed = true;
        return;
    }
    
    log("onHttpRequestCompleted - Response code: %lu", response->getResponseCode());
    
    if (!response->isSucceed())
    {
        log("onHttpRequestCompleted - Response failed");
        log("onHttpRequestCompleted - Error buffer: %s", response->getErrorBuffer());
        MessageBox(response->getErrorBuffer(), "Ads loading Failed");
        isFailed = true;
        return;
    }
    log("onHttpRequestCompleted - Response code: %s", response->getResponseDataString());
    
    
    std::vector<char> *buffer = response->getResponseData();
    const char* file_char = buffer->data();
    Image * image = new  Image ();
    image-> initWithImageData ( reinterpret_cast<const unsigned char*>(&(buffer->front())), buffer->size());
    Texture2D * texture = new  Texture2D ();
    texture-> initWithImage (image);
    Sprite * sprite = Sprite :: createWithTexture (texture);
    sprite->setName("spt");
    imgSize = sprite->getContentSize();
//    sprite->setPosition(Vec2(visibleSize.width/2, visibleSize.height / 2));
    addChild(sprite);
    isReady = true;
    log("onHttpRequestCompleted height %f", sprite->getContentSize().height);
}
