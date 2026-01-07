#include "httpconnection.h"

/*
typedef enum {
  Start,
  ReadingRequestLine,
  ReadingHeaders,
  ReadingBody,
  Complete,
  KeepAlive,
  StreamMode,
  Unknown
} ClientState;
 */
void HttpConnection::execute() {
  if ((_client == nullptr) || (!clientConnected(_client))) {
    state = CompleteClientConnection;
    return;
  }
  ClientState oldState = UnknownClientState;
  while (oldState != state) {
    oldState = state;
    switch (state) {
    case StartClientConnection:
      if (clientAvailable(_client)) {
        if (_dfs == nullptr) {
          while (clientAvailable(_client)) clientRead(_client);
          code = ServerErrorReturnCode;
          state = SendHeader;
        } else {
          state = ReadingRequestLine;
        }
      }
      break;
    case ReadingRequestLine: state = readRequestLine(); break;
    case ReadingHeaders: state = readHeaders(); break;
    case ReadingBody: state = readBody(); break;
    case SendHeader: state = sendHeader(); break;
    case CompleteClientConnection: break;
    case KeepAlive: state = processClient(); break;
    case StreamMode: state = processStream(); break;
    case UnknownClientState:
    default: state = StartClientConnection; break;
    }
  }
}

static inline bool isReadMethod(HttpMethod method) {
  switch (method) {
  case HTTP_GET: return true;
  default: break;
  }
  return false;
}

ClientState HttpConnection::readRequestLine() {
  while (clientAvailable(_client)) {
    char c = clientRead(_client);
    if (c == '\r') {
      clientRead(_client);
      _buffer.trim();
      if (_buffer.isEmpty()) return StartClientConnection;
      String methodStr, pathStr;
      int firstSpace = _buffer.indexOf(' ');
      int secondSpace = _buffer.indexOf(' ', firstSpace + 1);
      if (firstSpace > 0 && secondSpace > firstSpace) {
        methodStr = _buffer.substring(0, firstSpace);
        pathStr = _buffer.substring(firstSpace + 1, secondSpace);
      }
      method = StringToHttpMethod(methodStr.c_str());
      if (method == HTTP_UNKNOWN) {
        code = NotAllowedReturnCode;
        return SendHeader;
      }

      String fileLocation = normalizePath(pathStr);
      fileLocation = String(SERVER_DIRECTORY) + fileLocation;
      if (!_dfs->verifyFile(fileLocation.c_str())) {
        code = NotFoundReturnCode;
        fileLocation = String(SERVER_DIRECTORY) + "/" + _errorPage;
        bool defaultError = _errorPage.isEmpty() || !_dfs->verifyFile(fileLocation.c_str());
        if (defaultError) { return SendHeader; }
      }
      if (isReadMethod(method))
        file = _dfs->readFile(fileLocation.c_str());
      else
        file = _dfs->writeFile(fileLocation.c_str());
      if (file == nullptr) {
        code = BadRequestReturnCode;
        return SendHeader;
      }
      if (!file) {
        code = ServerErrorReturnCode;
        return SendHeader;
      }
      if (isReadMethod(method))
        responseContentLength = file->size();
      else
        responseContentLength = 0;
      _buffer = "";
      return ReadingHeaders;
    }
    _buffer += c;
  }
  return ReadingRequestLine;
}

ClientState HttpConnection::readHeaders() {
  const unsigned long timeoutTime = 1;
  const unsigned long timeoutTimeLong = 10 * timeoutTime;
  while (clientAvailable(_client)) {
    char c = clientRead(_client);
    if (c == '\r') {
      clientRead(_client);
      if (_buffer.length() == 0 || _buffer == "\n" || _buffer == "\r") {
        _buffer = "";
        if (stream)
          _client->setTimeout(timeoutTimeLong);
        else
          _client->setTimeout(timeoutTime);
        return ReadingBody; // end of headers
      }
      _buffer.trim();
      int colon = _buffer.indexOf(':');
      if (colon > 0) {
        String key = _buffer.substring(0, colon);
        String val = _buffer.substring(colon + 1);
        key.trim();
        val.trim();
        key.toLowerCase();
        if (key == "content-length") {
          requestContentLength = (unsigned int) val.toInt();
          bytesRecieved = 0;
        } else if (key == "content-type")
          contentType = val;
        else if ((key == "connection") && (val == "keep-alive"))
          closeConnection = false;
        else if ((key == "accept") && (val == "text/event-stream"))
          stream = true;
      }
      _buffer = "";
    }
    _buffer += c;
  }
  return ReadingHeaders;
}

ClientState HttpConnection::readBody() {
  // For write methods (e.g., POST), stream directly to file
  if (!isReadMethod(method) && requestContentLength > 0) {
    Timer t;
    t.setRefreshMilli(500);
    t.reset();
    while (!t.expired() && clientConnected(_client) && clientAvailable(_client) &&
           bytesRecieved < requestContentLength) {
      char buf[128];
      int need = (int) min((size_t) sizeof(buf), (size_t) (requestContentLength - bytesRecieved));
      int n = clientRead(_client, buf, need);
      if (n > 0) {
        file->write((const unsigned char*) buf, (size_t) n);
        bytesRecieved += n;
      }
    }
    if (bytesRecieved >= requestContentLength) {
      code = AcceptedReturnCode;
      return SendHeader;
    }
  } else {
    // For GET or no body, proceed to header sending
    code = OkReturnCode;
    return SendHeader;
  }
  return ReadingBody;
}

ClientState HttpConnection::sendHeader() {
  if (stream) {
    sendHttpHeader(_client, OkReturnCode, "text/event-stream", 0, false);
  } else if (file != nullptr) {
    sendHttpHeader(_client, code, contentTypeFromPath(file->name()), responseContentLength, closeConnection);
  } else {
    sendHttpHeader(_client, code, "text/plain");
  }
  if ((!closeConnection) && (stream)) return StreamMode;
  if (!closeConnection) return KeepAlive;
  return CompleteClientConnection;
}

static char fileBuffer[BUFFER_SIZE];
static bool transferFileToClient(Client* client, DigitalFile* file) {
  memset(fileBuffer, 0, BUFFER_SIZE);
  if (file->available() == 0) return false;
  unsigned long remainder = file->available() % BUFFER_SIZE;
  unsigned long loops = file->available() / BUFFER_SIZE;
  unsigned long bytes = 0;
  for (unsigned long i = 0; i < loops; i++) {
    bytes = file->readBytes(fileBuffer, BUFFER_SIZE);
    clientWrite(client, fileBuffer, bytes);
    memset(fileBuffer, 0, BUFFER_SIZE);
  }
  bytes = file->readBytes(fileBuffer, remainder);
  bytes = clientWrite(client, fileBuffer, bytes);
  return true;
}

ClientState HttpConnection::processClient() {
  if (isReadMethod(method)) transferFileToClient(_client, file);
  file->close();
  // else reading the body has already written to the file.

  clearStateMachine();
  if (!closeConnection) { return StartClientConnection; }
  clientClose(_client);
  return CompleteClientConnection;
}

ClientState HttpConnection::processStream() {
  if (isReadMethod(method)) transferFileToClient(_client, file);
  return StreamMode;
}