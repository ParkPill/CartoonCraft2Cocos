//
//  UploadedMaps.hpp
//  CartoonCraftNV
//
//  Created by SUNG PILL PACK on 2021/02/01.
//

#ifndef UploadedMaps_hpp
#define UploadedMaps_hpp


#include <stdio.h>
#include "PPLabel.h"
#include "ui/UIButton.h"
#include "ui/UIScrollView.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "TopBar.h"
#include "PageBase.h"
#include "NetworkWebSocket.h"

using namespace cocos2d;
using namespace cocos2d::ui;
class UploadedMaps : public PageBase
{
private:
    cocos2d::Size size;
    std::string strMapList;
public:
    
    
    virtual bool init();
    CREATE_FUNC(UploadedMaps);
    
    
    void oneSecUpdate(float dt);
    
    void loadRecent();
    void loadByTrohpy();
    void loadLike();
    
    void updateMapList();
    void onPlayClick(Ref* ref);
    void onDeleteClick(Ref* ref);
};


#endif /* UploadedMaps_hpp */
