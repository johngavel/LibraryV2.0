#include "servermodule.h"

#include <GavelDebug.h>
#include <GavelSPIWire.h>

#define HEADER_LENGTH 4096

// Helper Functions
String contentTypeFromPath(const String& path);
void sendHttpHeader(Client* client, int code, const String& status, const String& contentType, size_t contentLength = 0,
                    bool connectionClose = true);
String normalizePath(const String& rawPath);

// Server Mdoule Methods
ServerModule::ServerModule() : Task("HTTPServer") {
  setRefreshMilli(10);
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
  if (path == "/www/404") {
    sendHttpHeader(client, 404, "Not Found", "text/plain");
    client->print("404 Not Found");
    return false;
  }
  if (!dfs->verifyFile(fileLocation.c_str())) {
    sendHttpHeader(client, 404, "Not Found", "text/plain");
    client->print("404 Not Found");
    DEBUG("File Not Found.");
    return false;
  }
  DigitalFile* file = dfs->readFile(fileLocation.c_str());

  String ct = contentTypeFromPath(fileLocation);
  sendHttpHeader(client, 200, "OK", ct);

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
  return true;
}

// Simple API: GET /api/time -> JSON
bool ServerModule::serveApi(Client* client, const String& path) {
  // Only one endpoint for demo
  if (path.startsWith("/api/time")) {
    // Example JSON payload
    String json = "{";
    json += "\"epoch_ms\":" + String(millis()) + ",";
    json += "\"uptime_sec\":" + String(millis() / 1000) + ",";
    json += "\"message\":\"Hello from Pico W\"";
    json += "}";

    sendHttpHeader(client, 200, "OK", "application/json", json.length());
    client->print(json);
    return true;
  }

  // Unknown API endpoint
  sendHttpHeader(client, 404, "Not Found", "application/json");
  client->print("{\"error\":\"unknown endpoint\"}");
  return false;
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

    // Minimal header consume: read until blank line
    while (client->connected()) {
      String headerLine = client->readStringUntil('\r');
      client->read(); // consume '\n'
      if (headerLine.length() == 0 || headerLine == "\n" || headerLine == "\r") {
        break; // end of headers
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

    if (method == "GET") {
      if (path.startsWith(API_DIRECTORY)) {
        serveApi(client, path);
      } else {
        serveFile(client, path);
      }
    } else {
      sendHttpHeader(client, 405, "Method Not Allowed", "text/plain");
      client->print("Only GET is supported.");
    }

    if (closeClient) { clientClose(client); }
  }
  return true;
}

// Helper Functions
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

void sendHttpHeader(Client* client, int code, const String& status, const String& contentType, size_t contentLength,
                    bool connectionClose) {
  client->printf("HTTP/1.1 %d %s\r\n", code, status.c_str());
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
