#ifndef __GAVEL_SERVER_STD_H
#define __GAVEL_SERVER_STD_H

#include <GavelEEProm.h>
#include <GavelFileSystem.h>
#include <GavelInterfaces.h>
#include <GavelPicoStandard.h>
#include <GavelServer.h>
#include <GavelTask.h>

void loadServerStandard(JsonInterface* ethernet, ServerModule* server, FileSystem* fs, TaskManager* taskManager,
                        EEpromMemory* memory);

#endif // __GAVEL_SERVER_STD_H