#ifndef __GAVEL_CONTENT_TYPE_H
#define __GAVEL_CONTENT_TYPE_H

#include <Arduino.h>

// Returns true if the Content-Type represents text that is safe to render/print.
inline bool isPrintableTextContentType(const String& contentTypeIn) {
  if (contentTypeIn.length() == 0) return false;

  // 1) Make a lower-cased, trimmed copy
  String ct = contentTypeIn;
  ct.trim();

  String lower;
  lower.reserve(ct.length());
  for (size_t i = 0; i < ct.length(); ++i) { lower += (char) tolower((unsigned char) ct[i]); }

  // 2) Strip any parameters after the first ';'
  int semi = lower.indexOf(';');
  if (semi >= 0) {
    lower.remove(semi); // keep only "<type>/<subtype>"
  }
  lower.trim();

  // 3) Split type/subtype
  int slash = lower.indexOf('/');
  if (slash <= 0 || slash == (int) lower.length() - 1) {
    // malformed or missing subtype
    return false;
  }

  String type = lower.substring(0, slash);
  String subtype = lower.substring(slash + 1);

  // 4) Fast paths
  if (type == F("text")) {
    // All text/* are printable (text/plain, text/html, text/css, text/csv, text/event-stream, ...)
    return true;
  }

  // Recognize structured suffixes that are textual (e.g., application/ld+json, application/activity+json,
  // application/*+xml)
  if (subtype.endsWith(F("+json")) || subtype.endsWith(F("+xml"))) { return true; }

  // Common textual application/* types
  if (type == F("application")) {
    // Exact matches
    if (subtype == F("json") || subtype == F("xml") || subtype == F("x-www-form-urlencoded") ||
        subtype == F("javascript") || // historic but still seen
        subtype == F("yaml") || subtype == F("x-yaml") || subtype == F("toml")) {
      return true;
    }
    // Some servers send svg as application/svg+xml (less common but possible)
    if (subtype == F("svg+xml")) { return true; }
    return false;
  }

  // SVG is text-like though under image/*
  if (type == F("image") && subtype == F("svg+xml")) { return true; }

  // Everything else is assumed non-printable (e.g., image/jpeg, application/pdf, application/octet-stream, audio/*,
  // video/*)
  return false;
}

// Convenience overload for const char*
inline bool isPrintableTextContentType(const char* contentType) {
  if (!contentType) return false;
  return isPrintableTextContentType(String(contentType));
}

#endif // __GAVEL_CONTENT_TYPE_H
