//
//  BattleLobby.cpp
//  CartoonCraftNV-mobile
//
//  Created by PACKSUNG PILL on 27/12/2018.
//

#include "BattleLobby.h"
bool BattleLobby::init()
{
    if ( !Layer::init()){
        return false;
    }
    
    Node* layer = CSLoader::createNode("Battle.csb");
    this->addChild(layer);
    
    
    
    return true;
}

void BattleLobby::closePopup(){
    if(popupArray.size() > 0){
        Node* popup = popupArray.at(popupArray.size() - 1);
        popupArray.eraseObject(popup);
        popup->removeFromParent();
    }
}

bool BattleLobby::setAsPopup(Node* node){
    popupArray.pushBack(node);
    node->setLocalZOrder(200 + (int)popupArray.size());
    return true;
}
