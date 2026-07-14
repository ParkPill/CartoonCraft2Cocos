#include "AStar.h"
#include <vector>
#include <deque>
#include <algorithm>
//#include "main.h"


using namespace cocos2d;
using namespace std;

int g_goalX;
int g_goalY;
// Mirrors AStar::admissibleHeuristic for the free-function comparator below
// (same pattern as g_goalX/g_goalY). Set per search in
// fullFillPlayerAndGoalData().
bool g_useSqrtHeuristic = false;

bool compareTwoCellsByDistance(Cell *c1, Cell *c2){
    if(c1->getDistance() <= c2->getDistance()){
        return false;
    }else{
        return true;
    }
}

bool comparebyWhichNearerGoalSimpleWay(Cell *c1, Cell *c2){
    int distanceOfC1AndGoal = abs(g_goalX - c1->getX()) + abs(g_goalY - c1->getY());
    int distanceOfC2AndGoal = abs(g_goalX - c2->getX()) + abs(g_goalY - c2->getY());
    if(distanceOfC1AndGoal <= distanceOfC2AndGoal){
        return false;
    }else{
        return true;
    }
}

float distanceBetweenTwoCells(float c1X,float c1Y, float c2X, float c2Y){
    
//    return sqrt(pow(c2X - c1X,2) + pow(c2Y - c1Y,2));
    return (c2X - c1X)*(c2X - c1X) + (c2Y - c1Y)*(c2Y - c1Y);
}

bool comparebyWhichNearerGoalPhysicWay(Cell *c1, Cell *c2){
    float distanceOfC1AndGoal = distanceBetweenTwoCells((float)c1->getX(),(float)c1->getY(),(float)g_goalX,(float) g_goalY);
    
    float distanceOfC2AndGoal = distanceBetweenTwoCells((float)c2->getX(),(float)c2->getY(),(float)g_goalX,(float) g_goalY);
    if(distanceOfC1AndGoal <= distanceOfC2AndGoal){
        return false;
    }else{
        return true;
    }
}

bool comparebyDistanceBetweenStartAndGoal(Cell *c1, Cell *c2){
    float h1 = distanceBetweenTwoCells((float)c1->getX(),(float)c1->getY(),(float)g_goalX,(float) g_goalY);
    float h2 = distanceBetweenTwoCells((float)c2->getX(),(float)c2->getY(),(float)g_goalX,(float) g_goalY);
    if(g_useSqrtHeuristic){
        // distanceBetweenTwoCells returns SQUARED distance, which swamps the
        // accumulated step cost (1 / 1.414 per tile) and turns the search
        // into a greedy beeline that hugs obstacle walls. Taking the root
        // makes the heuristic admissible (Euclidean <= real path cost), so
        // the search returns genuinely short routes. Opt-in per grid to keep
        // the ground A*'s long-standing behavior/performance untouched.
        h1 = sqrt(h1);
        h2 = sqrt(h2);
    }
    float distanceOfC1AndGoal = c1->getDistance() + h1;

    float distanceOfC2AndGoal = c2->getDistance() + h2;
    if(distanceOfC1AndGoal <= distanceOfC2AndGoal){
        return false;
    }else{
        return true;
    }
}

