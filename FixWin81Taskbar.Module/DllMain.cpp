#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	LPCWPRETSTRUCT msg = reinterpret_cast<LPCWPRETSTRUCT>(lParam);

	if (msg->message == WM_LBUTTONUP) {
		auto hWnd = FindWindowW(L"Shell_TrayWnd", NULL);
		ShowWindow(hWnd, SW_SHOW);
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	HHOOK msgHook = nullptr;

	if (fdwReason == DLL_PROCESS_ATTACH)
		msgHook = SetWindowsHookExW(WH_CALLWNDPROCRET, HookProc, hInstDLL, 0);
	else if (fdwReason == DLL_PROCESS_DETACH)
		if (msgHook != nullptr)
			UnhookWindowsHookEx(msgHook);

	return TRUE;
}