#ifndef IDEDICATEDSERVERAPI_H
#define IDEDICATEDSERVERAPI_H

#include <interface.h>

class IDedicatedServerAPI : public IBaseInterface
{
public:
	virtual bool Init(char *basedir, char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory) = 0;
	virtual int Shutdown() = 0;
	virtual bool RunFrame() = 0;
	virtual void AddConsoleText(char *text) = 0;
	virtual void UpdateStatus(float *fps, int *nActive, int *nMaxPlayers, char *pszMap) = 0;
};

#define VENGINE_HLDS_API_VERSION "VENGINE_HLDS_API_VERSION002"

#endif // IDEDICATEDSERVERAPI_H
