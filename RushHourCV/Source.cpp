#include "Process.h"
#include "Vision.h"
#include "Solver.h"
#include "Board.h"
#include <chrono>

bool compareColAndRow(map<uchar, int>& m1, map<uchar, int>& m2);

int main(void)
{
	Mat image = processToMat(L"scrcpy.exe");
	if (image.empty()) {
		return -1;
	}
	//Mat image = imread("original.jpg");		//! TEMP
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
	board.printFlat();
	system("PAUSE");

	// Solving the generated board
	Solver solver;
	auto t_start = chrono::system_clock::now();
	auto path = solver.solveBoard(board);
	chrono::duration<float> t_durr = chrono::system_clock::now() - t_start;
	if (!path.empty()) {
		for (Board& b : path) {
			b.printBoard();
			cout << endl;
		}
	}
	else {
		cout << "Couldn't find a solution. Something's wrong..." << endl;
	}
	cout << "Time taken: " << to_string(t_durr.count()) << " seconds" << endl;
	return 0;
}

// False if the maps match by at least 80% (empty tile)
// True otherwise (special tile)
bool compareColAndRow(map<uchar, int>& m1, map<uchar, int>& m2)
{
	int resemblance = 0;
	auto it1 = m1.begin();
	auto it2 = m2.begin();
	while (it1 != m1.end() && it2 != m2.end()) {
		if (it1->first < it2->first) {
			++it1;
		}
		else if (it1->first > it2->first) {
			++it2;
		}
		else if (it1->first == it2->first) {
			resemblance += it1->second < it2->second ? it1->second : it2->second;
			++it1, ++it2;
		}
	}

	size_t mapSize = m1.size() > m2.size() ? m1.size() : m2.size();
	if (resemblance > (mapSize * 0.8)) {
		return false;
	}
	return true;
}