#include "clientUtil.h"

#include <Arduino.h>
#include <GavelSPIWire.h>

char clientRead(Client* client) {
  spiWire.wireTake();      // if there's bytes to read from the client,
  char c = client->read(); // read a byte
  spiWire.wireGive();
  return c;
}

unsigned int clientRead(Client* client, char* buffer, unsigned int length) {
  unsigned int receivedBytes = 0;
  spiWire.wireTake();
  receivedBytes = client->read((uint8_t*) buffer, BUFFER_SIZE);
  spiWire.wireGive();
  return receivedBytes;
}

bool clientAvailable(Client* client) {
  spiWire.wireTake();
  bool a = client->available();
  spiWire.wireGive();
  return a;
}

static unsigned int __clientWrite(Client* client, char* buffer, unsigned int length) {
  unsigned int __length = (length > BUFFER_SIZE) ? BUFFER_SIZE : length;
  unsigned int totalBytes = 0;
  spiWire.wireTake();
  totalBytes = client->write((const uint8_t*) buffer, __length);
  spiWire.wireGive();
  return totalBytes;
}

unsigned int clientWrite(Client* client, char* buffer, unsigned int length) {
  unsigned int totalBytes = 0;
  unsigned long remainder = length % BUFFER_SIZE;
  unsigned long loops = length / BUFFER_SIZE;
  for (unsigned long i = 0; i < loops; i++) { totalBytes += __clientWrite(client, &buffer[i * BUFFER_SIZE], BUFFER_SIZE); }
  totalBytes += __clientWrite(client, &buffer[loops * BUFFER_SIZE], remainder);
  return totalBytes;
}

bool clientConnected(Client* client) {
  spiWire.wireTake();
  bool c = client->connected();
  spiWire.wireGive();
  return c;
}

void clientClose(Client* client) {
  spiWire.wireTake();
  client->flush();
  client->stop();
  spiWire.wireGive();
}