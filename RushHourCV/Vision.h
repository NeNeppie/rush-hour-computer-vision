#pragma once
#include "stdafx.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

#define BOARD_TOPLEFT		143, 823	// TODO: Calculate board coordinates dynamically
#define BOARD_BOTTOMRIGHT	934, 1614	// Instead of doing it manually
#define BOARD_TOPLEFT_X		143
#define BOARD_TOPLEFT_Y		823

// Ad skip buttons
// 70, 150 - 1000, 160
// Next level button
// 530, 2000

// Pixel value ranges
#define ORANGE_PIXRANGE_START	167
#define ORANGE_PIXRANGE_END		169

#define PURPLE_PIXRANGE_START	170
#define PURPLE_PIXRANGE_END		172

#define RED_PIXRANGE_START		32
#define RED_PIXRANGE_END		35

void focusOnBoard(Mat& image);
bool compareColAndRow(std::map<uchar, int>& m1, std::map<uchar, int>& m2);
int mapRangeFind(std::map<uchar, int> map, int start, int end);