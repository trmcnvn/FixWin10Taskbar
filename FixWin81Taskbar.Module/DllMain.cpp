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
		auto hWnd = FindWindowW(L"Shell_SecondaryTrayWnd", NULL);
		hWnd = FindWindowExW(hWnd, nullptr, L"WorkerW", nullptr);

		if ((msg->message == WM_WINDOWPOSCHANGING || msg->message == WM_WINDOWPOSCHANGED || msg->message == WM_NCPAINT) && msg->hwnd == hWnd) {
			RECT rect;
			GetWindowRect(hWnd, &rect);
			SetWindowPos(hWnd, nullptr, 0, 0, rect.right, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DWORD WINAPI dwThread(LPVOID lpParam) {
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
		// Fuck off if you aren't explorer.exe
		std::vector<wchar_t> fileNameVec(MAX_PATH);
		GetModuleFileNameW(NULL, &fileNameVec[0], MAX_PATH);
		std::wstring fileName(fileNameVec.begin(), fileNameVec.end());
		if (fileName.find(L"Explorer") == std::wstring::npos && fileName.find(L"explorer") == std::wstring::npos)
			return FALSE;

		auto thread = CreateThread(NULL, NULL, dwThread, hInstDLL, NULL, NULL);
		CloseHandle(thread);
	}
	return TRUE;
}