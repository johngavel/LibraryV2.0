#ifndef __GAVEL_SERVER_STD_H
#define __GAVEL_SERVER_STD_H

#include <GavelEthernet.h>
#include <GavelFileSystem.h>
#include <GavelPicoStandard.h>
#include <GavelServer.h>

void loadServerStandard(EthernetMemory* ethernet, ServerModule* server, FileSystem* fs);

#endif // __GAVEL_SERVER_STD_H