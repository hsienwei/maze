#include "HelloWorldScene.h"

#include "Maze.h"
#include "ZSortObject.h"




Maze *maze2 = NULL;
Layer *mapTileLayer = NULL;
Grid currentGrid;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
	Maze *maze = MazeFactory::mazeCreate(20	, 20, RECURSIVE_BACKTRACKER);

	MazeFactory::printMaze(maze);

	maze2 = MazeFactory::transformMaze(maze, EXTENDED);

	MazeFactory::printMaze(maze2);

	mapTileLayer = Layer::create();
	this->addChild(mapTileLayer);

	for (int j = 0; j < maze2->_height; ++j)
	{
		for (int i = 0; i < maze2->_width; ++i)
		{
			unsigned char v = maze2->value(i, j);
			if (v == 1)
			{
				GameSprite *gSprite = GameSprite::create();
				gSprite->initWithFile("Floor_00.png");
				gSprite->setPosition( Map::instance()->getMapPosition(i, j));
				mapTileLayer->addChild(gSprite);
				gSprite->setGridIndex(ccg(i, j));
				Map::instance()->addToZSortArray(gSprite);

				if (i == maze2->_start.x && j == maze2->_start.y)
					gSprite->setColor(Color3B::RED);
				if (i == maze2->_end.x && j == maze2->_end.y)
					gSprite->setColor(Color3B::GREEN);
			}
		}
	}
	currentGrid.x = maze2->_start.x;
	currentGrid.y = maze2->_start.y;
	focusOnGrid(maze2->_start);

	
	
	// Register Touch Event
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::touchBegan, this);
	//listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
	//listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::touchBegan, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	delete maze;

    return true;
}


void HelloWorld::menuCloseCallback(Object* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}


void HelloWorld::focusOnGrid(Grid &grid)
{
	//focus on start
	Point focusPos = Map::instance()->getMapPosition(grid.x, grid.y);
	Size winSize = CCDirector::sharedDirector()->getWinSize();
	mapTileLayer->setPosition(ccp(winSize.width / 2 - focusPos.x, winSize.height / 2 - focusPos.y));
}

bool HelloWorld::touchBegan(Touch *touch, Event *unused_event)
{
	Size winSize = CCDirector::sharedDirector()->getWinSize();
	Point midPos = Point(winSize.width / 2, winSize.height / 2);
	Point touchPos = this->convertTouchToNodeSpace(touch);
	Point subPos = touchPos - midPos;
	Grid grid;
	if (subPos.x < 0 && subPos.y < 0)
	{
		grid.x = 0;    grid.y = 1; //down
	}
	else if (subPos.x > 0 && subPos.y < 0)
	{
		grid.x = 1;    grid.y = 0; //right
	}
	else if (subPos.x < 0 && subPos.y > 0)
	{
		grid.x = -1;    grid.y = 0;//left
	}
	else if (subPos.x > 0 && subPos.y > 0)
	{
		grid.x = 0;    grid.y = -1;//up
	}

	unsigned char v = maze2->value(currentGrid.x + grid.x, currentGrid.y + grid.y);
	if (v == 1)
	{
		currentGrid.x = currentGrid.x + grid.x;
		currentGrid.y = currentGrid.y + grid.y;
		focusOnGrid(currentGrid);
	}
	return true;
}