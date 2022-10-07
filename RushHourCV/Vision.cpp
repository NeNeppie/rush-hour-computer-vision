#include "Vision.h"

Mat getBoardImage(string path)
{
	Mat image = imread(path);
	if (image.empty()) {
		cerr << "Error getting screencap." << endl;
		return Mat{};
	}

	// TODO: Make this dynamic
	// Crop the image to fit only the game board
	Rect boardCoords(Point(BOARD_TOPLEFT), Point(BOARD_BOTTOMRIGHT));
	image = image(boardCoords);
	resize(image, image, Size(image.cols / 2, image.rows / 2), INTER_LINEAR);
	cvtColor(image, image, COLOR_RGB2GRAY);
	return image;
}

// False if the maps match by at least 80% (empty tile)
// True otherwise (special tile)
bool compareColAndRow(std::map<uchar, int>& m1, std::map<uchar, int>& m2)
{
	int resemblance = 0, count = 0;
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

	for (auto it = m1.begin(); it != m1.end(); ++it) {
		count += it->second;
	}
	if (resemblance > (count * 0.8)) {
		return false;
	}
	return true;
}

int mapRangeFind(std::map<uchar, int> map, int start, int end)
{
	int counter = 0;
	for (uchar i = start; i <= end; ++i) {
		auto it = map.find(i);
		if (it != map.end()) {
			counter += it->second;
		}
	}
	return counter;
}