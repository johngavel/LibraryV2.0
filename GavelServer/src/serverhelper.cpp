// Helper Functions
#include "serverhelper.h"

#include "serverdebug.h"

#include <GavelSPIWire.h>

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

// Case-sensitive endsWith helper for const char*
static bool endsWith(const char* s, const char* suffix) {
  if (!s || !suffix) return false;
  unsigned int ls = strlen(s);
  unsigned int lf = strlen(suffix);
  if (lf > ls) return false;
  const char* a = s + (ls - lf);
  for (unsigned int i = 0; i < lf; ++i) {
    if (a[i] != suffix[i]) return false;
  }
  return true;
}

const char* contentTypeFromPath(const char* path) {
  if (endsWith(path, ".html")) return "text/html";
  if (endsWith(path, ".htm")) return "text/html";
  if (endsWith(path, ".css")) return "text/css";
  if (endsWith(path, ".js")) return "application/javascript";
  if (endsWith(path, ".json")) return "application/json";
  if (endsWith(path, ".png")) return "image/png";
  if (endsWith(path, ".jpg")) return "image/jpeg";
  if (endsWith(path, ".jpeg")) return "image/jpeg";
  if (endsWith(path, ".gif")) return "image/gif";
  if (endsWith(path, ".svg")) return "image/svg+xml";
  if (endsWith(path, ".ico")) return "image/x-icon";
  if (endsWith(path, ".txt")) return "text/plain";
  if (endsWith(path, ".stream")) return "text/event-stream";
  return "application/octet-stream";
}

void sendHttpHeader(Client* client, int code, const char* contentType, size_t contentLength, bool connectionClose,
                    bool sendContentLength) {
  // Build line-by-line to reduce heap churn
  char line[128];

  int n = snprintf(line, sizeof(line), "HTTP/1.1 %d %s\r\n", code, statusText(code));
#ifdef DEBUG_SERVER
  DBG_PRINTLNS(line);
#endif
  if (n <= 0 || !clientWrite(client, line, (unsigned int) n)) return;

  n = snprintf(line, sizeof(line), "Content-Type: %s\r\n", contentType);
#ifdef DEBUG_SERVER
  DBG_PRINTLNS(line);
#endif
  if (n <= 0 || !clientWrite(client, line, (unsigned int) n)) return;

  // Always send Content-Length (0 for no body)
  if (sendContentLength) {
    n = snprintf(line, sizeof(line), "Content-Length: %lu\r\n", (unsigned long) contentLength);
#ifdef DEBUG_SERVER
    DBG_PRINTLNS(line);
#endif
    if (n <= 0 || !clientWrite(client, line, (unsigned int) n)) return;
  }

  n = snprintf(line, sizeof(line), "Cache-Control: no-cache\r\n");
#ifdef DEBUG_SERVER
  DBG_PRINTLNS(line);
#endif
  if (n <= 0 || !clientWrite(client, line, (unsigned int) n)) return;

  n = snprintf(line, sizeof(line), "Connection: %s\r\n", connectionClose ? "close" : "keep-alive");
#ifdef DEBUG_SERVER
  DBG_PRINTLNS(line);
#endif
  if (n <= 0 || !clientWrite(client, line, (unsigned int) n)) return;

  clientWrite(client, "\r\n", 2); // header terminator
  return;
}

String normalizePath(const String& rawPath) {
  String p = rawPath.length() ? rawPath : "/";
  int qpos = p.indexOf('?');
  if (qpos >= 0) p = p.substring(0, qpos);
  if (p == "/") return "/index.html";
  if (p.indexOf("..") >= 0) return "/index.html"; // basic traversal protection
  return p;
}

String normalizeQuery(const String& rawPath) {
  if (rawPath.length() == 0) return "";
  int qpos = rawPath.indexOf('?');
  if (qpos < 0) return "";
  if (qpos > (int) rawPath.length() - 1) return "";
  return rawPath.substring(qpos);
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
