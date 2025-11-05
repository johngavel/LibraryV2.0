#ifndef GAVEL_CLIENT_UTILS_H
#define GAVEL_CLIENT_UTILS_H

char clientRead(Client* client);
unsigned int clientRead(Client* client, char* buffer, unsigned int length);
bool clientAvailable(Client* client);
unsigned int clientWrite(Client* client, char* buffer, unsigned int length);
bool clientConnected(Client* client);
void clientClose(Client* client);

#endif // GAVEL_CLIENT_UTILS_H