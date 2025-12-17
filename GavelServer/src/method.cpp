#include "method.h"

HttpMethod StringToHttpMethod(const char* methodStr) {
  if (methodStr == NULL) return HTTP_GET; // Default or handle error

  if (safeCompare(methodStr, "GET") == 0) return HTTP_GET;
  if (safeCompare(methodStr, "POST") == 0) return HTTP_POST;
  if (safeCompare(methodStr, "PUT") == 0) return HTTP_PUT;
  if (safeCompare(methodStr, "DELETE") == 0) return HTTP_DELETE;
  if (safeCompare(methodStr, "PATCH") == 0) return HTTP_PATCH;
  if (safeCompare(methodStr, "HEAD") == 0) return HTTP_HEAD;
  if (safeCompare(methodStr, "OPTIONS") == 0) return HTTP_OPTIONS;
  if (safeCompare(methodStr, "CONNECT") == 0) return HTTP_CONNECT;
  if (safeCompare(methodStr, "TRACE") == 0) return HTTP_TRACE;
  if (safeCompare(methodStr, "") == 0) return HTTP_NONE;

  return HTTP_UNKNOWN; // Unknown method
};