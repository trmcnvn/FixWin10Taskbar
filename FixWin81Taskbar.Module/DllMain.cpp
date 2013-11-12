#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdio>
#include <iostream>

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION && wParam == WM_LBUTTONUP && lParam) {
		auto msg = reinterpret_cast<MOUSEHOOKSTRUCT*>(lParam);

		auto hWnd = FindWindowW(L"Shell_SecondaryTrayWnd", NULL);
		if (msg->hwnd == hWnd) {
			SetForegroundWindow(FindWindowW(L"Shell_TrayWnd", NULL));
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

DWORD WINAPI dwThread(LPVOID lpParam) {
	auto msgHook = SetWindowsHookExW(WH_MOUSE, MouseProc, reinterpret_cast<HINSTANCE>(lpParam), 0);
	
	MSG messages;
	while (GetMessageW(&messages, NULL, 0, 0)) {
		TranslateMessage(&messages);
		DispatchMessageW(&messages);
	}

	UnhookWindowsHookEx(msgHook);
	return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		auto thread = CreateThread(NULL, NULL, dwThread, hInstDLL, NULL, NULL);
		CloseHandle(thread);
	}
	return TRUE;
}