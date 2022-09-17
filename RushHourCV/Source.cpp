#include "Process.h"
#include "Vision.h"
#include "Solver.h"

bool compareColAndRow(map<uchar, int>& m1, map<uchar, int>& m2);

int main(void)
{
	//Mat image = processToMat(L"scrcpy.exe");
	Mat image = imread("original.jpg");

	Rect boardCoords(Point(BOARD_TOPLEFT), Point(BOARD_BOTTOMRIGHT));	// TODO: Make this dynamic
	image = image(boardCoords);
	resize(image, image, Size(image.cols/2, image.rows/2), INTER_LINEAR);
	//imshow("window", image);
	//waitKey(0);

	double cropX = image.cols / (float)BOARD_SIZE;
	double cropY = image.rows / (float)BOARD_SIZE;
	cvtColor(image, image, COLOR_RGB2GRAY);

	char boardStatus[BOARD_SIZE][BOARD_SIZE]{};
	char currPieceMarker = 'B';
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (boardStatus[i][j]) {
				continue;
			}

			// Cutting a square from the image
			Mat square = image(Range(cropY * i, cropY * (i + 1)), Range(cropX * j, cropX * (j + 1)));

			// Getting the middle row and column
			Mat midColData = square.col(square.cols / 2);
			Mat midRowData = square.row(square.rows / 2);

			// Map and compare range of values?
			map<uchar, int> midColDataMap, midRowDataMap;
			for (int y = 0; y < square.rows; y++) {
				midColDataMap[midColData.at<uchar>(y, 0)]++;
			}
			for (int x = 0; x < square.cols; x++) {
				midRowDataMap[midRowData.at<uchar>(0, x)]++;
			}

			if (compareColAndRow(midColDataMap, midRowDataMap)) {
				int redRate = mapRangeFind(midRowDataMap, RED_PIXRANGE_START, RED_PIXRANGE_END);
				if (redRate > 3) {
					boardStatus[i][j] = 'A';
					boardStatus[i][j + 1] = 'A';
					continue;
				}
				else if (midColDataMap.size() < midRowDataMap.size()) {
					// Vertical Piece
					int orangeRate = mapRangeFind(midRowDataMap, ORANGE_PIXRANGE_START, ORANGE_PIXRANGE_END);
					int purpleRate = mapRangeFind(midRowDataMap, PURPLE_PIXRANGE_START, PURPLE_PIXRANGE_END);

					if (orangeRate > purpleRate) {
						// Orange piece
						boardStatus[i][j] = currPieceMarker;
						boardStatus[i + 1][j] = currPieceMarker;
					}
					else {
						// Purple piece
						boardStatus[i][j] = currPieceMarker;
						boardStatus[i + 1][j] = currPieceMarker;
						boardStatus[i + 2][j] = currPieceMarker;
					}
				}
				else {
					// Horizontal Piece
					int orangeRate = mapRangeFind(midColDataMap, ORANGE_PIXRANGE_START, ORANGE_PIXRANGE_END);
					int purpleRate = mapRangeFind(midColDataMap, PURPLE_PIXRANGE_START, PURPLE_PIXRANGE_END);

					if (orangeRate > purpleRate) {
						// Orange piece
						boardStatus[i][j] = currPieceMarker;
						boardStatus[i][j + 1] = currPieceMarker;
					}
					else {
						// Purple piece
						boardStatus[i][j] = currPieceMarker;
						boardStatus[i][j + 1] = currPieceMarker;
						boardStatus[i][j + 2] = currPieceMarker;
					}
				}
				++currPieceMarker;
			}
			else {
				boardStatus[i][j] = '.';
			}
		}
	}

	// Printing the board
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			cout << boardStatus[i][j];
		}
		cout << endl;
	}

	vector<Car> cars = parseBoard(boardStatus);

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