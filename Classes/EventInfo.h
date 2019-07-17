//
//  EventInfo.hpp
//  CartoonCraft
//
//  Created by PACKSUNG PILL on 26/04/2018.
//

#ifndef EventInfo_hpp
#define EventInfo_hpp

#include <stdio.h>

#define SUBJECT_UNIT 0
#define SUBJECT_BUILDING 1
#define SUBJECT_TIME 2
#define SUBJECT_GOLD 3
#define SUBJECT_LUMBER 4
#define SUBJECT_ALL_ALLY 5
#define SUBJECT_ALL_ENEMY 6
#define SUBJECT_ALL_ENEMY_ON_MAP 7
#define SUBJECT_GAME 8

#define ACTION_MOVE 0
#define ACTION_DIE 1
#define ACTION_AFTER 2
#define ACTION_IS_GATHERED 3
#define ACTION_WIN 4
#define ACTION_OVER 5
#define ACTION_PRODUCE 6

#define OBJECT_UNIT 0
#define OBJECT_BUILDING 1
#define OBJECT_AMOUNT 2
#define OBJECT_SEC 3

#define PIA_HEROES 0
#define PIA_ALLY 1
#define PIA_ENEMY 2
#define PIA_TO_RESCUE 3

//using namespace cocos2d;
class EventInfo
{
private:
    
public:
    EventInfo();
    EventInfo(int ifSubject, int did, int object, int thenSubject, int does, int objects);
    int conditionSubject = SUBJECT_TIME;
    int conditionSubjectAction = ACTION_AFTER;
    int conditionSubjectActionObject = OBJECT_SEC;
    int actionSubject = SUBJECT_BUILDING;
    int actionSubjectAction = ACTION_PRODUCE;
    int actionSubjectActionObject = OBJECT_UNIT;
};


#endif /* EventInfo_hpp */
