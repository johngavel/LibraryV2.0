#include "servermodule.h"

#include <GavelDebug.h>
#include <GavelSPIWire.h>

#define HEADER_LENGTH 4096

// Helper Functions
static const char* statusText(int code);
String contentTypeFromPath(const String& path);
void sendHttpHeader(Client* client, int code, const String& contentType, size_t contentLength = 0,
                    bool connectionClose = true);
String normalizePath(const String& rawPath);

// Server Mdoule Methods
ServerModule::ServerModule() : Task("HTTPServer") {
  setRefreshMilli(10);
}

void ServerModule::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd) {
    // No Commands at this time.
  }
}

bool ServerModule::setupTask(OutputInterface* __terminal) {
  spiWire.wireTake();
  server->begin();
  spiWire.wireGive();
  return true;
}

static char fileBuffer[BUFFER_SIZE];

bool ServerModule::transferFileToClient(ClientFileEntry* cfe) {
  memset(fileBuffer, 0, BUFFER_SIZE);
  unsigned long remainder = cfe->file->size() % BUFFER_SIZE;
  unsigned long loops = cfe->file->size() / BUFFER_SIZE;
  unsigned long bytes = 0;
  for (unsigned long i = 0; i < loops; i++) {
    bytes = cfe->file->readBytes(fileBuffer, BUFFER_SIZE);
    clientWrite(cfe->client, fileBuffer, bytes);
    memset(fileBuffer, 0, BUFFER_SIZE);
  }
  bytes = cfe->file->readBytes(fileBuffer, remainder);
  bytes = clientWrite(cfe->client, fileBuffer, bytes);
  return true;
}

bool ServerModule::serveFile(ClientFileEntry* cfe, HttpMethod method, const String& path) {
  String fileLocation = normalizePath(path);
  fileLocation = String(SERVER_DIRECTORY) + fileLocation;

  if (!dfs->verifyFile(fileLocation.c_str())) {
    DEBUG("File Not Found.");
    fileLocation = String(SERVER_DIRECTORY) + "/" + errorPage;
    bool defaultError = errorPage.isEmpty() || !dfs->verifyFile(fileLocation.c_str());
    if (defaultError) {
      sendHttpHeader(cfe->client, 404, "text/plain");
      clientPrint(cfe->client, "404 Not Found");
      return false;
    }
  }

  DigitalFile* file = dfs->readFile(fileLocation.c_str());
  if (file == nullptr) {
    DEBUG("File Not Readable.");
    sendHttpHeader(cfe->client, 404, "text/plain");
    clientPrint(cfe->client, "404 Not Found");
    return false;
  }
  cfe->file = file;
  String ct = contentTypeFromPath(fileLocation);
  sendHttpHeader(cfe->client, 200, ct);
  transferFileToClient(cfe);
  if (cfe->stream == false) cfe->file->close();
  return true;
}

bool ServerModule::saveFile(ClientFileEntry* cfe, HttpMethod method, const String& body, const String& path) {
  String fileLocation = normalizePath(path);
  fileLocation = String(SERVER_DIRECTORY) + fileLocation;

  if (!dfs->verifyFile(fileLocation.c_str())) {
    DEBUG("File Not Found.");
    fileLocation = String(SERVER_DIRECTORY) + "/" + errorPage;
    bool defaultError = errorPage.isEmpty() || !dfs->verifyFile(fileLocation.c_str());
    if (defaultError) {
      sendHttpHeader(cfe->client, 404, "text/plain");
      clientPrint(cfe->client, "404 Not Found");
      return false;
    }
  }
  DigitalFile* file = dfs->writeFile(fileLocation.c_str());
  if (file == nullptr) {
    DEBUG("File Not Writable.");
    sendHttpHeader(cfe->client, 404, "text/plain");
    clientPrint(cfe->client, "404 Not Found");
    return false;
  }
  cfe->file = file;
  String ct = contentTypeFromPath(fileLocation);
  sendHttpHeader(cfe->client, 202, ct);

  int size = ((int) body.length() < cfe->file->size()) ? body.length() : cfe->file->size();
  cfe->file->write((const unsigned char*) body.c_str(), size);
  if (cfe->stream == false) cfe->file->close();

  return true;
}

