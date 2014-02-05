#include "ZSortObject.h"

#ifdef     GAMESPRITE_SHOW_ZORDER
bool GameSprite::init()
{
	bool rtn = CCSprite::init();
	if (rtn)
	{
		zvalueLabel = CCLabelTTF::create(" ", "Arial", 14);
		zvalueLabel->setColor(ccBLACK);
		this->addChild(zvalueLabel, 1000, 1000);
	}

	return rtn;
}

void GameSprite::setPreSortOrder(int pOrder)
{
	char strch[20];
	ZSortObject::setPreSortOrder(pOrder);
	sprintf(strch, "%d(%d:%d)\n[%d:%d]", pOrder, index.x, index.y, size.x, size.y);
	zvalueLabel->setString(strch);
}
#endif


Map Map::_instance = Map();

Map::Map()
{
	allNeedZSortAry = NULL;
	m_iDepth = 0;
	ptrResetCount = 0;
	needZSortCount = 0;
	genZSortCount = 0;
	gridWidth = 118;
	gridHeight = 54;
}

Map* Map::instance()
{
	return &_instance;
}



void Map::fixZValue(bool isForce)
{
	GameSprite *pNode = NULL, *pOtherNode = NULL;

	ccGridSize index1;
	ccGridSize index2;
	ccGridSize topIndex1, topIndex2;
	int i = 0, j = 0;

	m_iDepth = 1;

	//初始化
	for (i = 0; i< needZSortCount; ++i)
	{
		allNeedZSortAry[i]->setIsVisit(false);
		allNeedZSortAry[i]->getTheSameList()->nodeNum = 0;
		allNeedZSortAry[i]->getBehindList()->nodeNum = 0;
	}

	//重新設定排序用陣列(如果有增加或減少排序物件)
	if (ptrResetCount)
	{
		for (i = 0; i< needZSortCount; ++i)
		{
			allNeedZSortAry[i]->resetSortList(needZSortCount);
		}
		ptrResetCount = false;
	}


	for (i = 0; i< needZSortCount; ++i)
	{
		pNode = allNeedZSortAry[i];
		index1 = pNode->getGridIndex();

		for (j = i + 1; j< needZSortCount; ++j)
		{
			pOtherNode = allNeedZSortAry[j];
			index2 = pOtherNode->getGridIndex();

			if (index1.x == index2.x && index1.y == index2.y)
			{
				//同一格
				(pNode->getTheSameList()->nodeList)[pNode->getTheSameList()->nodeNum] = pOtherNode;//theSame[theSameCount] = pOtherNode;
				(pNode->getTheSameList()->nodeNum)++;
			}
			else
			{
				topIndex2 = pOtherNode->getGridTopIndex();
				if (topIndex2.x <= index1.x && topIndex2.y <= index1.y)
				{
					(pNode->getBehindList()->nodeList)[pNode->getBehindList()->nodeNum] = pOtherNode;
					pNode->getBehindList()->nodeNum++;
				}
				else
				{
					topIndex1 = pNode->getGridTopIndex();
					if (topIndex1.x <= index2.x && topIndex1.y <= index2.y)
					{
						(pOtherNode->getBehindList()->nodeList)[pOtherNode->getBehindList()->nodeNum] = pNode;
						pOtherNode->getBehindList()->nodeNum++;
					}
				}
			}
		}
	}

	for (i = 0; i< needZSortCount; ++i)
	{
		pNode = allNeedZSortAry[i];
		place(pNode, isForce);
	}
}

