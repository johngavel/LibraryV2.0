#ifndef __GAVEL_CLIENT_MANAGE_H
#define __GAVEL_CLIENT_MANAGE_H

#include "httpconnection.h"

#include <Client.h> // Arduino networking base class
#include <GavelInterfaces.h>
#include <GavelSPIWire.h>

#define CLIENT_FILE_POOL_CAPACITY MAX_CLIENTS

struct ClientFileEntry {
  HttpConnection connection;
  bool used = false;

  // A "valid" entry is one that is in use and has a client pointer.
  bool isValid() const { return used && connection.isValid(); }
};

class ClientFilePool {
public:
  // --- Add: places (Client*, File*) in the first free slot. Returns true if added.
  bool add(Client* c, DigitalFileSystem* dfs, String errorPage) {
    if (!c) return false; // must have a client
    for (size_t i = 0; i < CLIENT_FILE_POOL_CAPACITY; ++i) {
      if (!slots[i].used) {
        slots[i].connection.newConnection(c, dfs, errorPage);
        slots[i].used = true;
        return true;
      }
    }
    return false; // no space
  }

  // --- Remove by Client* (exact match). Returns true if removed.
  bool remove(Client* c) {
    if (!c) return false;
    for (size_t i = 0; i < CLIENT_FILE_POOL_CAPACITY; ++i) {
      if (slots[i].used && slots[i].connection.getClient() == c) {
        clearSlot(i);
        return true;
      }
    }
    return false;
  }

  // --- Remove by index, if currently used.
  bool removeAt(size_t idx) {
    if (idx >= CLIENT_FILE_POOL_CAPACITY || !slots[idx].used) return false;
    clearSlot(idx);
    return true;
  }

  // --- Convenience: remove entries whose client is disconnected or null.
  // If your client type doesn't implement connected(), comment that part out.
  void sweepDisconnected() {
    for (size_t i = 0; i < CLIENT_FILE_POOL_CAPACITY; ++i) {
      if (slots[i].used) {
        if (slots[i].connection.getClient() == nullptr || !slots[i].connection.getClient()->connected()) {
          clearSlot(i);
        }
      }
    }
  }

  // --- Stats / helpers
  size_t capacity() const { return CLIENT_FILE_POOL_CAPACITY; }
  size_t usedCount() const {
    size_t n = 0;
    for (size_t i = 0; i < CLIENT_FILE_POOL_CAPACITY; ++i)
      if (slots[i].used) ++n;
    return n;
  }
  bool hasSpace() const { return usedCount() < CLIENT_FILE_POOL_CAPACITY; }

  // Access a slot directly (read‑only).
  ClientFileEntry* at(size_t idx) { return &slots[idx]; }

private:
  ClientFileEntry slots[CLIENT_FILE_POOL_CAPACITY];

  void clearSlot(size_t idx) {
    ClientFileEntry& e = slots[idx];

    // Optional: gently stop the client if still connected.
    if (e.connection.getClient()) { clientClose(e.connection.getClient()); }

    // Optional: close the file if open (only if you own it).
    if (e.connection.file) {
      // Some cores allow `if (*e.file)` to test openness; `close()` is safe to call once.
      e.connection.file->close();
    }

    e.connection.initialize();
    e.used = false;
  }
};

#endif // __GAVEL_CLIENT_MANAGE_H