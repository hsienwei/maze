
#include "Maze.h"

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "cocos2d.h"

using namespace std;

typedef enum
{
	MAZE_BLOCK_CLOSE = 0,
	MAZE_BLOCK_UP = 1,
	MAZE_BLOCK_DOWN = 2,
	MAZE_BLOCK_LEFT = 4,
	MAZE_BLOCK_RIGHT = 8,

} MazeBlockType;

typedef enum
{
	GRID_TRACE_UP = 0,
	GRID_TRACE_DOWN,
	GRID_TRACE_LEFT,
	GRID_TRACE_RIGHT,
	GRID_TRACE_MAX,
} GridTraceDirection;

Maze::~Maze()
{
	free(_data);
}

unsigned char getValue(unsigned char *data, int w, int h, int x, int y)
{
	return  *(data + (y * w + x));
}

void setValue(unsigned char *data, int w, int h, int x, int y, unsigned char value)
{
	*(data + (y * w + x)) = value;
}

void setGrid(Grid *grid, int x, int y)
{
	grid->x = x;
	grid->y = y;
}

boolean isInRange(int w, int h, int x, int y)
{
	bool rtn = true;
	if (x < 0)   rtn = false;
	if (y < 0)   rtn = false;
	if (x >= w)    rtn = false;
	if (y >= h)    rtn = false;
	return rtn;
}

Grid getGridByDirection( int x, int y, GridTraceDirection direction)
{
	Grid rtn;
	if (direction == GRID_TRACE_UP)
		setGrid(&rtn, x, y - 1);
	else if (direction == GRID_TRACE_DOWN)
		setGrid(&rtn, x, y + 1);
	else if (direction == GRID_TRACE_LEFT)
		setGrid(&rtn, x - 1, y);
	else if (direction == GRID_TRACE_RIGHT)
		setGrid(&rtn, x + 1, y);
	return rtn;
}


Grid getNeighbourNotVisit(unsigned char *data, int x, int y, int w, int h)
{
	Grid  rtn;
	vector<Grid> neighbourList = vector<Grid>();
	neighbourList.clear();
	for (int i = 0; i < GRID_TRACE_MAX; ++i)
	{
		Grid neighbourGrid = getGridByDirection(x, y, (GridTraceDirection)i);
		if (isInRange(w, h, neighbourGrid.x, neighbourGrid.y))
		{
			if (getValue(data, w, h, neighbourGrid.x, neighbourGrid.y) == 0)
			{
				neighbourList.push_back(neighbourGrid);
			}
		}
	}

	if (neighbourList.size() == 0)
	{
		//if no not visit neighbour
		setGrid(&rtn, -1, -1);
	}
	else
	{
		int rndIdx = (int)(CCRANDOM_0_1() * neighbourList.size());
		rtn = neighbourList.at(rndIdx);
	}
	return rtn;
}

void removeWall(unsigned char *data, Grid self, Grid neighbour, int w, int h)
{
	int xDiff = neighbour.x - self.x;
	int yDiff = neighbour.y - self.y;
	unsigned char selfState = getValue(data, w, h, self.x, self.y);
	unsigned char neighbourState = getValue(data, w, h, neighbour.x, neighbour.y);

	if (xDiff == -1)  //left
	{
		setValue(data, w, h, self.x, self.y, MAZE_BLOCK_LEFT | selfState);
		setValue(data, w, h, neighbour.x, neighbour.y, MAZE_BLOCK_RIGHT | neighbourState);
	}
	if (xDiff == 1)  //right
	{
		setValue(data, w, h, self.x, self.y, MAZE_BLOCK_RIGHT | selfState);
		setValue(data, w, h, neighbour.x, neighbour.y, MAZE_BLOCK_LEFT | neighbourState);
	}
	if (yDiff == -1)  //up
	{
		setValue(data, w, h, self.x, self.y, MAZE_BLOCK_UP | selfState);
		setValue(data, w, h, neighbour.x, neighbour.y, MAZE_BLOCK_DOWN | neighbourState);
	}
	if (yDiff == 1)  //down
	{
		setValue(data, w, h, self.x, self.y, MAZE_BLOCK_DOWN | selfState);
		setValue(data, w, h, neighbour.x, neighbour.y, MAZE_BLOCK_UP | neighbourState);
	}
}

