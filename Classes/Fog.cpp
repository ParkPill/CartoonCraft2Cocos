//
//  Fog.cpp
//  CatoonCraft
//
//  Created by PACKSUNG PILL on 24/02/2018.
//

#include "Fog.h"
Fog* Fog::create(){
    Fog* drop = new Fog();
    drop->initWithSpriteFrameName("fog_201.png");
    return drop;
}