AStar::AStar():_m_Map(xLineCount,yLineCount),_pastTime(0),_startDrawProgress(false){
}
void AStar::setMap(int width, int height){
    _m_Map = Array2D<Cell>(width, height);
    xLineCount = width;
    yLineCount = height;
    initCells();
}
Scene* AStar::scene()
{
    Scene * scene = NULL;
    do
    {
        // 'scene' is an autorelease object
        scene = Scene::create();
        CC_BREAK_IF(! scene);
        
        // 'layer' is an autorelease object
        AStar *layer = AStar::create();
        CC_BREAK_IF(! layer);
        
        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool AStar::init()
{
    bool bRet = false;
    do
    {
        //////////////////////////////////////////////////////////////////////////
        // super init first
        //////////////////////////////////////////////////////////////////////////
        
        CC_BREAK_IF(! Layer::init());
        
        _screenSize = Director::getInstance()->getWinSize();
//        _eachBoxWidth = (_screenSize.width) / xLineCount;
//        _eachBoxHeight = (_screenSize.height) / yLineCount;
        
        initCells();
        
//        _lineContainer = LineContainer::create();
//        _lineContainer->updateMapParameter(_m_Map);
//        this->addChild(_lineContainer);
        
//        CCTexture2D *pPlayerTexture = CCTextureCache::sharedTextureCache()->addImage("start.png");
//        CCTexture2D *pGoalTexture = CCTextureCache::sharedTextureCache()->addImage("goal.png");
        
//        _player = DragSprite::spriteWithTexture(pPlayerTexture);
//        _player->setPosition(ccp(200,100));
//        this->addChild(_player,1);
//
//        _goal = DragSprite::spriteWithTexture(pGoalTexture);
//        _goal->setPosition(ccp(300,100));
//        this->addChild(_goal,1);
        
//        CCMenuItemFont *pBeginFindingByDistanceItem = CCMenuItemFont::create("Distance",
//                                                                             this,menu_selector(AStar::menuDistanceBasedFindingCallback));
//
//        CCMenuItemFont *pBeginFindingBySimpleHeuristicItem = CCMenuItemFont::create("Simple Heuristic",
//                                                                                    this,menu_selector(AStar::menuSimpleHeuristicBasedFindingCallback));
//
//        CCMenuItemFont *pBeginFindingByComplexHeuristicItem = CCMenuItemFont::create("Complex Heuristic",
//                                                                                     this,menu_selector(AStar::menuComplexHeuristicBasedFindingCallback));
//
//        CCMenuItemFont *pBeginFindingByAStarItem = CCMenuItemFont::create("A Star",
//                                                                          this,menu_selector(AStar::menuAStarFindingCallback));
//
//
//
//        // start finding menu
//        CCMenu* pMenu = CCMenu::create(pBeginFindingByDistanceItem,pBeginFindingBySimpleHeuristicItem,
//                                       pBeginFindingByComplexHeuristicItem,pBeginFindingByAStarItem,NULL);
//        pMenu->alignItemsHorizontallyWithPadding(20);
//        pMenu->setPosition(ccp(385,70));
//        CC_BREAK_IF(! pMenu);
//
//        this->addChild(pMenu);
        
        
//        this->setTouchEnabled(true);
        
        this->schedule(schedule_selector(AStar::update));
        
        
        _m_Map.Get(3,3)->setWeight(4);
        _m_Map.Get(4,3)->setWeight(4);
        _m_Map.Get(5,3)->setWeight(4);
        _m_Map.Get(6,3)->setWeight(4);
        _m_Map.Get(7,3)->setWeight(4);

        _m_Map.Get(3,4)->setWeight(4);
        _m_Map.Get(3,5)->setWeight(4);
        _m_Map.Get(3,6)->setWeight(4);
        _m_Map.Get(7,4)->setWeight(4);
        _m_Map.Get(7,5)->setWeight(4);
        _m_Map.Get(7,6)->setWeight(4);

        _startX = 4;
        _startY = 5;
        _goalX = 5;
        _goalY = 1;
        menuPathFinding(kAStar);
        
        bRet = true;
    } while (0);
    
    return bRet;
}
void AStar::setPathState(int x, int y, int state){
    _m_Map.Get(x, y)->setWeight(state);
}
deque<Cell*> AStar::getPath(int startX, int startY, int endX, int endY){
    _startX = startX;
    _startY = startY;
    _goalX = endX;
    _goalY = endY;
    menuPathFinding(kAStar);
    return _vecPathCell;
}
void AStar::menuPathFinding(PathFindingType type){
    
    
    fullFillPlayerAndGoalData();
    
    if(_startX >= 0 && _startX < xLineCount && _startY >=0 && _startY <= yLineCount
       && _goalX >= 0 && _goalX < xLineCount && _goalY >=0 && _goalY <= yLineCount){
        clearPathAndCells();
        
        switch (type){
            case kDistanceBase:
                startPathFinding(compareTwoCellsByDistance,_startX,_startY,_goalX,_goalY);
                break;
            case kSimpleHeuristic:
                startPathFinding(comparebyWhichNearerGoalSimpleWay,_startX,_startY,_goalX,_goalY);
                break;
            case kComplexHeuristic:
                startPathFinding(comparebyWhichNearerGoalPhysicWay,_startX,_startY,_goalX,_goalY);
                break;
            case kAStar:
                startPathFinding(comparebyDistanceBetweenStartAndGoal,_startX,_startY,_goalX,_goalY);
                break;
        };
//        _startDrawProgress = true;
        
        //build path
        fullFillPathData();
        
//        float distance = _m_Map.Get(_goalX,_goalY)->getDistance();
//        CCLOG("the lowest distance: %f",distance);
    }else{
        CCLOG("invalidate position!");
    }
};
void AStar::fullFillPathData()
{
    _vecPathCell.clear();
    Cell* cell = _m_Map.Get(_goalX,_goalY);
    if(partialPathToNearest && cell->getLastX() == -1
       && !(_goalX == _startX && _goalY == _startY)){
        // Goal never reached - fall back to the reachable cell closest to it.
        // Every cell reached by the search is marked and has a lastX backlink
        // (only the start cell is marked with lastX == -1).
        Cell* best = nullptr;
        float bestGoalDist = 0;
        for(int i = 0; i < xLineCount; ++i){
            for(int j = 0; j < yLineCount; ++j){
                Cell* c = _m_Map.Get(i,j);
                if(!c->getMarked()) continue;
                if(c->getLastX() == -1 && !(i == _startX && j == _startY)) continue;
                float d = distanceBetweenTwoCells((float)i,(float)j,(float)_goalX,(float)_goalY);
                if(best == nullptr || d < bestGoalDist
                   || (d == bestGoalDist && c->getDistance() < best->getDistance())){
                    best = c;
                    bestGoalDist = d;
                }
            }
        }
        if(best == nullptr || (best->getX() == _startX && best->getY() == _startY)){
            return; // already as close as the grid allows - report "no path"
        }
        cell = best;
    }
    while(cell->getLastX() != -1){
        _vecPathCell.push_front(cell);
        cell = _m_Map.Get(cell->getLastX(),cell->getLastY());
    }
    _vecPathCell.push_front(_m_Map.Get(_startX, _startY));
}

void AStar::fullFillPlayerAndGoalData(){
//    CCPoint playerPosition = _player->getPosition();
//    CCPoint goalPosition = _goal->getPosition();
//
//    _startX = (playerPosition.x - leftMargin) / _eachBoxWidth;
//    _startY = (playerPosition.y - bottomMargin) / _eachBoxHeight;
//
//    _goalX = (goalPosition.x - leftMargin) / _eachBoxWidth;
//    _goalY = (goalPosition.y - bottomMargin) / _eachBoxHeight;
//
    g_goalX = _goalX;
    g_goalY = _goalY;
    g_useSqrtHeuristic = admissibleHeuristic;
}

void AStar::menuDistanceBasedFindingCallback(cocos2d::Ref* pSender)
{
    menuPathFinding(kDistanceBase);
}

void AStar::menuSimpleHeuristicBasedFindingCallback(cocos2d::Ref* pSender)
{
    menuPathFinding(kSimpleHeuristic);
}


void AStar::menuComplexHeuristicBasedFindingCallback(cocos2d::Ref* pSender)
{
    menuPathFinding(kComplexHeuristic);
}


void AStar::menuAStarFindingCallback(cocos2d::Ref* pSender)
{
    menuPathFinding(kAStar);
}


void AStar::initCells(){
    //init map
    for(int i = 0; i < xLineCount; ++i){
        for(int j = 0; j < yLineCount; ++j){
            Cell *cell = new Cell();
            cell->setX(i);
            cell->setY(j);
            _m_Map.Set(i,j,cell);
        }
    }
}

void AStar::clearCells(){
    //init map
    for(int i = 0; i < xLineCount; ++i){
        for(int j = 0; j < yLineCount; ++j){
            Cell *cell = _m_Map.Get(i,j);
            cell->setMarked(false);
            cell->setDistance(0);
            cell->setLastX(-1);
            cell->setLastY(-1);
            cell->setDrawProgress(false);
            _m_Map.Set(i,j,cell);
        }
    }
}

void AStar::startPathFinding(compareTwoCells compareMethod, int startX,int startY,int goalX,int goalY){
    Cell *startCell = _m_Map.Get(startX, startY);
    vector<Cell*> vecCells;
    vecCells.push_back(startCell);
    make_heap(vecCells.begin(),vecCells.end(),compareMethod);
    startCell->setMarked(true);
    Cell *nowProcessCell;
    
    int searchCounter = 0;
    while(vecCells.size() != 0){
        pop_heap(vecCells.begin(),vecCells.end(),compareMethod);
        nowProcessCell = vecCells.back();
        vecCells.pop_back();
        
        _vecCommandCell.push_back(nowProcessCell);//just record the finding progress
        
        if(nowProcessCell->getX() == _goalX && nowProcessCell->getY() == _goalY){//the goal is reach
            return;
        }
        searchCounter++;
        if(searchCounter > SEARCH_LIMIT){
            return;
        }
        for(int i = 0; i < 8; ++i){ //check eight direction
            
            int indexX = nowProcessCell->getX() + DIRECTION[i][0];
            int indexY = nowProcessCell->getY() + DIRECTION[i][1];
            
            if(indexX >= 0 && indexX < xLineCount && indexY >= 0 && indexY < yLineCount
               && _m_Map.Get(indexX,indexY)->getPassable() == true){//check is a OK cell or not
                if(strictDiagonal && i >= 4 // diagonal directions are indices 4-7
                   && (!_m_Map.Get(indexX, nowProcessCell->getY())->getPassable()
                       || !_m_Map.Get(nowProcessCell->getX(), indexY)->getPassable())){
                    continue; // would cut the corner between two blocked cells
                }
                Cell *cell = _m_Map.Get(indexX,indexY);
                float beforeDistance = DISTANCE[i] * cell->getWeight() + _m_Map.Get(nowProcessCell->getX(),
                                                                                    nowProcessCell->getY())->getDistance();//calculate the distance
                if(cell->getMarked() == false){
                    cell->setMarked(true);
                    cell->setLastX(nowProcessCell->getX());
                    cell->setLastY(nowProcessCell->getY());
                    cell->setDistance(beforeDistance);
                    vecCells.push_back(cell);//only push the unmarked cell into the vector
                    push_heap(vecCells.begin(),vecCells.end(),compareMethod);
                }else{// if find a lower distance, update it
                    if(beforeDistance < cell->getDistance()){
                        cell->setDistance(beforeDistance);
                        cell->setLastX(nowProcessCell->getX());
                        cell->setLastY(nowProcessCell->getY());
                        make_heap(vecCells.begin(),vecCells.end(),compareMethod);//distance change,so make heap again
                    }
                }
            }
            
        }
    }
}


//void AStar::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent){
//    CCTouch *touch = (CCTouch*)pTouches->anyObject();
//    int indexX = (touch->getLocation().x - leftMargin) / _eachBoxWidth;
//    int indexY = (touch->getLocation().y - bottomMargin) / _eachBoxHeight;
//
//    if(indexX >=0 && indexX < xLineCount && indexY >=0 && indexY < yLineCount){
//        _m_Map.Get(indexX,indexY)->setWeight(_m_Map.Get(indexX,indexY)->getWeight() + 1);
//        _lineContainer->updateMapParameter(_m_Map);
//    }
//}

void AStar::clearPathAndCells(){
    clearCells();
    _vecCommandCell.clear();
//    _lineContainer->setStartDrawPath(false);
}
