#pragma once
#include "stdafx.h"

#define BOARD_TOPLEFT		54, 317		// TODO: Calculate board coordinates dynamically
#define BOARD_BOTTOMRIGHT	359, 622	// Instead of doing it manually

// Pixel value ranges
#define ORANGE_PIXRANGE_START	167
#define ORANGE_PIXRANGE_END		169

#define PURPLE_PIXRANGE_START	170
#define PURPLE_PIXRANGE_END		172

#define RED_PIXRANGE_START		32
#define RED_PIXRANGE_END		35

template <typename T1, typename T2>
T2 mapRangeFind(std::map<T1, T2> map, T2 start, T2 end)
{
	T2 counterterrorist = 0;
	for (T1 i = start; i <= end; ++i) {
		auto it = map.find(i);
		if (it != map.end()) {
			counterterrorist += it->second;
		}
	}
	return counterterrorist;
}