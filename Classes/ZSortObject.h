#ifndef _ZSORTOBJECT__H_
#define _ZSORTOBJECT__H_

#include "cocos2d.h"
#include <stdlib.h>
USING_NS_CC;


class GameSprite;


//copy from cocos2d-x old version
typedef struct _ccGridSize
{
	int    x;
	int    y;
} ccGridSize;

//! helper function to create a ccGridSize
static inline ccGridSize
ccg(const int x, const int y)
{
	ccGridSize v = { x, y };
	return v;
}


struct NodeList
{
	GameSprite** nodeList;
	int      nodeNum;
};

class ZSortObject
{
public:
	ZSortObject()
	{
		behind.nodeList = NULL;
		theSame.nodeList = NULL;
	}
	~ZSortObject()
	{
		CC_SAFE_FREE(behind.nodeList);
		CC_SAFE_FREE(theSame.nodeList);
	}
	virtual ccGridSize& getGridTopIndex()
	{
		return topIndex;
	}
	virtual void        resetGridTopIndex()        //依照設定的GridIndex 與 GridSize產出  GridTopIndex
	{
		//ccGridSize idx = this->getGridIndex();
		//ccGridSize size = this->getGridSize();
		topIndex = ccg(index.x - size.x + 1, index.y - size.y + 1);
	}
	virtual ccGridSize& getGridIndex()
	{
		return index;
	}
	virtual void        setGridIndex(ccGridSize& pIdx)
	{
		index = pIdx;
		resetGridTopIndex();
	}
	virtual ccGridSize& getGridSize()
	{
		return size;
	}
	virtual void        setGridSize(ccGridSize& pSize)
	{
		size = pSize;
		resetGridTopIndex();
	}

	void  setIsVisit(bool pIsVisit)
	{
		isVisit = pIsVisit;
	}

	bool&  getIsVisit()
	{
		return isVisit;
	}

	bool*  getIsVisitPtr()
	{
		return &isVisit;
	}

	void  setTheSameList(NodeList &pSameList)
	{
		theSame = pSameList;
	}

	NodeList*  getTheSameList()
	{
		return &(theSame);
	}
	void  setBehindList(NodeList &pBehindList)
	{
		behind = pBehindList;
	}

	NodeList*  getBehindList()
	{
		return &(behind);
	}

	int& getPreSortOrder()
	{
		return tempOrder;
	}

	virtual void setPreSortOrder(int pOrder)
	{
		tempOrder = pOrder;
	}

	void resetSortList(int listSize)
	{
		CC_SAFE_FREE(behind.nodeList);
		CC_SAFE_FREE(theSame.nodeList);

		GameSprite** ptrBehind = (GameSprite**)malloc(listSize * sizeof(GameSprite*));
		GameSprite** ptrTheSame = (GameSprite**)malloc(listSize * sizeof(GameSprite*));

		behind.nodeList = ptrBehind;
		behind.nodeNum = 0;

		theSame.nodeList = ptrTheSame;
		theSame.nodeNum = 0;
	}

protected:
	ccGridSize   topIndex;
	ccGridSize   index;
	ccGridSize   size;
	NodeList     behind;
	NodeList     theSame;
	bool         isVisit;
	int          tempOrder;
};


#define GAMESPRITE_SHOW_ZORDER  秀排序除錯訊息
//排序用Sprite
class GameSprite :public cocos2d::CCSprite, public ZSortObject
{
#ifdef     GAMESPRITE_SHOW_ZORDER      //測試用功能：顯示排序訊息
protected:
	CCLabelTTF *zvalueLabel;
public:
	virtual bool init();
	virtual void setPreSortOrder(int pOrder);

#endif
public:
	CREATE_FUNC(GameSprite);
};


class Map
{
private:
	GameSprite** allNeedZSortAry;
	int m_iDepth;
	int ptrResetCount;
	int needZSortCount;
	int genZSortCount;
	int gridWidth;
	int gridHeight;
	static Map _instance;

	Map();
public:
	static Map* instance();

	void fixZValue(bool isForce);
	void place(GameSprite* pNode, bool isForce);
	void addToZSortArray(GameSprite* obj);
	void removeFromZSortArray(GameSprite* obj);
	void resetZSortArray();

	CCPoint getMapPosition(int x, int y);
	CCPoint getMapPosition(int x, int y, int tileWidth, int tileHeight);
	ccGridSize posToMap(CCPoint &tpos);
	ccGridSize posToMap(CCPoint &tpos, int tileWidth, int tileHeight);
	//ccGridSize spritePosToMap(CCPoint tpos);
	//ccGridSize spritePosToMap(CCPoint tpos, int tileHeight);
};

#endif //_ZSORTOBJECT__H_