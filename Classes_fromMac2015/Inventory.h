//
//  Inventory.hpp
//  SantaMountain-mobile
//
//  Created by PACKSUNG PILL on 03/02/2018.
//

#ifndef Inventory_hpp
#define Inventory_hpp
#include <string>

#define INVENTORY_ITEM_TYPE_NONE -1
#define INVENTORY_ITEM_TYPE_EQUIPMENT 0
#define INVENTORY_ITEM_TYPE_ETC 1
#define MAX_ETC_COUNT_IN_A_SLOT 50

struct Slot
{
    int type=INVENTORY_ITEM_TYPE_NONE;
    std::string name;
    int count=0;
};
class Inventory
{
private:
    Slot slotArray[54];
    int slotCount = -1;
public:
//    Inventory();
    std::string loadData();
    void saveData();
    std::string getData();
    bool isSlotAvailableForThisItem(std::string itemName, int type, int count);
    int getMaxSlotCount();
    int getTotalItemCount(std::string name);
    int getOccupiedSlotCount();
    int getItemCount(int slot);
    void addItem(std::string name, int type);
    void addItem(std::string name, int type, int count);
    std::string getItemName(int slot);
    
    void removeItem(int slot);
    void removeItem(int slot, int count);
    void removeItem(std::string itemName);
    void removeItem(std::string itemName, int count);
    void arrange();
    void printItems();
    void reset();
};

#endif /* Inventory_hpp */
