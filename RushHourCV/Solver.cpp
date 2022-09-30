#include "Solver.h"

Solver::Solver()
{
	m_topRow = 0, m_bottomRow = 0, m_leftCol = 0, m_rightCol = 0;
	for (int col = 0; col < BOARD_SIZE; col++) {
		m_topRow |= (bitb)1 << (0 * BOARD_SIZE + col);
		m_bottomRow |= (bitb)1 << ((BOARD_SIZE - 1) * BOARD_SIZE + col);
	}
	for (int row = 0; row < BOARD_SIZE; row++) {
		m_leftCol |= (bitb)1 << (row * BOARD_SIZE);
		m_rightCol |= (bitb)1 << (row * BOARD_SIZE + (BOARD_SIZE - 1));
	}
}

// returns number of moves generated
int Solver::getNextMoves(Board& b)
{
	int generatedMoves = 0;
	bitb boardMask = b.getMask();
	for (const Piece& p : b.getPieces()) {
		if (p.m_orient == Vert) {
			// Vertical move upwards (shift right - negative)
			if ((p.m_mask & m_topRow) == 0) {
				bitb mask = p.m_mask;
				int steps = -1;
				do {
					mask = (mask >> Vert) & ~mask;
					if ((mask & boardMask) != 0) {
						break;
					}
					m_moves.emplace(Board(b, p, steps));
					++generatedMoves;
					--steps;
				} while ((mask & m_topRow) == 0);
			}
			// Vertical move downwards (shift left - positive)
			if ((p.m_mask & m_bottomRow) == 0) {
				bitb mask = p.m_mask;
				int steps = 1;
				do {
					mask = (mask << Vert) & ~mask;
					if ((mask & boardMask) != 0) {
						break;
					}
					m_moves.emplace(Board(b, p, steps));
					++generatedMoves;
					++steps;
				} while ((mask & m_bottomRow) == 0);
			}
		}
		else {
			// Horizontal move left (shift right - negative)
			if ((p.m_mask & m_leftCol) == 0) {
				bitb mask = p.m_mask;
				int steps = -1;
				do {
					mask = (mask >> Horz) & ~mask;
					if ((mask & boardMask) != 0) {
						break;
					}
					m_moves.emplace(Board(b, p, steps));
					++generatedMoves;
					--steps;
				} while ((mask & m_leftCol) == 0);
			}
			// Horizontal move right (shift left - positive)
			if ((p.m_mask & m_rightCol) == 0) {
				bitb mask = p.m_mask;
				int steps = 1;
				do {
					mask = (mask << Horz) & ~mask;
					if ((mask & boardMask) != 0) {
						break;
					}
					m_moves.emplace(Board(b, p, steps));
					++generatedMoves;
					++steps;
				} while ((mask & m_rightCol) == 0);
			}
		}
	}
	return generatedMoves;
}

vector<Board> Solver::solveBoard(const Board& root)
{
	vector<Board> path;
	set<pair<bitb, bitb>> discovered;
	
	// Pushing the root so it is saved in the path
	m_moves.push(root);
	while (!m_moves.empty()) {
		Board n = m_moves.front();
		m_moves.pop();
		if (!m_paths.empty()) {
			path = m_paths.front();
			m_paths.pop();
		}
		path.push_back(n);

		if (discovered.find(n.getComposition()) != discovered.end()) {
			continue;
		}
		discovered.insert(n.getComposition());

		if (n.isSolved()) {
			return path;
		}

		int moves = getNextMoves(n);
		for (int i = 0; i < moves; i++) {
			m_paths.push(path);
		}
	}
	return vector<Board>();
}