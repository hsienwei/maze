#ifndef  _MAZE_H_
#define  _MAZE_H_

typedef enum
{
	NORMAL,
	EXTENDED,

} MazeType;

typedef enum
{
	RECURSIVE_BACKTRACKER,

} MazeCreateAlgorithm;

typedef struct
{
	int x;
	int y;
} Grid;

class Maze
{
public:
	~Maze();

	MazeType mazeType;
	int _width;
	int _height;
	unsigned char *_data;
	Grid _start, _end;
};


class MazeFactory
{
public:
	static Maze* mazeCreate(int width, int height, MazeCreateAlgorithm algorithm);
	static Maze* transformMaze(Maze* maze, MazeType toType);
	static void  printMaze(Maze *maze);
};


#endif //_MAZE_H_