bool ServerModule::processMethodClient(ClientFileEntry* cfe) {
  const unsigned long timeoutTime = 1000;
  const unsigned long timeoutTimeLong = 10 * timeoutTime;
  if (clientConnected(cfe->client)) { // If a new client connects,
    String requestLine = clientReadStringUntil(cfe->client, '\r');
    clientRead(cfe->client); // consume '\n'
    requestLine.trim();
    DEBUG(("Method: " + requestLine).c_str());

    // --- New: capture headers we care about (Content-Length/Type) ---
    unsigned int contentLength = 0;
    String contentType = "";
    while (clientConnected(cfe->client)) {
      String headerLine = clientReadStringUntil(cfe->client, '\r');
      clientRead(cfe->client); // consume '\n'
      if (headerLine.length() == 0 || headerLine == "\n" || headerLine == "\r") {
        break; // end of headers
      }
      headerLine.trim();
      DEBUG(("Header: " + headerLine).c_str());
      int colon = headerLine.indexOf(':');
      if (colon > 0) {
        String key = headerLine.substring(0, colon);
        String val = headerLine.substring(colon + 1);
        key.trim();
        val.trim();
        key.toLowerCase();
        if (key == "content-length")
          contentLength = (unsigned int) val.toInt();
        else if (key == "content-type")
          contentType = val;
        else if (key == "Connection") {
          if (val == "keep-alive") cfe->keepAlive = true;
        } else if (key == "Accept")
          if (val == "text/event-stream") cfe->stream = true;
      }
    }
    if (cfe->stream)
      cfe->client->setTimeout(timeoutTimeLong);
    else
      cfe->client->setTimeout(timeoutTime);

    // Parse method and path
    String methodStr, pathStr;
    int firstSpace = requestLine.indexOf(' ');
    int secondSpace = requestLine.indexOf(' ', firstSpace + 1);
    if (firstSpace > 0 && secondSpace > firstSpace) {
      methodStr = requestLine.substring(0, firstSpace);
      pathStr = requestLine.substring(firstSpace + 1, secondSpace);
    }
    HttpMethod method = StringToHttpMethod(methodStr.c_str());

    // --- Optional: read request body (POST/PUT/PATCH) ---
    String body = "";
    if ((method == HTTP_POST || method == HTTP_PUT || method == HTTP_PATCH) && contentLength > 0) {
      // Read exactly contentLength bytes
      while (clientConnected(cfe->client) && body.length() < (unsigned int) contentLength) {
        // Read in small chunks to avoid blocking too long
        char buf[128];
        int n = clientRead(cfe->client, buf, (int) min((size_t) sizeof(buf), contentLength - body.length()));
        if (n > 0)
          body.concat(String(buf).substring(0, n));
        else
          break;
      }
      DEBUG(("Body:" + body).c_str());
    }
    switch (method) {
    case HTTP_GET: serveFile(cfe, method, pathStr); break;
    case HTTP_POST: saveFile(cfe, method, body, pathStr); break;
    case HTTP_NONE: break;
    case HTTP_PUT:
    case HTTP_PATCH:
    case HTTP_DELETE:
    case HTTP_UNKNOWN:
    default:
      sendHttpHeader(cfe->client, 405, statusText(405));
      clientPrint(cfe->client, "Only GET/POST/PUT/PATCH/DELETE are supported.");
      break;
    }

    if (cfe->keepAlive == false) { clientClose(cfe->client); }
  }
  return true;
}
bool ServerModule::processStreamClient(ClientFileEntry* cfe) {
  char c;
  if (cfe->file) {
    if (cfe->file->getMode() == READ_MODE) {
      transferFileToClient(cfe);
    } else {
      while (clientAvailable(cfe->client)) {
        c = clientRead(cfe->client);
        cfe->file->write(c);
      }
    }
  }
  return true;
}
bool ServerModule::processClient(ClientFileEntry* cfe) {
  if (cfe->stream == true)
    processStreamClient(cfe);
  else
    processMethodClient(cfe);
  return true;
}

bool ServerModule::executeTask() {
  Client* client;

  spiWire.wireTake();
  client = server->accept();
  spiWire.wireGive();
  if (clientConnected(client)) clientPool.add(client);

  for (unsigned int i = 0; i < clientPool.capacity(); i++) {
    ClientFileEntry* cfe = clientPool.at(i);
    if (cfe->used) processClient(cfe);
  }
  clientPool.sweepDisconnected();
  return true;
}

// Helper Functions

const char* statusText(int code) {
  switch (code) {
  case 200: return "OK";
  case 201: return "Created";
  case 202: return "Accepted";
  case 204: return "No Content";
  case 400: return "Bad Request";
  case 401: return "Unauthorized";
  case 403: return "Forbidden";
  case 404: return "Not Found";
  case 405: return "Method Not Allowed";
  case 415: return "Unsupported Media Type";
  case 500: return "Internal Server Error";
  default: return "OK";
  }
}

String contentTypeFromPath(const String& path) {
  if (path.endsWith(".html")) return "text/html";
  if (path.endsWith(".htm")) return "text/html";
  if (path.endsWith(".css")) return "text/css";
  if (path.endsWith(".js")) return "application/javascript";
  if (path.endsWith(".json")) return "application/json";
  if (path.endsWith(".png")) return "image/png";
  if (path.endsWith(".jpg")) return "image/jpeg";
  if (path.endsWith(".jpeg")) return "image/jpeg";
  if (path.endsWith(".gif")) return "image/gif";
  if (path.endsWith(".svg")) return "image/svg+xml";
  if (path.endsWith(".ico")) return "image/x-icon";
  if (path.endsWith(".txt")) return "text/plain";
  return "application/octet-stream";
}

void sendHttpHeader(Client* client, int code, const String& contentType, size_t contentLength, bool connectionClose) {
  String response = "HTTP/1.1 " + String(code) + " " + statusText(code) + "\r\n";
  clientPrint(client, response);
  response = "Content-Type: " + contentType + "\r\n";
  clientPrint(client, response);
  if (contentLength > 0) {
    response = "Content-Length: " + String(contentLength) + "\r\n";
    clientPrint(client, response);
  }
  if (connectionClose) { clientPrint(client, "Connection: close\r\n"); }
  clientPrint(client, "\r\n");
}

String normalizePath(const String& rawPath) {
  String p = rawPath.length() ? rawPath : "/";
  int qpos = p.indexOf('?');
  if (qpos >= 0) p = p.substring(0, qpos);
  if (p == "/") return "/index.html";
  if (p.indexOf("..") >= 0) return "/404"; // basic traversal protection
  return p;
}
