#include "Vision.h"
#include "Solver.h"
#include "Board.h"
#include <chrono>
#include <thread>

int main(void)
{
	cout << "Starting ADB server..." << endl;
	system("adb.exe start-server");
	system("adb.exe exec-out screencap -p > output.png");
	Mat image = imread("output.png");
	if (image.empty()) {
		cerr << "Error getting screencap." << endl;
		system("adb.exe kill-server");
		return -1;
	}
	focusOnBoard(image);

	const double cropX = image.cols / (float)BOARD_SIZE;
	const double cropY = image.rows / (float)BOARD_SIZE;

	Board board{};
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board.getMask() & (uint64_t)1 << (i * 6 + j)) {
				continue;
			}

			// Cutting a square from the image
			Mat square = image(Range(cropY * i, cropY * (i + 1)), Range(cropX * j, cropX * (j + 1)));
			// Getting the middle row and column
			Mat midColData = square.col(square.cols / 2);
			Mat midRowData = square.row(square.rows / 2);

			// Map and later on compare pixel data
			map<uchar, int> midColDataMap, midRowDataMap;
			for (int y = 0; y < square.rows; y++) {
				midColDataMap[midColData.at<uchar>(y, 0)]++;
			}
			for (int x = 0; x < square.cols; x++) {
				midRowDataMap[midRowData.at<uchar>(0, x)]++;
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
					}
					else {
						board.addPiece(i, j, 3, Vert);
					}
				}
				else {
					// Horizontal piece
					if (mapRangeFind(midColDataMap, ORANGE_PIXRANGE_START, ORANGE_PIXRANGE_END) >
						mapRangeFind(midColDataMap, PURPLE_PIXRANGE_START, PURPLE_PIXRANGE_END)) {
						board.addPiece(i, j, 2, Horz);
					}
					else {
						board.addPiece(i, j, 3, Horz);
					}
				}
			}
		}
	}

	cout << "Generated board: " << endl;
	board.printBoard();
	system("PAUSE");

	// Solving the generated board
	cout << "Starting ADB server." << endl;
	//system("scrcpy-win64-v1.24\\adb.exe start-server");
	Solver solver;
	auto t_start = chrono::system_clock::now();
	cout << "Solving board..." << endl;
	vector<Board> path = solver.solveBoard(board);
	chrono::duration<float> t_durr = chrono::system_clock::now() - t_start;
	cout << "Time taken: " << to_string(t_durr.count()) << " seconds" << endl;
	if (!path.empty()) {
		for (int i = 0; i < path.size(); i++) {
			if (path[i].getMask() == path[0].getMask()) {
				this_thread::sleep_for(chrono::milliseconds(500));
				string shellCommand = "adb.exe shell input tap " +
					to_string(BOARD_TOPLEFT_X) + ' ' + to_string(BOARD_TOPLEFT_Y);
				system(shellCommand.c_str());
				continue;
			}
			path[i].printBoard();
			cout << endl;

			auto itCurr = path[i].getPieces().begin();
			auto itPrev = path[i - 1].getPieces().begin();
			while (itCurr != path[i].getPieces().end()) {
				int source = itPrev->m_pos;
				int destination = itCurr->m_pos;
				if (source != destination) {
					// '20' is a random value just so we're not right on the edge
					int srcCol = source % BOARD_SIZE, srcRow = source / BOARD_SIZE;
					int dstCol = destination % BOARD_SIZE, dstRow = destination / BOARD_SIZE;
					int srcX = (cropX * 2 * srcCol + cropX) + BOARD_TOPLEFT_X;
					int srcY = (cropY * 2 * srcRow + cropY) + BOARD_TOPLEFT_Y;
					int dstX = (cropX * 2 * dstCol + cropX) + BOARD_TOPLEFT_X;
					int dstY = (cropY * 2 * dstRow + cropY) + BOARD_TOPLEFT_Y;

					string shellCommand = "adb.exe shell input swipe " +
						to_string(srcX) + ' ' + to_string(srcY) + ' ' +
						to_string(dstX) + ' ' + to_string(dstY) + ' ' + 
						to_string(300 * (abs(srcCol - dstCol) + abs(srcRow - dstRow)));	// duration of the swipe
					system(shellCommand.c_str());
					this_thread::sleep_for(chrono::milliseconds(50));
				}
				++itCurr;
				++itPrev;
			}
		}
	}
	else {
		cout << "Couldn't find a solution. Something's wrong..." << endl;
	}
	cout << "Stopping ADB server." << endl;
	system("adb.exe kill-server");
	system("PAUSE");
	return 0;
}