#include "Vision.h"
#include "Exec.h"
#include "Solver.h"
#include "Board.h"
#include <chrono>
#include <thread>

void makeMove(adb& bridge, Board& bC, Board& bP, double c);

int main(void)
{
	adb bridge;
	bridge.screencap();
	Mat image = imread("output.png");
	if (image.empty()) {
		cerr << "Error getting screencap." << endl;
		return -1;
	}
	focusOnBoard(image);

	const double crop = image.cols / (float)BOARD_SIZE;
	Board board{};
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board.getMask() & (bitb)1 << (i * 6 + j)) {
				continue;
			}

			// Cutting a smaller square
			Mat square = image(Range(crop * i, crop * (i + 1)), Range(crop * j, crop * (j + 1)));
			Mat midColData = square.col(square.cols / 2);
			Mat midRowData = square.row(square.rows / 2);

			map<uchar, int> midColDataMap, midRowDataMap;
			for (int p = 0; p < square.rows; p++) {
				midColDataMap[midColData.at<uchar>(p, 0)]++;
				midRowDataMap[midRowData.at<uchar>(0, p)]++;
			}

			if (compareColAndRow(midColDataMap, midRowDataMap)) {
				// Main piece (The "red car")
				if (mapRangeFind(midRowDataMap, RED_PIXRANGE_START, RED_PIXRANGE_END) > 3) {
					board.addMain(i, j);
					continue;
				}
				else if (midColDataMap.size() < midRowDataMap.size()) {
					// Vertical piece
					if (mapRangeFind(midRowDataMap, ORANGE_PIXRANGE_START, ORANGE_PIXRANGE_END) >
						mapRangeFind(midRowDataMap, PURPLE_PIXRANGE_START, PURPLE_PIXRANGE_END)) {
						board.addPiece(i, j, 2, Vert);
						continue;
					}
					board.addPiece(i, j, 3, Vert);
				}
				else {
					// Horizontal piece
					if (mapRangeFind(midColDataMap, ORANGE_PIXRANGE_START, ORANGE_PIXRANGE_END) >
						mapRangeFind(midColDataMap, PURPLE_PIXRANGE_START, PURPLE_PIXRANGE_END)) {
						board.addPiece(i, j, 2, Horz);
						continue;
					}
					board.addPiece(i, j, 3, Horz);
				}
			}
		}
	}

	cout << "Generated board: " << endl;
	board.printBoard();
	system("PAUSE");

	// Solving the generated board
	Solver solver;
	auto t_start = chrono::system_clock::now();
	cout << "Solving board..." << endl;
	vector<Board> path = solver.solveBoard(board);
	chrono::duration<float> t_durr = chrono::system_clock::now() - t_start;
	cout << "Time taken: " << to_string(t_durr.count()) << " seconds" << endl;

	if (!path.empty()) {
		cout << "Executing moves" << endl;
		for (int i = 0; i < path.size(); i++) {
			if (path[i].getMask() == path[0].getMask()) {
				bridge.execInput(BOARD_TOPLEFT_X + crop, BOARD_TOPLEFT_Y + crop);
				continue;
			}
			path[i].printBoard();
			cout << endl;

			makeMove(bridge, path[i], path[i - 1], crop);
		}
	}
	else {
		cerr << "Error finding solution" << endl;
	}
	
	system("PAUSE");
	return 0;
}

void makeMove(adb& bridge, Board& bC, Board& bP, double c)
{
	auto itCurr = bC.getPieces().begin();
	auto itPrev = bP.getPieces().begin();
	while (itCurr != bC.getPieces().end()) {
		int source = itPrev->m_pos;
		int destination = itCurr->m_pos;
		if (source != destination) {
			int srcCol = source % BOARD_SIZE, srcRow = source / BOARD_SIZE;
			int dstCol = destination % BOARD_SIZE, dstRow = destination / BOARD_SIZE;
			int srcX = (c * 2 * srcCol + c) + BOARD_TOPLEFT_X;
			int srcY = (c * 2 * srcRow + c) + BOARD_TOPLEFT_Y;
			int dstX = (c * 2 * dstCol + c) + BOARD_TOPLEFT_X;
			int dstY = (c * 2 * dstRow + c) + BOARD_TOPLEFT_Y;

			bridge.execInput(srcX, srcY, dstX, dstY,
				300 * (abs(srcCol - dstCol) + abs(srcRow - dstRow)));
		}
		++itCurr;
		++itPrev;
	}
}