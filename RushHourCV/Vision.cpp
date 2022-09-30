#include "Vision.h"

// Crop the image to fit only the game board
void focusOnBoard(Mat& image)
{
	// TODO: Make this dynamic
	Rect boardCoords(Point(BOARD_TOPLEFT), Point(BOARD_BOTTOMRIGHT));
	image = image(boardCoords);
	resize(image, image, Size(image.cols / 2, image.rows / 2), INTER_LINEAR);
	cvtColor(image, image, COLOR_RGB2GRAY);
}