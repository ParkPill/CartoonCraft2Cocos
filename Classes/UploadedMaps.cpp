//
//  UploadedMaps.cpp
//  CartoonCraftNV
//
//  Created by SUNG PILL PACK on 2021/02/01.
//

#include "UploadedMaps.h"
#include "Intro.h"
#include "LanguageManager.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "BuggyServerManager.h"
#include <editor-support/spine/spine-cocos2dx.h>
#include <editor-support/spine/SkeletonAnimation.h>
//#include "CrossPromotion.h"
//#include "NativeInterface.h"
#include "ShopLayer.h"
#include "HeroPage.h"
#include "EventPopup.h"
#include "MultiplayLobby.h"
#include "ChatRoom.h"
bool UploadedMaps::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !PageBase::init())
    {
        return false;
    }
//    Layer* colorLayer = LayerColor::create(Color4B(235, 229, 210, 255));
//    this->addChild(colorLayer, -1);
    size = Director::getInstance()->getWinSize();
    
    Node* layer = CSLoader::createNode("UploadedMap.csb");
    this->addChild(layer);
    layer->setName("uploadedMap");
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    
    
    Text* lbl = (Text*)layer->getChildByName("lblMapOfWeek");
    LM->setLocalizedString(lbl, "map of week");
    
    Button* btn = (Button*)layer->getChildByName("btnRecent");
    btn->addClickEventListener(CC_CALLBACK_0(UploadedMaps::loadRecent, this));
    
    ImageView* imgWeek = (ImageView*)layer->getChildByName("imgWeek");
    btn = (Button*)imgWeek->getChildByName("btnDelete");
    btn->addClickEventListener(CC_CALLBACK_1(UploadedMaps::onDeleteClick, this));
    btn->setVisible(false);
    
    btn = (Button*)imgWeek->getChildByName("btnPlay");
    btn->addClickEventListener(CC_CALLBACK_1(UploadedMaps::onPlayClick, this));
    
    btn = (Button*)layer->getChildByName("btnHeart");
    btn->addClickEventListener(CC_CALLBACK_0(UploadedMaps::loadLike, this));
    ImageView* img = (ImageView*)btn->getChildByName("imgIcon");
    img->loadTexture("eventItem4.png");
    
    btn = (Button*)layer->getChildByName("btnTrophy");
    btn->addClickEventListener(CC_CALLBACK_0(UploadedMaps::loadByTrohpy, this));
    
    btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(UploadedMaps::closeThis, this));
    btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(UploadedMaps::closeThis, this));

    this->schedule(schedule_selector(UploadedMaps::oneSecUpdate), 1);
    
    loadRecent();
    return true;
}


void UploadedMaps::oneSecUpdate(float dt){
    if(BSM->isServerFailed || BSM->isOffline){
        BSM->isServerFailed = false;
        BSM->isOffline = false;
        hideIndicator();
    }
    if (BSM->downloadMapListState >= 0) {
        hideIndicator();
        if (BSM->downloadMapListState == 2) {
            log("map data load success: %s", BSM->mapList.c_str());
            strMapList = BSM->mapList;
            updateMapList();
        }
        BSM->downloadMapListState = -1;
        
//        BSM->resetMapLike(); // test 
    }
    if (BSM->downloadMapState >= 0) {
        hideIndicator();
        if (BSM->downloadMapState == 2) {
            log("map  download success: %s", BSM->mapList.c_str());
            GM->loadMapData = BSM->mapData;
            GM->nextScene = STAGE_FIELD;
            auto scene = GameScene::scene(STAGE_CUSTOM, false);
            Director::getInstance()->replaceScene(scene);
        }
        BSM->downloadMapState = -1;
    }
    
}

void UploadedMaps::loadRecent(){
    showIndicator();
    BSM->downloadMapList(0);
}
void UploadedMaps::loadByTrohpy(){
    showIndicator();
    BSM->downloadMapList(1);
}
void UploadedMaps::loadLike(){
    showIndicator();
    BSM->downloadMapList(2);
}

