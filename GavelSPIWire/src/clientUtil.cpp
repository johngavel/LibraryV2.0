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
  receivedBytes = client->read((unsigned char*) buffer, length);
  spiWire.wireGive();
  return receivedBytes;
}

String clientReadStringUntil(Client* client, char ch) {
  spiWire.wireTake();
  String str = client->readStringUntil(ch);
  spiWire.wireGive();
  return str;
}

bool clientAvailable(Client* client) {
  spiWire.wireTake();
  bool a = client->available();
  spiWire.wireGive();
  return a;
}

// Writes exactly len bytes or fails after deadlineMs.
// Returns true on success, false on timeout/disconnect.
static unsigned int writeAll(Client* client, const unsigned char* data, unsigned int len,
                             unsigned long deadlineMs = 2000) {
  unsigned int sent = 0;
  unsigned long start = millis();
  while (sent < len) {
    if (!clientConnected(client)) return 0;

    // Try to write remaining bytes
    unsigned int n;
    // Keep the SPI lock as SHORT as possible (or remove if driver handles SPI internally).
    spiWire.wireTake();
    n = client->write(data + sent, (unsigned int) (len - sent));
    spiWire.wireGive();

    if (n > 0) {
      sent += (size_t) n;
      continue;
    }

    // No progress: check timeout and yield so net stack can drain buffers
    if ((millis() - start) > deadlineMs) return sent;
    delay(1);
  }
  return sent;
}

unsigned int clientWrite(Client* client, const char* buffer, unsigned int length) {
  return writeAll(client, (const unsigned char*) buffer, length);
}

unsigned int clientWrite(Client* client, void* buffer, unsigned int length) {
  return writeAll(client, (const unsigned char*) buffer, length);
}

unsigned int clientWrite(Client* client, char c) {
  unsigned int totalBytes = 0;
  spiWire.wireTake();
  totalBytes = client->write(c);
  spiWire.wireGive();
  return totalBytes;
}

unsigned int clientPrint(Client* client, String str) {
  const unsigned char* p = (const unsigned char*) str.c_str();
  return writeAll(client, p, str.length());
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