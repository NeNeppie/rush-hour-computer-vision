#pragma once
#include "stdafx.h"
#include <vector>

#define BOARD_SIZE 6

typedef uint64_t bitboard;

typedef struct {
	int row;
	int col;
} coords;

class Car 
{
public:
	Car(int row, int col, int l, bool v);

	int row;	// starting row
	int col;	// starting col
	int length;	// size (2 / 3)
	bool isVertical;

private:

};

vector<Car> parseBoard(char board[][BOARD_SIZE]);