//
//  ChatRoom.hpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 2020/05/20.
//

#ifndef ChatRoom_hpp
#define ChatRoom_hpp

#include <stdio.h>

#include "NetworkWebSocket.h"
#include "PageBase.h"
#include "ui/UIText.h"
#include "ui/UIListView.h"
#include "ui/UITextField.h"
using namespace cocos2d;
using namespace cocos2d::ui;

class ChatRoom : public PageBase
{
private:
    cocos2d::Size size;
    NetworkWebSocket* _webSocket;
    Text* lblTemp;
    ListView* lvLines;
    TextField* tfLine;
    bool isMsgArrived = false;
    std::string receivedMsg;
    std::vector<std::string> receivedMsgList;
public:
    
    virtual bool init();
    CREATE_FUNC(ChatRoom);

    void onSendClick();

    void onConnectionOpened();
    void onMessageReceived(string message);
    void onConnectionClosed();
    void onConnectionError(const cocos2d::network::WebSocket::ErrorCode & error);

    void AddLine(std::string str);
    void quitChat();
    void update(float dt);
};
#endif /* ChatRoom_hpp */
