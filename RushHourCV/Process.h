#pragma once
#include <windows.h>
#include "stdafx.h"
#include <TlHelp32.h>

BOOL CALLBACK enumwindows_callback(HWND handle, LPARAM param);
DWORD GetProcID(LPCWSTR procName);
Mat hwnd2mat(HWND hwnd); // https://stackoverflow.com/questions/50395294/c-window-capture-output-isnt-the-same-size-as-said-window
Mat processToMat(LPCWSTR procName);

struct handle_data {
	DWORD procId;
	HWND windowHandle;
};