Maze* MazeFactory::mazeCreate(int width, int height, MazeCreateAlgorithm algorithm)
{
	srand(time(NULL));
	unsigned char *traceRecord = NULL;
	vector<Grid> traceStack = vector<Grid>();
	traceStack.clear();
	int longestDistance = 0;

	Maze *maze = new Maze();
	maze->_width = width;
	maze->_height = height;
	maze->mazeType = NORMAL;
	maze->_data = (unsigned char*)malloc(width * height * sizeof(unsigned char));
	traceRecord = (unsigned char*)malloc(width * height * sizeof(unsigned char));

	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			*(maze->_data + (j * width + i)) = MAZE_BLOCK_CLOSE;
			*(traceRecord + (j * width + i)) = 0;
		}
	}

	//Recursive_backtracker
	//1 Make the initial cell the current cell and mark it as visited
	Grid initGrid;
	setGrid(&initGrid, (int)(CCRANDOM_0_1() * width), (int)(CCRANDOM_0_1() * height));
	setValue(traceRecord, width, height, initGrid.x, initGrid.y, 1);
	traceStack.push_back(initGrid);
	setGrid(&(maze->_start), initGrid.x, initGrid.y);

	//2 While there are unvisited cells
	while (traceStack.size() > 0)
	{
		Grid currentGrid = traceStack.at(traceStack.size()-1 );
		Grid neighbour = getNeighbourNotVisit(traceRecord, currentGrid.x, currentGrid.y, width, height);
		//1 If the current cell has any neighbours which have not been visited
		if (isInRange(width, height, neighbour.x, neighbour.y))
		{
			//	1 Choose randomly one of the unvisited neighbours
			//	2 Push the current cell to the stack
			traceStack.push_back(neighbour);
			//	3 Remove the wall between the current cell and the chosen cell
			removeWall(maze->_data, currentGrid, neighbour, width, height);
			//	4 Make the chosen cell the current cell and mark it as visited
			setValue(traceRecord, width, height, neighbour.x, neighbour.y, 1);
		}
		//2 Else if stack is not empty
		else 
		{
			cocos2d::log("(%d, %d) %d", currentGrid.x, currentGrid.y, traceStack.size());
			cocos2d::log("%d, %d", traceStack.size(), longestDistance);
			if (traceStack.size() > longestDistance)
			{
				longestDistance = traceStack.size();
				setGrid(&(maze->_end), currentGrid.x, currentGrid.y);
			}

			//1 Pop a cell from the stack
			traceStack.pop_back();
		}
	}

	free(traceRecord);
	return maze;
}

string UTF8TOASCII( wstring text)
{
	wstring tes;
	int asciSize = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), text.size(), NULL, 0, NULL, NULL);
	if (asciSize == ERROR_NO_UNICODE_TRANSLATION || asciSize == 0)
	{
		return string();
	}

	char *resultString = new char[asciSize];
	int conveResult = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), text.size(), resultString, asciSize, NULL, NULL);
	if (conveResult != asciSize)
	{
		return string();
	}
	string buffer = "";
	buffer.append(resultString, asciSize);

	delete[] resultString;
	return buffer;
}


