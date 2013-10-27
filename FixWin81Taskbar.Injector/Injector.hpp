#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// should probably do some error checking
class Injector {
public:
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
};