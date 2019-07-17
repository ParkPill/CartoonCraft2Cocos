//
//  Fog.hpp
//  CatoonCraft
//
//  Created by PACKSUNG PILL on 24/02/2018.
//

#ifndef Fog_hpp
#define Fog_hpp

#define FOG_SEEN_NOT 0
#define FOG_SEEN_NOT_NOW 1
#define FOG_SEEN_LITTLE 2
#define FOG_SEEN_NOW 3
#define FOG_SIZE 200


#include "cocos2d.h"
using namespace cocos2d;

class Fog : public cocos2d::Sprite
{
private:
    
public:
    Vector<Fog*> adjacentFogs;
    Vector<Fog*> farFogs;
    static Fog* create();
    cocos2d::Point coordinate;
    int newState = FOG_SEEN_NOT;
    int appliedState = FOG_SEEN_NOT;
};


#endif /* Fog_hpp */
