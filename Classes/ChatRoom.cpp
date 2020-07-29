//
//  ChatRoom.cpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 2020/05/20.
//

#include "ChatRoom.h"

#include "ui/UIButton.h"
#include "ui/UIScrollView.h"
#include "GameManager.h"
#include "LanguageManager.h"
bool ChatRoom::init(){
    
    if ( !PageBase::init())
    {
        return false;
    }
    
    // connect
//    _webSocket = NetworkWebSocket::create("localhost:8104");
    _webSocket = NetworkWebSocket::create("http://222.120.115.95:8104");
    if (_webSocket->connect()) {
        log("connected");
        this->_webSocket->setOnConnectionOpenedCallBack(CC_CALLBACK_0(ChatRoom::onConnectionOpened, this));
        this->_webSocket->setOnMessageReceivedCallBack(CC_CALLBACK_1(ChatRoom::onMessageReceived, this));
        this->_webSocket->setOnConnectionClosedCallBack(CC_CALLBACK_0(ChatRoom::onConnectionClosed, this));
        this->_webSocket->setOnErrorOccurredCallBack(CC_CALLBACK_1(ChatRoom::onConnectionError, this));
    }else{
        log("connection failed");
    }
    // connect ends
    
    
    size = Director::getInstance()->getWinSize();
    Node* layer = CSLoader::createNode("Chat.csb");
    this->addChild(layer, 4);
    layer->setName("chatPopup");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(ChatRoom::quitChat, this));
    ImageView* img = (ImageView*)layer->getChildByName("btnBlock");
    img->addClickEventListener(CC_CALLBACK_0(ChatRoom::quitChat, this));
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "chat room");
    lbl = (Text*)layer->getChildByName("lblWarning");
    LM->setLocalizedString(lbl, "char warning");
    
    lvLines = (ListView*)layer->getChildByName("lvLines");
    lvLines->setClippingEnabled(true);
    lblTemp = (Text*)layer->getChildByName("lblTemp");
    lblTemp->setVisible(false);
    tfLine = (TextField*)layer->getChildByName("tfLine");
    tfLine->setString("");
    
    btn = (Button*)layer->getChildByName("btnSend");
    btn->addClickEventListener(CC_CALLBACK_0(ChatRoom::onSendClick, this));
    
    AddLine("Connecting...");
    
    this->schedule(schedule_selector(ChatRoom::update), 0.1f);
    // init done
    return true;
}
void ChatRoom::update(float dt){
    if(isMsgArrived){
        isMsgArrived = false;
//        AddLine(receivedMsg);
        for (int i = 0; i < receivedMsgList.size(); i++) {
            AddLine(receivedMsgList.at(i));
        }
        receivedMsgList.clear();
    }
}
void ChatRoom::onSendClick(){
    string str = tfLine->getString();
    
    int msgCode = 1;
    LanguageType lType = LM->getLanguageType();
    std::string gameName = "cc";
    std::string msg = str;
    _webSocket->send(StringUtils::format("%d%d%s_%s", msgCode, lType, gameName.c_str(), msg.c_str()));
    tfLine->setString("");
}
void ChatRoom::AddLine(std::string str){
    std::string strName = UDGetStr(KEY_NAME);
    
    Text* lbl = (Text*)lblTemp->clone();
    if(str.find(strName) != std::string::npos){
        lbl->setTextColor(Color4B(150, 163, 83, 255));
    }
    lbl->setVisible(true);
    lvLines->pushBackCustomItem(lbl);
    LM->setLocalizedString(lbl, str);
    if(lvLines->getChildren().size() >50){
        lvLines->removeItem(0);
    }
}
void ChatRoom::onConnectionOpened()
{
    log("INFO : Connection opened");
    int msgCode = 0;
    LanguageType lType = LM->getLanguageType();
    std::string gameName = "cc";
    std::string msg = UDGetStr(KEY_NAME);
    _webSocket->send(StringUtils::format("%d%d%s_%s", msgCode, lType, gameName.c_str(), msg.c_str()));
}

void ChatRoom::onMessageReceived(string message)
{
    log("Receive : %s", message.c_str());
    if(message.substr(0, 1).compare("0") == 0){
        log("join");
        string str = LM->getText("chat join format");
        str = str.replace(str.find("FORMAT"), 6, "%s");
        
        receivedMsg = StringUtils::format(str.c_str(), message.substr(1).c_str());
        receivedMsgList.push_back(receivedMsg);
        isMsgArrived = true;
    }else if(message.substr(0, 1).compare("1") == 0){
        log("msg");
        receivedMsg = message.substr(1);
        receivedMsgList.push_back(receivedMsg);
        isMsgArrived = true;
    }else if(message.substr(0, 1).compare("2") == 0){
        string str = LM->getText("chat quit format");
        str = str.replace(str.find("FORMAT"), 6, "%s");
        
        receivedMsg = StringUtils::format(str.c_str(), message.substr(1).c_str());
        receivedMsgList.push_back(receivedMsg);
        isMsgArrived = true;
        log("quit");
    }
}

void ChatRoom::onConnectionClosed()
{
    log("INFO : Connection closed");

}
void ChatRoom::onConnectionError(const cocos2d::network::WebSocket::ErrorCode & error)
{
    string errorCodeValue = "";
    switch (error)
    {
    case cocos2d::network::WebSocket::ErrorCode::CONNECTION_FAILURE:
        errorCodeValue = "CONNECTION_FAILURE";
        break;
    case cocos2d::network::WebSocket::ErrorCode::TIME_OUT:
        errorCodeValue = "TIME_OUT";
        break;
    case cocos2d::network::WebSocket::ErrorCode::UNKNOWN:
        errorCodeValue = "UNKNOWN";
        break;
    }
    log("chat room error: %s", errorCodeValue.c_str());
    AddLine(LM->getText("connection failed"));
}
void ChatRoom::quitChat(){
    int msgCode = 2;
    LanguageType lType = LM->getLanguageType();
    std::string gameName = "cc";
    std::string msg = UDGetStr(KEY_NAME);
    _webSocket->send(StringUtils::format("%d%d%s_%s", msgCode, lType, gameName.c_str(), msg.c_str()));
    _webSocket->close();
    closeThis();
}