void Map::place(GameSprite* pNode, bool isForce)
{
	if (pNode->getIsVisit() != false)    return;
	pNode->setIsVisit(true);

	int i = 0;
	NodeList *processList = NULL;
	GameSprite *pSameNode = NULL;

	processList = pNode->getBehindList();
	for (i = 0; i< processList->nodeNum; ++i)
	{
		place(processList->nodeList[i], isForce);
	}

	if (isForce || pNode->getPreSortOrder() != m_iDepth)
	{
		if (pNode->getParent() != NULL)
		{
			pNode->getParent()->reorderChild(pNode, m_iDepth);
			pNode->setPreSortOrder(m_iDepth);
		}
	}

	//拜訪所有在同一格的物件
	processList = pNode->getTheSameList();
	for (i = 0; i< processList->nodeNum; ++i)
	{
		pSameNode = processList->nodeList[i];
		if (pSameNode->getIsVisit() == false)
		{
			pSameNode->setIsVisit(true);

			if (isForce || pSameNode->getPreSortOrder() != m_iDepth)
			{
				if (pSameNode->getParent() != NULL)
				{
					pSameNode->getParent()->reorderChild(pSameNode, m_iDepth);
					pSameNode->setPreSortOrder(m_iDepth);
				}
			}
		}
	}
	++m_iDepth;
}

void Map::addToZSortArray(GameSprite* obj)
{

	//先檢查有沒有
	if (allNeedZSortAry != NULL)
	{
		for (int i = 0; i<needZSortCount; i++)
		{
			if (allNeedZSortAry[i] == obj)
			{
				return;
			}
		}
	}

	if (allNeedZSortAry == NULL || (needZSortCount >= genZSortCount))
	{
		//        GameSprite** tempList = allNeedZSortAry;
		//        genZSortCount+=10;  // 一次加10個
		//        allNeedZSortAry =    (GameSprite**)malloc(genZSortCount * sizeof(GameSprite*));
		//        if(tempList != NULL)
		//            memcpy(allNeedZSortAry, tempList, sizeof(GameSprite*) * needZSortCount);
		//        
		//        CC_SAFE_FREE(tempList);

		genZSortCount += 10;  // 一次加10個

		if (allNeedZSortAry == NULL)
			allNeedZSortAry = (GameSprite**)malloc(genZSortCount * sizeof(GameSprite*));
		else
		{
			void* tempList = realloc(allNeedZSortAry, genZSortCount * sizeof(GameSprite*));
			if (tempList != NULL)  allNeedZSortAry = (GameSprite**)tempList;
		}
	}

	allNeedZSortAry[needZSortCount] = obj;
	needZSortCount++;
	ptrResetCount = true;
}

void Map::removeFromZSortArray(GameSprite* obj)
{
	if (allNeedZSortAry == NULL)    return;
	int  moveAhead = 0;
	for (int i = 0; i<needZSortCount; ++i)
	{
		if (moveAhead > 0)
		{
			allNeedZSortAry[i - moveAhead] = allNeedZSortAry[i];
		}
		if (obj == allNeedZSortAry[i])
		{
			moveAhead += 1;
		}

	}
	needZSortCount -= moveAhead;
	ptrResetCount = true;
}

void Map::resetZSortArray()
{
	needZSortCount = 0;
}

CCPoint Map::getMapPosition(int x, int y)
{
	return getMapPosition(x, y, gridWidth, gridHeight);
}

CCPoint Map::getMapPosition(int x, int y, int tileWidth, int tileHeight)
{
	return ccp(	(x - y)* (tileWidth*0.5f),
				(x + y) * -(tileHeight*0.5f));
}

ccGridSize Map::posToMap(CCPoint &tpos)
{
	return posToMap(tpos, gridWidth, gridHeight);
}

ccGridSize Map::posToMap(CCPoint &tpos, int tileWidth, int tileHeight)
{
	float x = ((tpos.x) / (tileWidth)+(tpos.y) / -(tileHeight));
	int gridx = (int)((x >= 0) ? (x + 0.5) : (x - 0.5));

	float y = ((tpos.y) / -(tileHeight)-(tpos.x) / (tileWidth));
	int gridy = (int)((y >= 0) ? (y + 0.5) : (y - 0.5));

	return ccg(gridx, gridy);
}
/*
ccGridSize Map::spritePosToMap(CCPoint tpos)
{
	return spritePosToMap(tpos, gridHeight);
}

ccGridSize Map::spritePosToMap(CCPoint tpos, int tileHeight)
{
	CCPoint pos = ccpAdd(tpos, ccp(0, tileHeight*0.5f));
	return posToMap(pos);
}*/