// Helper Functions
#include "serverhelper.h"

#include "GavelSPIWire.h"

HttpMethod StringToHttpMethod(const char* methodStr) {
  if (methodStr == NULL) return HTTP_NONE; // Default or handle error

  if (safeCompare(methodStr, "GET") == 0) return HTTP_GET;
  if (safeCompare(methodStr, "POST") == 0) return HTTP_POST;
  // if (safeCompare(methodStr, "PUT") == 0) return HTTP_PUT;
  // if (safeCompare(methodStr, "DELETE") == 0) return HTTP_DELETE;
  // if (safeCompare(methodStr, "PATCH") == 0) return HTTP_PATCH;
  // if (safeCompare(methodStr, "HEAD") == 0) return HTTP_HEAD;
  // if (safeCompare(methodStr, "OPTIONS") == 0) return HTTP_OPTIONS;
  // if (safeCompare(methodStr, "CONNECT") == 0) return HTTP_CONNECT;
  // if (safeCompare(methodStr, "TRACE") == 0) return HTTP_TRACE;
  if (safeCompare(methodStr, "") == 0) return HTTP_UNKNOWN;

  return HTTP_UNKNOWN; // Unknown method
};

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
  default: return "Uknown Status Code";
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
  if (path.endsWith(".stream")) return "text/event-stream";
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

/* ===========================================================
 * Helpers (String-based, Arduino-safe)
 * =========================================================== */
static inline String at_trim(const String& s) {
  unsigned int start = 0;
  while (start < s.length() && isspace(s[start])) start++;
  unsigned int end = s.length();
  while (end > start && isspace(s[end - 1])) end--;
  return (end > start) ? s.substring(start, end) : String();
}

// Lowercase copy (String has toLowerCase(), but we avoid in-place mutations)
static inline String at_lower(const String& s) {
  String tmp = s;
  tmp.toLowerCase();
  return tmp;
}

// Strip parameters after ';' (e.g., "; charset=utf-8", ";q=0.9")
static inline String at_strip_params(const String& s) {
  int semi = s.indexOf(';');
  return (semi >= 0) ? s.substring(0, semi) : s;
}

// True if type wildcard like "image/*"
static inline bool at_is_type_wildcard(const String& mime) {
  return mime.endsWith("/*");
}

/* ===========================================================
 * Wildcard mapping
 * =========================================================== */
static inline AcceptType at_wildcard_to_enum(const String& mime) {
  if (mime == "*/*")
    return ACCEPT_ANY;
  else if (mime == "text/*")
    return ACCEPT_ANY_TEXT;
  else if (mime == "image/*")
    return ACCEPT_ANY_IMAGE;
  else if (mime == "application/*")
    return ACCEPT_ANY_APPLICATION;
  return ACCEPT_UNKNOWN;
}

/* ===========================================================
 * stringToAcceptType: convert a single media type to enum
 * e.g., "application/json", "Text/HTML; q=0.9" or wildcards
 * =========================================================== */
static inline AcceptType stringToAcceptType(const String& mediaTypeIn) {
  if (mediaTypeIn.length() == 0) return ACCEPT_UNKNOWN;

  // Trim, lowercase, strip parameters
  String s = at_lower(at_strip_params(at_trim(mediaTypeIn)));

  // Wildcards
  if (s == "*/*") return ACCEPT_ANY;
  if (at_is_type_wildcard(s)) return at_wildcard_to_enum(s);

  // Exact matches
  for (uint16_t i = 0; i < AT_LUT_COUNT; ++i) {
    // Compare against PROGMEM literals via String
    if (s.equals(String(AT_LUT[i].mime))) { return AT_LUT[i].type; }
  }
  return ACCEPT_UNKNOWN;
}

AcceptType parseAcceptHeader(const String& acceptHeader) {
  if (acceptHeader.length() == 0) return ACCEPT_UNKNOWN;

  unsigned int start = 0;
  while (start <= acceptHeader.length()) {
    unsigned int comma = acceptHeader.indexOf(',', start);
    String token = (comma >= 0) ? acceptHeader.substring(start, comma) : acceptHeader.substring(start);

    token = at_trim(token);
    if (token.length() > 0) {
      AcceptType t = stringToAcceptType(token);
      if (t != ACCEPT_UNKNOWN) return t; // choose first recognizable type
    }

    if (comma < 0) break;
    start = comma + 1;
  }
  return ACCEPT_UNKNOWN;
}
