#ifndef IDEDICATEDEXPORTS_H
#define IDEDICATEDEXPORTS_H

#include <interface.h>

class IDedicatedExports : public IBaseInterface
{
public:
	virtual void Sys_Printf(char* text) = 0;
};

#define VENGINE_DEDICATEDEXPORTS_API_VERSION "VENGINE_DEDICATEDEXPORTS_API_VERSION001"

#endif // IDEDICATEDEXPORTS_H
