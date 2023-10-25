//
//  MultiplayLobby.hpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 06/12/2019.
//

#ifndef MultiplayLobby_hpp
#define MultiplayLobby_hpp

#include <stdio.h>
#include "PPLabel.h"
#include "ui/UIText.h"
#include "ui/UIButton.h"
#include "ui/UIScrollView.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "PageBase.h"
#include "NetworkWebSocket.h"



using namespace cocos2d;
using namespace cocos2d::ui;
class MultiplayLobby : public PageBase
{
private:
    cocos2d::Size size;
    TextField* tfRoomName;
    std::string waitingMsg = "-1";
    std::string userInfo;
    bool isInRoom = false;
    int heroRace = 0;
    int enemyRace = 0;
    float countDown = 3;
    bool isGameStarted = false;
    int dotCounter = 0;
    float oneSecTimer = 0;
    bool isFindingMatch = false;
    
public:
    virtual bool init();
    CREATE_FUNC(MultiplayLobby);
    
    void update(float dt);
    void onStartClick(Ref* ref);
    
    void onEnterRoomClick(Ref* ref);
    void showReadyRoom(std::string users);
    void updateEnemyInfo();
    void showEmoticon(int index, bool isHero);
    void updateUserInfo();
    
    void onSelectedRace(Ref* ref);
    void updateEnemyRace(int race);
    void onSelectedEmoticon(Ref* ref);
    void onCloseRoom();
    void updateUI();
    void InitUI();
    virtual void closeThis() override;
    
    void setHeroBackground(int level);
    void setEnemyBackground(int level);
    void resetEnemyInfo();
};


#endif /* MultiplayLobby_hpp */
