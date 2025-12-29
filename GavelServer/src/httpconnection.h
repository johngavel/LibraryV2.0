#ifndef __GAVEL_HTTP_CONNECTION_H
#define __GAVEL_HTTP_CONNECTION_H

#include "serverhelper.h"

#include <Client.h>
#include <GavelInterfaces.h>
#include <GavelSPIWire.h>

#define SERVER_DIRECTORY "/www"

/*
Start → Reading Request Line → Reading Headers → Reading Body → Send Header → Complete
       ↘ Error → Terminate
Complete → Keep-Alive → Reading Request Line (loop)
Complete → Stream Mode → Event Push Loop → Terminate
Complete → Terminate
*/
typedef enum {
  StartClientConnection,
  ReadingRequestLine,
  ReadingHeaders,
  ReadingBody,
  SendHeader,
  CompleteClientConnection,
  KeepAlive,
  StreamMode,
  UnknownClientState
} ClientState;

class HttpConnection {
public:
  HttpConnection() { initialize(); };
  void initialize() {
    clearStateMachine();
    if (_client) clientClose(_client);
    _client = nullptr;

    _dfs = nullptr;
    _errorPage = "";
  };

  void clearStateMachine() {
    if (file) file->close();
    file = nullptr;
    _buffer = "";

    state = StartClientConnection;
    method = HTTP_NONE;
    code = BadRequestReturnCode;
    type = ACCEPT_ANY;

    requestContentLength = 0;
    responseContentLength = 0;
    contentType = "";
    closeConnection = true;
    stream = false;
    bytesRecieved = 0;
  }

  void newConnection(Client* c, DigitalFileSystem* dfs, String errorPage) {
    initialize();
    _client = c;
    _dfs = dfs;
    _errorPage = errorPage;
  };
  void restart() { state = StartClientConnection; }
  void close() {
    clientClose(_client);
    _client = nullptr;
  }
  Client* getClient() { return _client; };
  bool isValid() const { return _client != nullptr; };
  void execute();

  ClientState state = StartClientConnection;
  HttpMethod method = HTTP_NONE;
  HttpReturnCode code = BadRequestReturnCode;
  AcceptType type = ACCEPT_ANY;
  DigitalFile* file = nullptr;

  int requestContentLength = 0;
  int responseContentLength = 0;
  String contentType = "";
  bool closeConnection = true;
  bool stream = false;
  int bytesRecieved = 0;

private:
  ClientState readRequestLine();
  ClientState readHeaders();
  ClientState readBody();
  ClientState sendHeader();
  ClientState processClient();
  ClientState processStream();

  DigitalFileSystem* _dfs = nullptr;
  String _errorPage = "";
  Client* _client;
  String _buffer = "";
};

#endif // __GAVEL_HTTP_CONNECTION_H