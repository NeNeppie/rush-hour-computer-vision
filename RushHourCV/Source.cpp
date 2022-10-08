#include "Vision.h"
#include "Exec.h"
#include "Solver.h"
#include "Board.h"
#include <chrono>
#include <thread>

Board constructBoard(Mat& image, double c);
void makeMove(adb& bridge, Board& bC, Board& bP, double c);

int main(int argc, char** argv)
{
	int currlvl, endlvl;
	switch (argc) {
	case 1:
		cout << "No additional arguments. Solving one board" << endl;
		currlvl = 0, endlvl = 0;
		break;
	case 2:
		cout << "Solving the next " << argv[1] << " boards" << endl;
		currlvl = 1, endlvl = atoi(argv[1]);
		break;
	case 3:
		currlvl = atoi(argv[1]), endlvl = atoi(argv[2]);
		break;
	default:
		cerr << "Too many arguments provided" << endl;
		return -1;
	}

	adb bridge;
	while (currlvl++ <= endlvl) {
		bridge.screencap();
		Mat image = getBoardImage("output.png");
		if (image.empty()) {
			return -1;
		}

		const double crop = image.cols / (float)BOARD_SIZE;
		Board board = constructBoard(image, crop);
		cout << "Generated board: " << endl;
		board.printBoard();

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
					continue;
				}
				path[i].printBoard();
				cout << endl;

				makeMove(bridge, path[i], path[i - 1], crop);
				if (i == 1) {
					bridge.execInput(BOARD_TOPLEFT_X, BOARD_TOPLEFT_Y);
					bridge.screencap();
					Mat img = getBoardImage("output.png");
					if (image.empty()) {
						return -1;
					}
					Board b = constructBoard(img, crop);
					// Retrying because the first swipe very randomly refuses to register
					if (b.getComposition() != path[i].getComposition()) {
						cout << "Failed to execute move, retrying..." << endl << endl;
						makeMove(bridge, path[i], path[i - 1], crop);
					}
				}
			}
		}
		else {
			cerr << "Error finding solution" << endl;
			return -1;
		}

		// Next level
		this_thread::sleep_for(chrono::milliseconds(1000));
		bridge.execInput(530, 1900);
	}
	return 0;
}

Board constructBoard(Mat& image, double c)
{
	Board board{};
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board.getMask() & (bitb)1 << (i * 6 + j)) {
				continue;
			}

			// Cutting a smaller square
			Mat square = image(Range(c * i, c * (i + 1)), Range(c * j, c * (j + 1)));
			Mat midColData = square.col(square.cols / 2);
			Mat midRowData = square.row(square.rows / 2);

			map<uchar, int> midColDataMap, midRowDataMap;
			for (int p = 0; p < square.rows; p++) {
				midColDataMap[midColData.at<uchar>(p, 0)]++;
				midRowDataMap[midRowData.at<uchar>(0, p)]++;
			}

			if (compareColAndRow(midColDataMap, midRowDataMap)) {
				// Main piece (The "red car")
				if (mapRangeFind(midRowDataMap, RED_PIXRANGE_START, RED_PIXRANGE_END) > (c / BOARD_SIZE)) {
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
	return board;
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