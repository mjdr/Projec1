#include "ProcessReader.h"
#include "DeepPointer.h"
#include "Config.h"
#include "Memory.hpp"
#include <windows.h>

static HANDLE pHandle = nullptr;
static DWORD pid;

static Memory mem;


void ProcessReader_open(){

	mem.GetDebugPrivileges();
	pid = mem.GetProcessId(Config_get()->process);
	if (!pid) {
		printf("PID: %d\nCan't find process!\n", pid);
		exit(-1);
	}
	pHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pid);
}
int ProcessReader_readVariable(VariableValue* buffer, Variable const *  var) {
	if (var->type == VARTYPE_FLOAT) {
		string mod(var->pointer.module);
		buffer->floatvalue = mem.ReadPointerFloat(pHandle, mem.GetModuleBase(pHandle, mod), (int*)var->pointer.offsets, var->pointer.offsetsCount);
		return 1;
	}
	return 0;
}
