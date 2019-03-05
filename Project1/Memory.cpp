#include "Memory.hpp"

int Memory::GetProcessId(char* processName) {
	SetLastError(0);
	PROCESSENTRY32 pe32;
	HANDLE hSnapshot = NULL;
	GetLastError();
	pe32.dwSize = sizeof(PROCESSENTRY32);
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);


	string p(processName);

	if (Process32First(hSnapshot, &pe32)) {
		do {
			std::wstring w(pe32.szExeFile);;
			string str(w.begin(), w.end());
			if (p == str)
				break;
		} while (Process32Next(hSnapshot, &pe32));
	}

	if (hSnapshot != INVALID_HANDLE_VALUE)
		CloseHandle(hSnapshot);
	int err = GetLastError();
	//std::cout << err << std::endl;
	if (err != 0)
		return 0;
	return pe32.th32ProcessID;
}
unsigned long long Memory::GetModuleBase(HANDLE processHandle, string &sModuleName)
{
	HMODULE *hModules = NULL;
	WCHAR szBuf[50];
	DWORD cModules;
	unsigned long long dwBase = -1;


	EnumProcessModules(processHandle, hModules, 0, &cModules);
	hModules = new HMODULE[cModules / sizeof(HMODULE)];

	if (EnumProcessModules(processHandle, hModules, cModules / sizeof(HMODULE), &cModules)) {
		for (size_t i = 0; i < cModules / sizeof(HMODULE); i++) {
			
			if (GetModuleBaseName(processHandle, hModules[i], szBuf, sizeof(szBuf))) {


				std::wstring w(szBuf);
				string str(w.begin(), w.end());
				if (sModuleName.compare(str) == 0) {
					dwBase = (unsigned long long)hModules[i];
					break;
				}
			}
		}
	}

	delete[] hModules;

	return dwBase;
}
BOOL Memory::SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) {
		//printf("LookupPrivilegeValue error: %u\n", GetLastError() );
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL)) {
		//printf("AdjustTokenPrivileges error: %u\n", GetLastError() );
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
		return FALSE;
	}

	return TRUE;
}
BOOL Memory::GetDebugPrivileges(void) {
	HANDLE hToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
		return FALSE; 

	if (!SetPrivilege(hToken, SE_DEBUG_NAME, TRUE))
		return FALSE; 

	return TRUE;
}
int Memory::ReadInt(HANDLE processHandle, unsigned long long address) {
	if (address == -1)
		return -1;
	int buffer = 0;
	SIZE_T NumberOfBytesToRead = sizeof(buffer); //this is equal to 4
	SIZE_T NumberOfBytesActuallyRead;
	BOOL success = ReadProcessMemory(processHandle, (LPCVOID)address, &buffer, NumberOfBytesToRead, &NumberOfBytesActuallyRead);
	if (!success || NumberOfBytesActuallyRead != NumberOfBytesToRead) {
		printf("Memory Error!\n");
		return -1;
	}
	return buffer;
}
int Memory::GetPointerAddress(HANDLE processHandle, unsigned long long startAddress, int offsets[], int offsetCount) {
	if (startAddress == -1)
		return -1;
	long ptr = ReadInt(processHandle, startAddress+offsets[0]);
	for (int i = 1; i < offsetCount - 1; i++) {
		ptr += offsets[i];
		ptr = ReadInt(processHandle, ptr);
	}
	ptr += offsets[offsetCount - 1];
	return (int)ptr;
}
int Memory::ReadPointerInt(HANDLE processHandle, unsigned long long startAddress, int offsets[], int offsetCount) {
	if (startAddress == -1)
		return -1;
	return ReadInt(processHandle, GetPointerAddress(processHandle, startAddress, offsets, offsetCount));
}
float Memory::ReadFloat(HANDLE processHandle, unsigned long long address) {
	if (address == -1)
		return -1;
	float buffer = 0.0;
	SIZE_T NumberOfBytesToRead = sizeof(buffer); //this is equal to 4
	SIZE_T NumberOfBytesActuallyRead;
	BOOL success = ReadProcessMemory(processHandle, (LPCVOID)address, &buffer, NumberOfBytesToRead, &NumberOfBytesActuallyRead);
	if (!success || NumberOfBytesActuallyRead != NumberOfBytesToRead)
		return -1;
	return buffer;
}
float Memory::ReadPointerFloat(HANDLE processHandle, unsigned long long startAddress, int offsets[], int offsetCount) {
	if (startAddress == -1)
		return -1;
	return ReadFloat(processHandle, GetPointerAddress(processHandle, startAddress, offsets, offsetCount));
}
const char* Memory::ReadText(HANDLE processHandle, unsigned long long address) {
	if (address == -1)
		return "-1";
	char buffer = !0;
	char* stringToRead = new char[128];
	SIZE_T NumberOfBytesToRead = sizeof(buffer);
	SIZE_T NumberOfBytesActuallyRead;
	int i = 0;
	while (buffer != 0) {
		BOOL success = ReadProcessMemory(processHandle, (LPCVOID)address, &buffer, NumberOfBytesToRead, &NumberOfBytesActuallyRead);
		if (!success || NumberOfBytesActuallyRead != NumberOfBytesToRead)
			return "-1";
		stringToRead[i] = buffer;
		i++;
		address++;
	}
	return stringToRead;
}
const char* Memory::ReadPointerText(HANDLE processHandle, unsigned long long startAddress, int offsets[], int offsetCount) {
	if (startAddress == -1)
		return "-1";
	return ReadText(processHandle, GetPointerAddress(processHandle, startAddress, offsets, offsetCount));
}