Maze* MazeFactory::transformMaze(Maze* maze, MazeType toType)
{
	if (maze->mazeType == toType)
		return NULL;

	if (toType != EXTENDED)    
		return NULL;  //only support one type right now

	Maze *newMaze = new Maze();
	int w = maze->_width + 2 + maze->_width - 1;
	int h = maze->_height + 2 + maze->_height - 1;
	newMaze->_width = w;
	newMaze->_height = h;
	newMaze->mazeType = toType;
	newMaze->_data = (unsigned char*)malloc(w * h * sizeof(unsigned char));

	//init
	for (int j = 0; j < h; ++j)
	{
		for (int i = 0; i < w; ++i)
		{
			setValue(newMaze->_data, w, h, i, j, 0);
		}
	}

	//expend
	for (int j = 0; j < maze->_height; ++j)
	{
		for (int i = 0; i < maze->_width; ++i)
		{
			int idxX = i * 2 + 1;
			int idxY = j * 2 + 1;
			unsigned char gridState = getValue(maze->_data, maze->_width, maze->_height, i, j);

			if (gridState != MAZE_BLOCK_CLOSE)
			{
				if (i == maze->_start.x && j == maze->_start.y)
				{
					setGrid(&(newMaze->_start), idxX, idxY);
				}
				else if (i == maze->_end.x && j == maze->_end.y)
				{
					setGrid(&(newMaze->_end), idxX, idxY);
				}
				setValue(newMaze->_data, w, h, idxX, idxY, 1);
			}

			if (i < maze->_width - 1)
			{
				if ((gridState & MAZE_BLOCK_RIGHT) == MAZE_BLOCK_RIGHT)
					setValue(newMaze->_data, w, h, idxX + 1, idxY, 1);
			}
			if (j < maze->_height - 1)
			{
				if ((gridState & MAZE_BLOCK_DOWN) == MAZE_BLOCK_DOWN)
					setValue(newMaze->_data, w, h, idxX, idxY + 1, 1);
			}
		}
	}
	return newMaze;
}

void  MazeFactory::printMaze(Maze *maze)
{
	wstring str = wstring(L"");

	if (maze->mazeType == NORMAL)
	{

		for (int j = 0; j < maze->_height; ++j)
		{
			for (int i = 0; i < maze->_width; ++i)
			{
				unsigned char direction = *(maze->_data + (j * maze->_width + i));

				if (maze->_start.x == i && maze->_start.y == j)
					str.append(L"Ｓ");
				else if (maze->_end.x == i && maze->_end.y == j)
					str.append(L"Ｅ");
				else
				{
					if (direction == 0)		str.append(L"。");
					if (direction == 1)		str.append(L"↓"); // u
					if (direction == 2)		str.append(L"↑"); // d
					if (direction == 3)		str.append(L"│"); // u d
					if (direction == 4)		str.append(L"→"); // l
					if (direction == 5)		str.append(L"┘"); // u l
					if (direction == 6)		str.append(L"┐"); // d l
					if (direction == 7)		str.append(L"┤"); // u d l
					if (direction == 8)		str.append(L"←"); // r
					if (direction == 9)		str.append(L"└"); // u r 
					if (direction == 10)	str.append(L"┌"); // d r 
					if (direction == 11)	str.append(L"├"); // u d r 
					if (direction == 12)	str.append(L"─"); // l r
					if (direction == 13)	str.append(L"┴"); // u l r
					if (direction == 14)	str.append(L"┬"); // d l r
					if (direction == 15)	str.append(L"┼"); // u d l r
				}
			}
			str.append(L"\n");
		}
	}
	else if (maze->mazeType == EXTENDED)
	{
		for (int j = 0; j < maze->_height; ++j)
		{
			for (int i = 0; i < maze->_width; ++i)
			{
				int num = (*(maze->_data + (j * maze->_width + i)));
				if (i == maze->_start.x && j == maze->_start.y)
					str.append(L"※");
				else if (i == maze->_end.x && j == maze->_end.y)
					str.append(L"◎");
				else
					str.append(num >= 1 ? L"○" : L"●");
			}
			str.append(L"\n");
		}
	}

	string convertedStr = UTF8TOASCII(str);
	cocos2d::log("%s", convertedStr.c_str());
}