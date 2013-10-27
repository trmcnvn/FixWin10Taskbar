#include <iostream>
#include <string>
#include <codecvt>
#include <locale>

#include "Injector.hpp"


int main(int argc, char** argv) {
	// Add this program to startup
	HKEY regKey;
	const std::wstring subKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	const std::wstring appName = L"FixWin81Taskbar";

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring appPath = converter.from_bytes(argv[0]);

	RegOpenKeyExW(HKEY_CURRENT_USER, subKey.data(), 0, KEY_ALL_ACCESS, &regKey);
	RegSetValueExW(regKey, appName.data(), 0, REG_SZ, reinterpret_cast<const BYTE*>(appPath.data()), appPath.size() * 2);
	RegCloseKey(regKey);

	appPath = appPath.substr(0, appPath.find_last_of('\\'));
	appPath.append(L"\\FixWin81Taskbar.Module.dll");

	// Inject into explorer.exe
	auto inj = new Injector();
	inj->Inject(L"Shell_SecondaryTrayWnd", appPath);
	delete inj;
}