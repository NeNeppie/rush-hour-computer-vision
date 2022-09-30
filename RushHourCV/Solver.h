#pragma once
#include "Board.h"
#include <queue>
#include <set>

class Solver
{
public:
	Solver();
	int getNextMoves(Board& b);
	vector<Board> solveBoard(const Board& root);

private:
	queue<Board> m_moves;
	queue<vector<Board>> m_paths;
	bitb m_topRow, m_bottomRow, m_leftCol, m_rightCol;
};

/*
1 1 1 1 1 1
1 0 0 0 0 1
1 0 0 0 0 1
1 0 0 0 0 1
1 0 0 0 0 1
1 1 1 1 1 1
*/