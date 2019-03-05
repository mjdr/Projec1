#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <psapi.h>
#pragma comment(lib, "psapi")
using std::string;

class Memory
{
public:
	int GetProcessId(char* processName);
	unsigned long long GetModuleBase(HANDLE processHandle, string &sModuleName);
	BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);
	BOOL GetDebugPrivileges(void);
	int ReadInt(HANDLE processHandle, unsigned long long address);
	int GetPointerAddress(HANDLE processHandle, unsigned long long startAddress, int offsets[], int offsetCount);
	int ReadPointerInt(HANDLE processHandle, unsigned long long startAddress, int offsets[], int offsetCount);
	float ReadFloat(HANDLE processHandle, unsigned long long address);
	float ReadPointerFloat(HANDLE processHandle, unsigned long long startAddress, int offsets[], int offsetCount);
	const char* ReadText(HANDLE processHandle, unsigned long long address);
	const char* ReadPointerText(HANDLE processHandle, unsigned long long startAddress, int offsets[], int offsetCount);
};