void UploadedMaps::updateMapList(){
    Node* layer = this->getChildByName("uploadedMap");
    ImageView* imgWeek = (ImageView*)layer->getChildByName("imgWeek");
    ImageView* img = (ImageView*)imgWeek->getChildByName("imgHeart");
    img->loadTexture("eventItem4.png");
    ScrollView* sv = (ScrollView*)layer->getChildByName("scrollView");
    sv->removeAllChildren();
    Button* btn = (Button*)imgWeek->getChildByName("btnPlay");
    btn->setEnabled(false);
    btn = (Button*)imgWeek->getChildByName("btnDelete");
//    btn->setEnabled(false);
    btn->setVisible(false);
    int gapY = 5;
    int nextY = 0;
    Text* lbl = (Text*)imgWeek->getChildByName("lblName");
    float fontSize = lbl->getFontSize();
    ValueVector mapDataList = GM->split(strMapList, ",");
    int totalHeight = (imgWeek->getContentSize().height + gapY)*mapDataList.size();
//    mapdata += map[i]._id + "_" + map[i].mapname + "_" + map[i].like + "_" + map[i].success + "_" + map[i].fail + ",";
    int indexOfWeek = -1;
    int like = 0;
    for (int i = 0; i < mapDataList.size(); i++) {
        ValueVector dataDetail = GM->split(mapDataList[i].asString(), "_");
        log("map: %s", mapDataList[i].asString().c_str());
        Widget* wdg = imgWeek->clone();
        
        sv->addChild(wdg);
        nextY = totalHeight - (wdg->getContentSize().height + gapY)*i - wdg->getContentSize().height/2;
        wdg->setPosition(Vec2(sv->getContentSize().width/2, nextY));
//        totalHeight = nextY + wdg->getContentSize().height + gapY;
        
        btn = (Button*)wdg->getChildByName("btnPlay");
        btn->setEnabled(true);
        btn->setTag(i);
        
        btn = (Button*)wdg->getChildByName("btnDelete");
//        btn->setEnabled(false); // test
//        btn->setVisible(true);// test
        btn->setTag(i);
        
        lbl = (Text*)wdg->getChildByName("lblName");
        lbl->setFontName("arial");
        lbl->setString(dataDetail[1].asString());
        lbl->setFontSize(fontSize);
        
        lbl = (Text*)wdg->getChildByName("lblUploader");
        lbl->setFontName("arial");
        lbl->setString(dataDetail[6].asString());
        lbl->setFontSize(fontSize);
        
        lbl = (Text*)wdg->getChildByName("lblHeart");
        lbl->setString(dataDetail[2].asString());
        lbl = (Text*)wdg->getChildByName("lblTrophy");
        int success = dataDetail[3].asInt();
        lbl->setString(Value(success).asString());
//        int fail = dataDetail[4].asInt();
//        if (success == 0 && fail == 0) {
//            lbl->setString("-");
//        }else{
//            int successRate = success*100/(success + fail);
//            lbl->setString(Value(successRate).asString());
//        }
        
        if (like < dataDetail[2].asInt()) {
            indexOfWeek = i;
            like = dataDetail[2].asInt();
            btn = (Button*)imgWeek->getChildByName("btnPlay");
            btn->setTag(i);
            btn->setEnabled(true);
            lbl = (Text*)imgWeek->getChildByName("lblName");
            lbl->setFontName("arial");
            lbl->setString(dataDetail[1].asString());
            lbl->setFontSize(fontSize);
            
            lbl = (Text*)imgWeek->getChildByName("lblUploader");
            lbl->setFontName("arial");
            lbl->setString(dataDetail[6].asString());
            lbl->setFontSize(fontSize);
            
            lbl = (Text*)imgWeek->getChildByName("lblHeart");
            lbl->setString(dataDetail[2].asString());
            
            lbl = (Text*)imgWeek->getChildByName("lblTrophy");
            int success = dataDetail[3].asInt();
            lbl->setString(Value(success).asString());
//            int fail = dataDetail[4].asInt();
//            if (success == 0 && fail == 0) {
//                lbl->setString("-");
//            }else{
//                int successRate = success*100/(success + fail);
//                lbl->setString(Value(successRate).asString());
//            }
        }
    }
    
    sv->setInnerContainerSize(Size(sv->getContentSize().width, totalHeight));
}
void UploadedMaps::onPlayClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    int index = btn->getTag();
    
    ValueVector mapDataList = GM->split(strMapList, ",");
//    mapdata += map[i]._id + "_" + map[i].mapname + "_" + map[i].like + "_" + map[i].success + "_" + map[i].fail + ",";
    ValueVector dataDetail = GM->split(mapDataList[index].asString(), "_");
    GM->playedMapID = dataDetail[0].asString();
    BSM->downloadMap(dataDetail[0].asString());
    showIndicator();
}
void UploadedMaps::onDeleteClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    int index = btn->getTag();
    
    ValueVector mapDataList = GM->split(strMapList, ",");
//    mapdata += map[i]._id + "_" + map[i].mapname + "_" + map[i].like + "_" + map[i].success + "_" + map[i].fail + ",";
    ValueVector dataDetail = GM->split(mapDataList[index].asString(), "_");
    BSM->deleteMap(dataDetail[0].asString());
}

