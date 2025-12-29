#ifndef __GAVEL_SERVER_HELPER_H
#define __GAVEL_SERVER_HELPER_H

#include <Client.h>
#include <GavelUtil.h>

typedef enum {
  OkReturnCode = 200,
  AcceptedReturnCode = 202,
  BadRequestReturnCode = 400,
  NotFoundReturnCode = 404,
  NotAllowedReturnCode = 405,
  ServerErrorReturnCode = 500
} HttpReturnCode;

typedef enum { HTTP_GET, HTTP_POST, HTTP_NONE, HTTP_UNKNOWN } HttpMethod;

/* =========================================================================
 * Enum of common HTTP Accept types + wildcards (compact)
 * ========================================================================= */
typedef enum {
  // Wildcards
  ACCEPT_ANY,             // */*
  ACCEPT_ANY_TEXT,        // text/*
  ACCEPT_ANY_IMAGE,       // image/*
  ACCEPT_ANY_APPLICATION, // application/*

  // Unknown / fallback
  ACCEPT_UNKNOWN,

  // text/*
  ACCEPT_TEXT_PLAIN,
  ACCEPT_TEXT_HTML,
  ACCEPT_TEXT_CSS,
  ACCEPT_TEXT_JAVASCRIPT,   // historical; prefer application/javascript
  ACCEPT_TEXT_EVENT_STREAM, // Server-Sent Events

  // application/*
  ACCEPT_APP_JSON,
  ACCEPT_APP_XML,

  // image/*
  ACCEPT_IMAGE_PNG,
  ACCEPT_IMAGE_JPEG,
  ACCEPT_IMAGE_GIF,
  ACCEPT_IMAGE_SVG_XML,
  ACCEPT_IMAGE_AVIF,
  ACCEPT_IMAGE_XICON,
} AcceptType;

/* ===========================================================
 * Exact MIME lookups (lowercased, no parameters)
 * =========================================================== */
typedef struct {
  const char* mime;
  AcceptType type;
} at_map_t;

static const at_map_t AT_LUT[] = {
    // text
    {"text/plain", ACCEPT_TEXT_PLAIN},
    {"text/html", ACCEPT_TEXT_HTML},
    {"text/css", ACCEPT_TEXT_CSS},
    {"text/javascript", ACCEPT_TEXT_JAVASCRIPT},
    {"text/event-stream", ACCEPT_TEXT_EVENT_STREAM},

    // application
    {"application/json", ACCEPT_APP_JSON},
    {"application/xml", ACCEPT_APP_XML},

    // image
    {"image/png", ACCEPT_IMAGE_PNG},
    {"image/jpeg", ACCEPT_IMAGE_JPEG},
    {"image/gif", ACCEPT_IMAGE_GIF},
    {"image/svg+xml", ACCEPT_IMAGE_SVG_XML},
    {"image/avif", ACCEPT_IMAGE_AVIF},
    {"image/x-icon", ACCEPT_IMAGE_XICON}};

#define AT_LUT_COUNT (sizeof(AT_LUT) / sizeof(AT_LUT[0]))

// Helper Functions
AcceptType parseAcceptHeader(const String& acceptHeader);

HttpMethod StringToHttpMethod(const char* methodStr);
const char* statusText(int code);
const char* contentTypeFromPath(const char* path);
void sendHttpHeader(Client* client, int code, const char* contentType, size_t contentLength = 0,
                    bool connectionClose = true);
String normalizePath(const String& rawPath);

#endif // __GAVEL_SERVER_HELPER_H