#include "Process.h"

BOOL CALLBACK enumwindows_callback(HWND handle, LPARAM param)
{
	handle_data& data = *(handle_data*)param;
	DWORD process_id;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.procId == process_id) {
		data.windowHandle = handle;
		return FALSE;
	}
	return TRUE;
}

DWORD GetProcID(LPCWSTR procName)
{
	// Get a snapshot of the process list
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 entry{};
	entry.dwSize = sizeof(PROCESSENTRY32);

	// Going through processes to find the one we need
	if (Process32First(hSnapshot, &entry)) {
		while (Process32Next(hSnapshot, &entry)) {
			if (wcscmp(entry.szExeFile, procName) == 0) {
				CloseHandle(hSnapshot);
				return entry.th32ProcessID;
			}
		}
	}
	CloseHandle(hSnapshot);
	return NULL;
}

// https://stackoverflow.com/questions/50395294/c-window-capture-output-isnt-the-same-size-as-said-window
Mat hwnd2mat(HWND hwnd)
{
	RECT rc;
	GetClientRect(hwnd, &rc);
	int width = rc.right;
	int height = rc.bottom;

	Mat src;
	src.create(height, width, CV_8UC4);

	HDC hdc = GetDC(hwnd);
	HDC memdc = CreateCompatibleDC(hdc);
	HBITMAP hbitmap = CreateCompatibleBitmap(hdc, width, height);
	HBITMAP oldbmp = (HBITMAP)SelectObject(memdc, hbitmap);

	BitBlt(memdc, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
	SelectObject(memdc, oldbmp);

	BITMAPINFOHEADER  bi = { sizeof(BITMAPINFOHEADER), width, -height, 1, 32, BI_RGB };
	GetDIBits(hdc, hbitmap, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	DeleteObject(hbitmap);
	DeleteDC(memdc);
	ReleaseDC(hwnd, hdc);

	return src;
}

Mat processToMat(LPCWSTR procName)
{
	SetProcessDPIAware();
	
	// Find and open scrcpy process
	DWORD scrcpyProcId = GetProcID(procName);
	handle_data data{ scrcpyProcId, 0 };

	// Find the correlating window
	EnumWindows(enumwindows_callback, (LPARAM)&data);
	if (data.windowHandle == 0) {
		cerr << "scrcpy window not found" << endl;
		return Mat{};
	}

	Mat frame = hwnd2mat(data.windowHandle);
	return frame;
}