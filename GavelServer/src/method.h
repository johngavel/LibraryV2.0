#ifndef __GAVEL_METHOD_ENUM_H
#define __GAVEL_METHOD_ENUM_H

#include <GavelUtil.h>

typedef enum {
  HTTP_GET,
  HTTP_POST,
  HTTP_PUT,
  HTTP_DELETE,
  HTTP_PATCH,
  HTTP_HEAD,
  HTTP_OPTIONS,
  HTTP_CONNECT,
  HTTP_TRACE,
  HTTP_NONE,
  HTTP_UNKNOWN
} HttpMethod;

HttpMethod StringToHttpMethod(const char* methodStr);

#endif // __GAVEL_METHOD_ENUM_H