#ifndef __GAVEL_SERVER_STD_H
#define __GAVEL_SERVER_STD_H

#include <GavelFileSystem.h>
#include <GavelInterfaces.h>
#include <GavelServer.h>
#include <GavelTaskManager.h>

void loadServerStandard(JsonInterface* programMem, JsonInterface* hwlist, JsonInterface* license,
                        JsonInterface* ethernet, ServerModule* server, FileSystem* fs, TaskManager* taskManager,
                        JsonInterface* memory, DeviceCmd* device);

#endif // __GAVEL_SERVER_STD_H