#include "Exec.h"

adb::adb()
{
	m_path = string(ADB_PATH) + ' ';
	cout << "Starting ADB server..." << endl;
	system(string(m_path + "start-server").c_str());
}

adb::~adb()
{
	cout << "Stopping ADB server." << endl;
	system(string(m_path + "kill-server").c_str());
}

void adb::execInput(int x, int y)
{
	string shellCommand = "shell input tap " +
		to_string(x) + ' ' + to_string(y);
	system((m_path + shellCommand).c_str());
	this_thread::sleep_for(chrono::milliseconds(50));
}

void adb::execInput(int x1, int y1, int x2, int y2, int duration)
{
	string shellCommand = "shell input swipe " +
		to_string(x1) + ' ' + to_string(y1) + ' ' +
		to_string(x2) + ' ' + to_string(y2) + ' ' + to_string(duration);
	system((m_path + shellCommand).c_str());
	this_thread::sleep_for(chrono::milliseconds(50));

}