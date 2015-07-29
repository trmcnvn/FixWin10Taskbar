#include <iostream>
#include <string>
#include <codecvt>
#include <locale>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

int main(int argc, char** argv) {
	// Add this program to startup
	HKEY regKey;
	const std::wstring subKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	const std::wstring appName = L"FixWin10Taskbar";

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring appPath = converter.from_bytes(argv[0]);

	RegOpenKeyExW(HKEY_CURRENT_USER, subKey.data(), 0, KEY_ALL_ACCESS, &regKey);
	RegSetValueExW(regKey, appName.data(), 0, REG_SZ, reinterpret_cast<const BYTE*>(appPath.data()), appPath.size() * 2);
	RegCloseKey(regKey);

	appPath = appPath.substr(0, appPath.find_last_of('\\'));
	appPath.append(L"\\FixWin10Taskbar.Module.dll");

	// Hide the console window
	ShowWindow(GetForegroundWindow(), SW_HIDE);

	// Hide buttons
	auto hWnd = FindWindowW(L"Shell_SecondaryTrayWnd", nullptr);
	ShowWindow(FindWindowExW(hWnd, nullptr, L"Start", nullptr), SW_HIDE);
	ShowWindow(FindWindowExW(hWnd, nullptr, L"TrayButton", L"Search Windows"), SW_HIDE);
	ShowWindow(FindWindowExW(hWnd, nullptr, L"TrayButton", L"Task View"), SW_HIDE);

	// Some hooks
	auto dllHandle = LoadLibraryW(L"FixWin10Taskbar.Module.dll");
	auto threadId = GetWindowThreadProcessId(hWnd, nullptr);
	auto mouseProc = GetProcAddress(dllHandle, "MouseProc");
	auto redrawProc = GetProcAddress(dllHandle, "RedrawProc");

	SetWindowsHookExW(WH_MOUSE, reinterpret_cast<HOOKPROC>(mouseProc), dllHandle, threadId);
	SetWindowsHookExW(WH_CALLWNDPROC, reinterpret_cast<HOOKPROC>(redrawProc), dllHandle, threadId);

	Sleep(INFINITE);
}