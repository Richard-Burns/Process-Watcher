#include "stdafx.h"
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <atlstr.h> 

using namespace std;

DWORD FindProcessId(const std::wstring& processName);

std::wstring s2ws(const std::string& s) {
	int slength = (int)s.length() + 1;
	int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	std::wstring r(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
	r.resize(r.size() - 1);
	return r;
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		return 0;
	}

	std::cout << "starting Process Watcher" << std::endl;
	std::cout << "monitoring process name: " << argv[1] << std::endl;
	std::cout << "file to launch on process closing: " << argv[2] << std::endl;

	std::wstring procName = s2ws(argv[1]);

	while (true) {
		if (!FindProcessId(procName)) {
			std::cout << argv[1] << " has quit, relaunching..." << std::endl;
			CString str = argv[2];
			CString action = "open";
			ShellExecute(NULL, action, str, NULL, NULL, SW_SHOW);
		}
	}

	return 0;
}


DWORD FindProcessId(const std::wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}