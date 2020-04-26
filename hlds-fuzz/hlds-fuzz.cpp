#include <stdio.h>
#include <winsock2.h>
#include <setjmp.h>
#include "IDedicatedExports.h"
#include "IDedicatedServerAPI.h"

class CDedicatedExports : public IDedicatedExports
{
public:
	void Sys_Printf(char *text)
	{
		fputs(text, stdout);
	}
};

EXPOSE_SINGLE_INTERFACE(CDedicatedExports, IDedicatedExports, VENGINE_DEDICATEDEXPORTS_API_VERSION);

void *(*Mod_ForName)(const char *name, int crash, int trackCRC);
void (*Host_ClearMemory)(int bQuiet);
jmp_buf* host_abortserver;
jmp_buf tmp;
char fileName[_MAX_PATH];

__declspec(noinline) int fuzz()
{
	if (setjmp(tmp))
	{
		Host_ClearMemory(0);
		return 0;
	}
	
	memcpy(host_abortserver, &tmp, sizeof(jmp_buf));
	Mod_ForName(fileName, 1, 0);
	Host_ClearMemory(0);

	return 0;
}

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	char baseDir[_MAX_PATH] = ".";
	char cmdLine[4096];
	snprintf(cmdLine, sizeof(cmdLine), "-console -nobreakpad -port %s", argv[2]);
	strncpy(fileName, argv[1], _MAX_PATH - 1);
	fileName[_MAX_PATH - 1] = '\0';

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	CreateInterfaceFn launcherFactory = Sys_GetFactoryThis();
	CSysModule *filesystemModule = Sys_LoadModule("filesystem_stdio.dll");
	CreateInterfaceFn filesystemFactory = Sys_GetFactory(filesystemModule);
	CSysModule *engineModule = Sys_LoadModule("sw.dll");
	CreateInterfaceFn engineFactory = Sys_GetFactory(engineModule);
	IDedicatedServerAPI *dedicatedServerAPI = (IDedicatedServerAPI *)engineFactory(VENGINE_HLDS_API_VERSION, nullptr);

	HANDLE hProcess = GetCurrentProcess();
	DWORD dwOldProtect;

	VirtualProtect((char *)engineModule + 0xc8f4a, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, (char *)engineModule + 0xc8f4a, "\xc3", 1, nullptr); // disable SDL_ShowSimpleMessageBox
	VirtualProtect((char *)engineModule + 0xc8f4a, 1, dwOldProtect, &dwOldProtect);

	VirtualProtect((char *)engineModule + 0xa32e8, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, (char *)engineModule + 0xa32e8, "\x90\x90\x90\x90\x90", 5, nullptr); // disable _set_se_translator
	VirtualProtect((char *)engineModule + 0xa32e8, 5, dwOldProtect, &dwOldProtect);

	VirtualProtect((char*)engineModule + 0x393d1c, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, (char*)engineModule + 0x393d1c, "\x01\x00\x00\x00", 4, nullptr); // enable bReentry_14509
	VirtualProtect((char*)engineModule + 0x393d1c, 4, dwOldProtect, &dwOldProtect);

	Mod_ForName = (void *(*)(const char *, int, int))((char *)engineModule + 0x525e0);
	Host_ClearMemory = (void (*)(int))((char*)engineModule + 0x45970);
	host_abortserver = (jmp_buf *)((char*)engineModule + 0x96dfe0);

	dedicatedServerAPI->Init(baseDir, cmdLine, launcherFactory, filesystemFactory);

	fuzz();

	dedicatedServerAPI->Shutdown();

	return 0;
}
