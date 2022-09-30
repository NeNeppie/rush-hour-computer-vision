#include "Board.h"

void Piece::movePiece(int steps)
{
	int distance = steps * m_orient;
	m_pos += distance;

	if (steps > 0) {
		m_mask <<= distance;
	}
	else {
		m_mask >>= -distance;
	}
}

Board::Board(const Board& b)
{
	m_horizontal = b.m_horizontal;
	m_vertical = b.m_vertical;
	m_pieces = b.m_pieces;
}

Board::Board(const Board& b, const Piece& p, const int steps) : Board(b)
{
	auto& pieces = m_pieces;
	for (Piece& piece : pieces) {
		if (piece.m_pos == p.m_pos) {
			if (piece.m_orient == Vert) {
				m_vertical &= ~piece.m_mask;
				piece.movePiece(steps);
				m_vertical |= piece.m_mask;
			}
			else {
				m_horizontal &= ~piece.m_mask;
				piece.movePiece(steps);
				m_horizontal |= piece.m_mask;
			}
		}
	}
}

void Board::addPiece(int row, int col, int length, int orientation)
{
	bitb pieceMask = 0;
	int pos = row * 6 + col;
	for (int i = 0; i < length; i++) {
		pieceMask |= (bitb)1 << pos;
		pos += orientation;
	}
	m_pieces.push_back(Piece{ length, orientation, row * 6 + col, pieceMask });

	// Update the bitboards
	if (orientation == Vert) {
		m_vertical |= pieceMask;
	}
	else {
		m_horizontal |= pieceMask;
	}
}

// Always an horizontal piece with length 2
void Board::addMain(int row, int col)
{
	bitb pieceMask = 0;
	int pos = row * BOARD_SIZE + col;
	for (int i = 0; i < 2; i++) {
		pieceMask |= (bitb)1 << pos++;
	}
	m_pieces.insert(m_pieces.begin(), Piece{ 2, Horz, row * BOARD_SIZE + col, pieceMask });
	m_horizontal |= pieceMask;
}

// Print the current state of the board with regard to individual pieces
void Board::printBoard()
{
	string boardString;
	boardString.resize(BOARD_SIZE * BOARD_SIZE, '.');
	char id = 'a';

	for (const Piece& piece : m_pieces) {
		if (piece.m_pos == m_pieces[0].m_pos) {
			for (int i = 0; i < piece.m_length; i++) {
				boardString[piece.m_pos + i] = 'X';
			}
			continue;
		}
		for (int i = 0; i < piece.m_length; i++) {
			boardString[piece.m_pos + (i * piece.m_orient)] = id;
		}
		id++;
	}

	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			cout << boardString[i * BOARD_SIZE + j] << ' ';
		}
		cout << endl;
	}
}

// Simpler print with no regard to individual pieces, only the mask
void Board::printFlat()
{
	bitb mask = getMask();
	char c = 254u, dot = '.';
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			int pos = i * BOARD_SIZE + j;
			if (mask & ((bitb)1 << pos)) {
				cout << c << ' ';
			}
			else {
				cout << dot << ' ';
			}
		}
		cout << endl;
	}
}