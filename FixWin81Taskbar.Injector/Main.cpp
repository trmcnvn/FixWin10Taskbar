#include <iostream>
#include <string>
#include <codecvt>
#include <locale>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void Inject(const std::wstring& className, const std::wstring& source) {
	DWORD procId;
	GetWindowThreadProcessId(FindWindowW(className.c_str(), NULL), &procId);
	auto procHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD |
		PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_VM_OPERATION, FALSE, procId);

	auto addr = VirtualAllocEx(procHandle, NULL, source.size() * 2, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	auto ret = WriteProcessMemory(procHandle, addr, source.data(), source.size() * 2, NULL);

	auto libaddr = GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");
	auto thread = CreateRemoteThread(procHandle, NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(libaddr), addr, NULL, NULL);

	WaitForSingleObject(thread, INFINITE);
	VirtualFreeEx(procHandle, addr, source.size() * 2, MEM_RELEASE);
	CloseHandle(thread);
	CloseHandle(procHandle);
}


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
	Inject(L"Shell_SecondaryTrayWnd", appPath);
}