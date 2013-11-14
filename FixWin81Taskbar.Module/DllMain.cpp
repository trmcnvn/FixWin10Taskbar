#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdio>
#include <iostream>
#include <vector>

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= HC_ACTION && wParam == WM_LBUTTONUP && lParam) {
		auto msg = reinterpret_cast<MOUSEHOOKSTRUCT*>(lParam);

		auto hWnd = FindWindowW(L"Shell_SecondaryTrayWnd", NULL);
		if (msg->hwnd == hWnd) {
			SetForegroundWindow(FindWindowW(L"Shell_TrayWnd", NULL));
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK RedrawProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= HC_ACTION) {
		auto msg = reinterpret_cast<CWPSTRUCT*>(lParam);
		if ((msg->message == WM_WINDOWPOSCHANGING || msg->message == WM_WINDOWPOSCHANGED || msg->message == WM_NCPAINT)) {
			auto hWnd = FindWindowW(L"Shell_SecondaryTrayWnd", NULL);
			hWnd = FindWindowExW(hWnd, nullptr, L"WorkerW", nullptr);

			if (msg->hwnd == hWnd) {
				SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, 1);
			}
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

DWORD WINAPI dwThread(LPVOID lpParam) {
	// Fuck off if you aren't explorer.exe
	wchar_t fileName[MAX_PATH];
	GetModuleFileNameW(NULL, fileName, MAX_PATH);
	if (wcsstr(fileName, L"xplorer.") == NULL) {
		FreeLibraryAndExitThread(LoadLibraryW(L"FixWin81Taskbar.Module.dll"), 0);
	}

	// initially hide the start button
	auto hWnd = FindWindowW(L"Shell_SecondaryTrayWnd", nullptr);
	hWnd = FindWindowExW(hWnd, nullptr, L"Start", nullptr);
	ShowWindow(hWnd, SW_HIDE);

	SetWindowsHookExW(WH_MOUSE, MouseProc, reinterpret_cast<HINSTANCE>(lpParam), 0);
	SetWindowsHookExW(WH_CALLWNDPROC, RedrawProc, reinterpret_cast<HINSTANCE>(lpParam), 0);
	
	MSG messages;
	while (GetMessageW(&messages, NULL, 0, 0)) {
		TranslateMessage(&messages);
		DispatchMessageW(&messages);
	}

	return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		auto thread = CreateThread(NULL, NULL, dwThread, hInstDLL, NULL, NULL);
		CloseHandle(thread);
	}
	return TRUE;
}