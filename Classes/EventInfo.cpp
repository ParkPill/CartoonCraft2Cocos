//
//  EventInfo.cpp
//  CartoonCraft
//
//  Created by PACKSUNG PILL on 26/04/2018.
//

#include "EventInfo.h"

EventInfo::EventInfo(){
    
}

EventInfo::EventInfo(int ifSubject, int did, int object, int thenSubject, int does, int objects){
    conditionSubject = ifSubject;
    conditionSubjectAction = did;
    conditionSubjectActionObject = object;
    actionSubject = thenSubject;
    actionSubjectAction = does;
    actionSubjectActionObject = objects;
}
