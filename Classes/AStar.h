#ifndef __HELLOWORLDastar_SCENE_H__
#define __HELLOWORLDastar_SCENE_H__

#include "cocos2d.h"
//#include "LineContainer.h"
//#include "DragSprite.h"
#include "Array2D.h"
#include "Cell.h"
#include <deque>

#define PATH_OCCUPIED 4
#define PATH_EMPTY 1
#define MAP_WIDTH 100
#define MAP_HEIGHT 100
#define SEARCH_LIMIT 100

USING_NS_CC;
using namespace std;

typedef enum{
    kDistanceBase,
    kSimpleHeuristic,
    kComplexHeuristic,
    kAStar
}PathFindingType;

typedef bool (*compareTwoCells)(Cell *c1, Cell *c2);
class AStar : public cocos2d::Layer
{
public:
    
    int xLineCount = MAP_WIDTH;
    int yLineCount = MAP_HEIGHT;
    
    const int DIRECTION[8][2]={
        {0,1},  //north
        {1,0},  //east
        {0,-1},  //south
        {-1,0},  //west
        {1,1},  //northeast
        {1,-1},  //southeast
        {-1,-1},  //southwest
        {-1,1}  //northwest
    };
    
    const float DISTANCE[8]={
        1,
        1,
        1,
        1,
        1.414f,
        1.414f,
        1.414f,
        1.414f
    };
    
    
    AStar();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::Scene* scene();
    void setMap(int width, int height);
    // a selector callback
    void menuDistanceBasedFindingCallback(cocos2d::Ref* pSender);
    void menuSimpleHeuristicBasedFindingCallback(cocos2d::Ref* pSender);
    void menuComplexHeuristicBasedFindingCallback(cocos2d::Ref* pSender);
    void menuAStarFindingCallback(cocos2d::Ref* pSender);
    void menuPathFinding(PathFindingType type);
    
    
    // implement the "static node()" method manually
    CREATE_FUNC(AStar);
    
//    void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
    
    void setPathState(int x, int y, int state);
    deque<Cell*> getPath(int startX, int startY, int endX, int endY);
private:
//    DragSprite *_player;
//    DragSprite *_goal;
//    LineContainer *_lineContainer;
    cocos2d::Size _screenSize;
//    float _eachBoxWidth;
//    float _eachBoxHeight;
    
    Array2D<Cell> _m_Map;
    int _startX;
    int _startY;
    int _goalX;
    int _goalY;
    deque<Cell*> _vecCommandCell;
    deque<Cell*> _vecPathCell;
    bool _startDrawProgress;
    float _pastTime;
    
    void initCells();
    void clearCells();
    void startPathFinding(compareTwoCells compareMethod, int startX,int startY,int goalX,int goalY);
    void fullFillPathData();
    void fullFillPlayerAndGoalData();
    void clearPathAndCells();
};

#endif  // __HELLOWORLD_SCENE_H__
