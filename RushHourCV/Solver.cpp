#include "Solver.h"

Car::Car(int row, int col, int l, bool v) :
	row(row), col(col), length(l), isVertical(v) {}

// Given a Rush Hour game board, get a list of pieces
vector<Car> parseBoard(char board[][BOARD_SIZE])
{
	vector<Car> pieces;
	map<char, vector<coords>> locations;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] != '.') {
				locations[board[i][j]].push_back(coords{ i , j });
			}
		}
	}
	for (auto it = locations.begin(); it != locations.end(); ++it) {
		bool isVertical = false;
		if (it->second[0].col == it->second[1].col) {
			isVertical = true;
		}
		pieces.push_back(Car(it->second[0].row + 1, it->second[0].col + 1, it->second.size(), isVertical));
	}
	return pieces;
}