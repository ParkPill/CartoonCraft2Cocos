//
//  Inventory.cpp
//  SantaMountain-mobile
//
//  Created by PACKSUNG PILL on 03/02/2018.
//

#include "Inventory.h"
#include "GameManager.h"
//Inventory::Inventory(){
//    for(int i = 0; i < 54; i++){
//        slotArray[i] = new Slot();
//    }
//}
bool Inventory::isSlotAvailableForThisItem(std::string itemName, int type, int count){
    int nextSlot = getOccupiedSlotCount();
    
    if(type == INVENTORY_ITEM_TYPE_ETC){
        int accumulatedAvailableCount = 0;
        for(int i = 0; i < getMaxSlotCount(); i++){
            if(slotArray[i].count == 0){
                accumulatedAvailableCount += MAX_ETC_COUNT_IN_A_SLOT;
            }else if(itemName.compare(slotArray[i].name) == 0){
                accumulatedAvailableCount += (MAX_ETC_COUNT_IN_A_SLOT - slotArray[i].count);
            }
            if(accumulatedAvailableCount >= count){
                break;
            }
        }
        return accumulatedAvailableCount >= count;
    }else{
        return nextSlot + count <= getMaxSlotCount();
    }
}
int Inventory::getMaxSlotCount(){
    if(slotCount < 0){
        slotCount = 36;
    }
    return slotCount;
}
int Inventory::getTotalItemCount(std::string name){
    int accumulatedCount = 0;
    for(int i = 0; i < getMaxSlotCount(); i++){
        if(slotArray[i].count == 0){
            break;
        }
        if(name.compare(slotArray[i].name) == 0){
            accumulatedCount += slotArray[i].count;
        }
    }
    return accumulatedCount;
}
int Inventory::getOccupiedSlotCount(){
    for(int i = 0; i < getMaxSlotCount(); i++){
        if(slotArray[i].count == 0){
            return i;
        }
    }
    return 0;
}
int Inventory::getItemCount(int slot){
    return slotArray[slot].count;
}
void Inventory::addItem(std::string name, int type){
    addItem(name, type, 1);
}
void Inventory::addItem(std::string name, int type, int count){
    int nextSlot = getOccupiedSlotCount();
    if (type == INVENTORY_ITEM_TYPE_ETC) {
        for (int i = 0; i < nextSlot; i++) {
            if(name.compare(getItemName(0)) == 0){
                int countInSlot = getItemCount(i);
                if (countInSlot < MAX_ETC_COUNT_IN_A_SLOT) {
                    if (countInSlot + count <= MAX_ETC_COUNT_IN_A_SLOT) {
                        countInSlot += count;
                        count = 0;
                        slotArray[i].count = countInSlot;
                    }else{
                        count -= (MAX_ETC_COUNT_IN_A_SLOT - countInSlot);
                        slotArray[i].count = MAX_ETC_COUNT_IN_A_SLOT;
                    }
                }
            }
        }
        
        // add left over
        while(count > 0){
            if(count > MAX_ETC_COUNT_IN_A_SLOT){
                slotArray[nextSlot].count = MAX_ETC_COUNT_IN_A_SLOT;
                count -= MAX_ETC_COUNT_IN_A_SLOT;
            }else{
                slotArray[nextSlot].count = count;
                count = 0;
            }
            slotArray[nextSlot].name = (char*)name.c_str();
            slotArray[nextSlot].type = type;
            nextSlot++;
        }
    }else{
        while(count > 0){
            slotArray[nextSlot].name = (char*)name.c_str();
            slotArray[nextSlot].type = type;
            slotArray[nextSlot].count = 1;
            nextSlot++;
            count--;
        }
    }
}
std::string Inventory::getItemName(int slot){
    return slotArray[slot].name;
}

void Inventory::removeItem(int slot){
    removeItem(slot, 1);
}
void Inventory::removeItem(int slot, int count){
    slotArray[slot].count -= count;
    arrange();
}
void Inventory::removeItem(std::string itemName){
    removeItem(itemName, 1);
}
void Inventory::removeItem(std::string itemName, int count){
    for(int i = 0; i < getMaxSlotCount(); i++){
        if(itemName.compare(slotArray[i].name) == 0){
            if(slotArray[i].count >= count){
                slotArray[i].count -= count;
                count = 0;
                break;
            }else{
                count -= slotArray[i].count;
                slotArray[i].count = 0;
            }
        }
    }
    arrange();
}
void Inventory::arrange(){
    for(int i = 0; i < getMaxSlotCount(); i++){
        if (slotArray[i].count <= 0) {
            bool isSomethingThereAfterZero = false;
            int index = -1;
            for (int j = i+1; j < getMaxSlotCount(); j++) {
                if(slotArray[j].count > 0){
                    isSomethingThereAfterZero = true;
                    index = j;
                    break;
                }
            }
            slotArray[i].name = slotArray[index].name;
            slotArray[i].count = slotArray[index].count;
            slotArray[index].count = 0;
        }
    }
}
void Inventory::printItems(){
    log("slot length: %d", getMaxSlotCount());
    for(int i = 0; i < getMaxSlotCount(); i++){
        if(slotArray[i].count == 0){
            continue;
        }
        log("%d.Type:%d, Name: %s, Count: %d", i, slotArray[i].type, slotArray[i].name.c_str(), slotArray[i].count);
    }
}

std::string Inventory::loadData(){
    std::string strData = UDGetStr("inventory_data");
    ValueVector rows = GM->split(strData, ",");
    for (int i = 0; i< rows.size(); i+=3) {
        slotArray[i/3].type = rows[i].asInt();
        slotArray[i/3].name = (char*)rows[i+1].asString().c_str();
        slotArray[i/3].count = rows[i+2].asInt();
    }
    printItems();
    return strData;
}
void Inventory::saveData(){
    std::string strData;
    for(int i = 0; i < getMaxSlotCount(); i++){
        if(slotArray[i].count == 0){
            break;
        }
        strData.append(strmake("%d,%s,%d,", slotArray[i].type, slotArray[i].name.c_str(), slotArray[i].count));
    }
    if(strData.size() > 0){
        strData.erase(strData.end() - 1);
    }
    
    log("save data: %s", strData.c_str());
    UDSetStr("inventory_data", strData.c_str());
}
std::string Inventory::getData(){
    saveData();
    return loadData();
}
void Inventory::reset(){
    for(int i = 0; i < getMaxSlotCount(); i++){
        slotArray[i].count = 0;
    }
    saveData();
}
