#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

extern "C" __declspec(dllexport) LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= HC_ACTION && wParam == WM_LBUTTONUP && lParam) {
		auto msg = reinterpret_cast<MOUSEHOOKSTRUCT*>(lParam);
		auto hWnd = FindWindowW(L"Shell_SecondaryTrayWnd", nullptr);
		if (msg->hwnd == hWnd) {
			SetForegroundWindow(FindWindowW(L"Shell_TrayWnd", nullptr));
		}
	}

	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) LRESULT CALLBACK RedrawProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= HC_ACTION) {
		auto msg = reinterpret_cast<CWPSTRUCT*>(lParam);
		if ((msg->message == WM_WINDOWPOSCHANGING || msg->message == WM_WINDOWPOSCHANGED || msg->message == WM_NCPAINT)) {
			auto hWnd = FindWindowW(L"Shell_SecondaryTrayWnd", nullptr);
			hWnd = FindWindowExW(hWnd, nullptr, L"WorkerW", nullptr);

			if (msg->hwnd == hWnd) {
				SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, 1);
			}
		}
	}

	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	return TRUE;
}