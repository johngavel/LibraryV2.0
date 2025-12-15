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
bool ServerModule::serveFile(Client* client, const String& path) {
  String fileLocation = normalizePath(path);
  fileLocation = String(SERVER_DIRECTORY) + fileLocation;
  DEBUG(fileLocation.c_str());

  if (!dfs->verifyFile(fileLocation.c_str())) {
    DEBUG("File Not Found.");
    fileLocation = String(SERVER_DIRECTORY) + "/" + errorPage;
    bool defaultError = errorPage.isEmpty() || !dfs->verifyFile(fileLocation.c_str());
    DEBUG(fileLocation.c_str());
    if (defaultError) {
      sendHttpHeader(client, 404, "text/plain");
      client->print("404 Not Found");
      return false;
    }
  }
  DigitalFile* file = dfs->readFile(fileLocation.c_str());

  String ct = contentTypeFromPath(fileLocation);
  sendHttpHeader(client, 200, ct);

  memset(fileBuffer, 0, BUFFER_SIZE);
  unsigned long remainder = file->size() % BUFFER_SIZE;
  unsigned long loops = file->size() / BUFFER_SIZE;
  unsigned long bytes = 0;
  for (unsigned long i = 0; i < loops; i++) {
    bytes = file->readBytes(fileBuffer, BUFFER_SIZE);
    clientWrite(client, fileBuffer, bytes);
    memset(fileBuffer, 0, BUFFER_SIZE);
  }
  bytes = file->readBytes(fileBuffer, remainder);
  bytes = clientWrite(client, fileBuffer, bytes);
  file->close();
  return true;
}

bool ServerModule::executeTask() {
  const unsigned long timeoutTime = 1000;
  bool closeClient = true;
  Client* client;

  spiWire.wireTake();
  client = server->accept();
  spiWire.wireGive();
  if (clientConnected(client)) { // If a new client connects,
    DEBUG("Client Connected");
    spiWire.wireTake();
    client->setTimeout(timeoutTime);
    String requestLine = client->readStringUntil('\r');
    client->read();
    spiWire.wireGive();
    requestLine.trim();
    DEBUG(requestLine.c_str());

    // --- New: capture headers we care about (Content-Length/Type) ---
    unsigned int contentLength = 0;
    String contentType = "";
    while (client->connected()) {
      String headerLine = client->readStringUntil('\r');
      client->read(); // consume '\n'
      if (headerLine.length() == 0 || headerLine == "\n" || headerLine == "\r") {
        break; // end of headers
      }
      headerLine.trim();
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
      }
    }

    // Parse method and path
    String method, path;
    int firstSpace = requestLine.indexOf(' ');
    int secondSpace = requestLine.indexOf(' ', firstSpace + 1);
    if (firstSpace > 0 && secondSpace > firstSpace) {
      method = requestLine.substring(0, firstSpace);
      path = requestLine.substring(firstSpace + 1, secondSpace);
    }

    // --- Optional: read request body (POST/PUT/PATCH) ---
    String body = "";
    if ((method == "POST" || method == "PUT" || method == "PATCH") && contentLength > 0) {
      // Read exactly contentLength bytes
      while (client->connected() && body.length() < (unsigned int) contentLength) {
        // Read in small chunks to avoid blocking too long
        char buf[128];
        int n = client->readBytes(buf, (int) min((size_t) sizeof(buf), contentLength - body.length()));
        if (n > 0)
          body.concat(String(buf).substring(0, n));
        else
          break;
      }
    }

    if (method == "GET" || method == "POST" || method == "PUT" || method == "PATCH" || method == "DELETE") {
      serveFile(client, path);
    } else {
      sendHttpHeader(client, 405, "text/plain");
      client->print("Only GET/POST/PUT/PATCH/DELETE are supported.");
    }

    if (closeClient) { clientClose(client); }
  }
  return true;
}

// Helper Functions

const char* statusText(int code) {
  switch (code) {
  case 200: return "OK";
  case 201: return "Created";
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
  client->printf("HTTP/1.1 %d %s\r\n", code, statusText(code));
  client->printf("Content-Type: %s\r\n", contentType.c_str());
  if (contentLength > 0) { client->printf("Content-Length: %u\r\n", (unsigned) contentLength); }
  if (connectionClose) { client->print("Connection: close\r\n"); }
  client->print("\r\n");
}

String normalizePath(const String& rawPath) {
  String p = rawPath.length() ? rawPath : "/";
  int qpos = p.indexOf('?');
  if (qpos >= 0) p = p.substring(0, qpos);
  if (p == "/") return "/index.html";
  if (p.indexOf("..") >= 0) return "/404"; // basic traversal protection
  return p;
}
