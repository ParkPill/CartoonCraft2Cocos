//
//  Fog.cpp
//  CatoonCraft
//
//  Created by PACKSUNG PILL on 24/02/2018.
//

#include "Fog.h"
Fog* Fog::create(){
    Fog* drop = new Fog();
    drop->initWithSpriteFrameName("fog_101.png");
    drop->setScale(2);
    return drop;
}
