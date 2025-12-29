#ifndef __GAVEL_CLIENT_UTILS_H
#define __GAVEL_CLIENT_UTILS_H

#include <Arduino.h>
#include <Client.h>

#define BUFFER_SIZE 1024

char clientRead(Client* client);
unsigned int clientRead(Client* client, char* buffer, unsigned int length);
String clientReadStringUntil(Client* client, char ch);
bool clientAvailable(Client* client);
unsigned int clientWrite(Client* client, const char* buffer, unsigned int length);
unsigned int clientWrite(Client* client, void* buffer, unsigned int length);
unsigned int clientWrite(Client* client, char c);
unsigned int clientPrint(Client* client, String str);
bool clientConnected(Client* client);
void clientClose(Client* client);

#endif // __GAVEL_CLIENT_UTILS_H