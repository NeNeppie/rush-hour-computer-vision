#pragma once
#include "stdafx.h"
#include <thread>

#define ADB_PATH "..\\adb.exe"

class adb
{
public:
	adb();
	~adb();

	void screencap() {
		system(string(m_path + "exec-out screencap -p > output.png").c_str());
	}

	void execInput(int x, int y);
	void execInput(int x1, int y1, int x2, int y2, int duration = 500);

private:
	string m_path;
};