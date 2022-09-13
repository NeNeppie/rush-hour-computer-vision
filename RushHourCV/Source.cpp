#include "Process.h"
#include "Imgproc.h"
#include <map>

#define BOARD_SIZE 36	// 6 * 6

/*
Orange Piece:
Middle - 212, 213, 214
Edges - 167, 168, 169

Purple Piece:
Middle - 212, 213, 214
Edges - 170, 171, 172

Player Hat - 32, 33, 34
*/

bool compareColAndRow(map<uchar, int>& m1, map<uchar, int>& m2);

int main(void)
{
	Mat image = processToMat(L"scrcpy.exe");
	//Mat image = imread("original.jpg");

	Rect boardCoords(Point(BOARD_TOPLEFT), Point(BOARD_BOTTOMRIGHT));
	image = image(boardCoords);
	resize(image, image, Size(image.cols, image.rows), INTER_LINEAR);
	resize(image, image, Size(image.cols/2, image.rows/2), INTER_LINEAR);
	//imshow("window", image);
	//waitKey(0);

	Mat boardSquares[36];
	char boardStatus[36]{};
	char currPieceMarker = 'B';
	double cropX = image.cols / 6.0;
	double cropY = image.rows / 6.0;
	cvtColor(image, image, COLOR_RGB2GRAY);
	for (int y = 0; y < 6; y++) {
		for (int x = 0; x < 6; x++) {
			boardSquares[6 * y + x] = image(Range(cropY * y, cropY * (y + 1)), Range(cropX * x, cropX * (x + 1)));
		}
	}

	int squareMidRow = boardSquares[0].rows / 2;
	int squareMidCol = boardSquares[0].cols / 2;
	for (int i = 0; i < 36; i++) {
		if (boardStatus[i]) {
			continue;
		}

		Mat midColData = boardSquares[i].col(squareMidCol);
		Mat midRowData = boardSquares[i].row(squareMidRow);

		// Map and compare range of values?
		map<uchar, int> midColDataMap, midRowDataMap;
		for (int y = 0; y < boardSquares[i].rows; y++) {
			midColDataMap[midColData.at<uchar>(y, 0)]++;
		}
		for (int x = 0; x < boardSquares[i].cols; x++) {
			midRowDataMap[midRowData.at<uchar>(0, x)]++;
		}

		if (compareColAndRow(midColDataMap, midRowDataMap)) {
			int redRate = 0;
			for (uchar j = 32; j < 35; j++) {
				auto searchIt = midRowDataMap.find(j);
				if (searchIt != midRowDataMap.end()) {
					redRate += searchIt->second;
				}
			}
			if (redRate > 3) {
				boardStatus[i] = 'A';
				boardStatus[i + 1] = 'A';
				continue;
			}
			else if (midColDataMap.size() < midRowDataMap.size()) {
				// Vertical Piece
				int orangeRate = 0, purpleRate = 0;
				for (uchar j = 167; j <= 169; j++) {
					auto searchIt = midRowDataMap.find(j);
					if (searchIt != midRowDataMap.end()) {
						orangeRate += searchIt->second;
					}
				}
				for (uchar j = 170; j <= 172; j++) {
					auto searchIt = midRowDataMap.find(j);
					if (searchIt != midRowDataMap.end()) {
						purpleRate += searchIt->second;
					}
				}

				if (orangeRate > purpleRate) {
					// Orange piece
					boardStatus[i] = currPieceMarker;
					boardStatus[i + 6] = currPieceMarker;
				}
				else {
					// Purple piece
					boardStatus[i] = currPieceMarker;
					boardStatus[i + 6] = currPieceMarker;
					boardStatus[i + 12] = currPieceMarker;
				}
			}
			else {
				// Horizontal Piece
				int orangeRate = 0, purpleRate = 0;
				for (uchar j = 167; j <= 169; j++) {
					auto searchIt = midColDataMap.find(j);
					if (searchIt != midColDataMap.end()) {
						orangeRate += searchIt->second;
					}
				}
				for (uchar j = 170; j <= 172; j++) {
					auto searchIt = midColDataMap.find(j);
					if (searchIt != midColDataMap.end()) {
						purpleRate += searchIt->second;
					}
				}

				if (orangeRate > purpleRate) {
					// Orange piece
					boardStatus[i] = currPieceMarker;
					boardStatus[i + 1] = currPieceMarker;
				}
				else {
					// Purple piece
					boardStatus[i] = currPieceMarker;
					boardStatus[i + 1] = currPieceMarker;
					boardStatus[i + 2] = currPieceMarker;
				}
			}
			++currPieceMarker;
		}
		else {
			boardStatus[i] = '.';
		}
	}

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			cout << boardStatus[6 * i + j];
		}
		cout << endl;
	}

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