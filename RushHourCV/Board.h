#pragma once
#include <cstdint>
#include <vector>
#include <iostream>
#include <tuple>
#include <map>

using namespace std;

#define BOARD_SIZE 6

const int Horz = 1;
const int Vert = BOARD_SIZE;

typedef uint64_t bitb;

class Piece
{
public:
	void movePiece(int steps);

	int m_length;
	int m_orient;
	int m_pos;
	bitb m_mask;
};

class Board
{
public:
	Board() : m_horizontal(0), m_vertical(0) {}
	Board(const Board& b);
	Board(const Board& b, const Piece& p, const int steps);
	void addPiece(int row, int col, int length, int orientation);
	void addMain(int row, int col);
	void printBoard();
	void printFlat();

	bitb getMask() const {
		return m_horizontal | m_vertical;
	}
	bitb getHorz() const {
		return m_horizontal;
	}
	bitb getVert() const {
		return m_vertical;
	}
	const vector<Piece>& getPieces() const {
		return m_pieces;
	}
	pair<bitb, bitb> getComposition() const {
		return make_pair(m_horizontal, m_vertical);
	}
	bool isSolved() const {
		return m_pieces[0].m_mask & ((bitb)1 << 17);
	}

private:
	bitb m_horizontal, m_vertical;
	vector<Piece> m_pieces;
};