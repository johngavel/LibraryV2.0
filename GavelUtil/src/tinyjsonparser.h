
#ifndef __GAVEL_TINY_JSON_PARSER_H
#define __GAVEL_TINY_JSON_PARSER_H
#include <Arduino.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
/**
 * TinyJsonParser — minimal, event-driven JSON parser for Arduino.
 *
 * Goals (complimentary to TinyJsonBuilder):
 *  - Small, header-only, no dynamic allocations by default
 *  - Works with in-memory JSON (const char*) and streaming (Stream&)
 *  - Emits SAX-style events so you can react without building a DOM
 *  - Understands objects, arrays, strings, numbers, true/false/null
 *  - Handles common escapes (\" \\ \n \r \t). \uXXXX is passed through.
 *
 * Usage (in-memory):
 *   TinyJsonParser::Events ev;
 *   ev.onKey    = [](void* ctx, const char* s){ Serial.print("key:"); Serial.println(s); };
 *   ev.onString = [](void* ctx, const char* s){ Serial.print("str:"); Serial.println(s); };
 *   ev.onNumber = [](void* ctx, double v){ Serial.print("num:"); Serial.println(v, 6); };
 *   TinyJsonParser p;
 *   p.parse("{\"ok\":true,\"vals\":[12,\"foo\"],\"meta\":{\"device\":\"pico-w\"}}", ev);
 *
 * Usage (streaming):
 *   TinyJsonParser::Events ev = ...;
 *   TinyJsonParser p;
 *   p.parse(Serial, ev, 64, 1000);
 */
class TinyJsonParser {
public:
  struct Events {
    void (*onObjectStart)(void* ctx) = nullptr;
    void (*onObjectEnd)(void* ctx) = nullptr;
    void (*onArrayStart)(void* ctx) = nullptr;
    void (*onArrayEnd)(void* ctx) = nullptr;

    void (*onKey)(void* ctx, const char* key) = nullptr;      // object field name
    void (*onString)(void* ctx, const char* value) = nullptr; // string value
    void (*onNumber)(void* ctx, double value) = nullptr;      // numeric value
    void (*onBool)(void* ctx, bool value) = nullptr;          // true/false
    void (*onNull)(void* ctx) = nullptr;                      // null

    void (*onError)(void* ctx, const char* message) = nullptr; // parse error
    void* user = nullptr;                                      // opaque user data
  };

  // Configuration
  static constexpr uint8_t MAX_DEPTH = 16; // nesting depth guard

  TinyJsonParser() {}

  // -------- In-memory parsing --------
  bool parse(const char* json, Events& ev) {
    if (!json) json = "";
    const char* p = json;
    depth_ = 0;
    return parseCore(
        [&]() -> int {
          unsigned char c = (unsigned char) (*p);
          return c ? (p++, c) : -1; // -1 signals end
        },
        [&]() -> void {
          // no-op for in-memory: nothing to unread
        },
        [&]() -> bool { return *p != '\0'; }, ev);
  }

  // -------- Streaming parsing (reads from Arduino Stream) --------
  bool parse(Stream& in, Events& ev, size_t maxTokenLen = 64, uint32_t timeoutMs = 1000) {
    tokenBufLen_ = (maxTokenLen > 0 ? (maxTokenLen < sizeof(tokenBuf_) ? maxTokenLen : sizeof(tokenBuf_)) : 64);
    depth_ = 0;
    uint32_t deadline = millis() + timeoutMs;

    auto nextCh = [&]() -> int {
      while (!in.available()) {
        if (timeoutMs && millis() > deadline) return -1;
        delay(1);
      }
      return in.read();
    };
    auto unread = [&]() -> void { /* Stream has no unread; we keep simple */ };
    auto hasMore = [&]() -> bool { return in.available() > 0; };
    return parseCore(nextCh, unread, hasMore, ev);
  }

private:
  // -------- Core parser (pulls via lambdas) --------
  template <typename NextCh, typename Unread, typename HasMore>
  bool parseCore(NextCh nextCh, Unread unread, HasMore hasMore, Events& ev) {
    skipWs(nextCh);
    int c = nextCh();
    if (c == -1) return error(ev, "empty input");
    unread(); // we consumed one when peeking; push back logically

    if (!parseValue(nextCh, unread, hasMore, ev)) return false;
    skipWs(nextCh);
    // must be end or only trailing whitespace
    c = nextCh();
    if (c != -1) {
      // allow trailing whitespace
      while (c != -1 && isspace(c)) c = nextCh();
      if (c != -1) return error(ev, "trailing characters after value");
    }
    return true;
  }

  template <typename NextCh> void skipWs(NextCh nextCh) {
    int c;
    do { c = nextCh(); } while (c != -1 && isspace(c));
    // we cannot unread universally; callers may simulate with their Unread
  }

  template <typename NextCh, typename Unread, typename HasMore>
  bool parseValue(NextCh nextCh, Unread unread, HasMore hasMore, Events& ev) {
    int c = readNonWs(nextCh);
    if (c == -1) return error(ev, "unexpected end of input");

    switch (c) {
    case '{': return parseObject(nextCh, unread, hasMore, ev);
    case '[': return parseArray(nextCh, unread, hasMore, ev);
    case '\"': return parseString(nextCh, ev, /*isKey*/ false);
    case 't':
    case 'f':
    case 'n': return parseLiteral(nextCh, c, ev);
    default:
      if (c == '-' || isdigit(c)) return parseNumber(nextCh, c, ev);
      return error(ev, "invalid value start");
    }
  }

  template <typename NextCh> int readNonWs(NextCh nextCh) {
    int c;
    do { c = nextCh(); } while (c != -1 && isspace(c));
    return c;
  }

  template <typename NextCh, typename Unread, typename HasMore>
  bool parseObject(NextCh nextCh, Unread unread, HasMore hasMore, Events& ev) {
    if (!pushDepth(ev)) return false;
    if (ev.onObjectStart) ev.onObjectStart(ev.user);
    // object: { ( string : value )* }
    bool first = true;
    for (;;) {
      int c = readNonWs(nextCh);
      if (c == '}') {
        if (!popDepth(ev)) return false;
        if (ev.onObjectEnd) ev.onObjectEnd(ev.user);
        return true;
      }
      if (!first) {
        if (c != ',') return error(ev, "expected ',' between object fields");
        c = readNonWs(nextCh);
      }
      if (c != '\"') return error(ev, "expected '\"' starting field name");
      if (!parseString(nextCh, ev, /*isKey*/ true)) return false;
      c = readNonWs(nextCh);
      if (c != ':') return error(ev, "expected ':' after field name");
      if (!parseValue(nextCh, unread, hasMore, ev)) return false;
      first = false;
    }
  }

  template <typename NextCh, typename Unread, typename HasMore>
  bool parseArray(NextCh nextCh, Unread unread, HasMore hasMore, Events& ev) {
    if (!pushDepth(ev)) return false;
    if (ev.onArrayStart) ev.onArrayStart(ev.user);
    bool first = true;
    for (;;) {
      int c = readNonWs(nextCh);
      if (c == ']') {
        if (!popDepth(ev)) return false;
        if (ev.onArrayEnd) ev.onArrayEnd(ev.user);
        return true;
      }
      if (!first) {
        if (c != ',') return error(ev, "expected ',' between array items");
        c = readNonWs(nextCh);
      }
      // unread not available generically; we dispatch based on c
      if (c == '{') {
        if (!parseObject(nextCh, unread, hasMore, ev)) return false;
      } else if (c == '[') {
        if (!parseArray(nextCh, unread, hasMore, ev)) return false;
      } else if (c == '\"') {
        if (!parseString(nextCh, ev, false)) return false;
      } else if (c == 't' || c == 'f' || c == 'n') {
        if (!parseLiteral(nextCh, c, ev)) return false;
      } else if (c == '-' || isdigit(c)) {
        if (!parseNumber(nextCh, c, ev)) return false;
      } else
        return error(ev, "invalid array item");
      first = false;
    }
  }

  template <typename NextCh> bool parseString(NextCh nextCh, Events& ev, bool isKey) {
    // starting '\"' already consumed
    size_t i = 0;
    for (;;) {
      int c = nextCh();
      if (c == -1) return error(ev, "unterminated string");
      if (c == '\"') break;
      if (c == '\\') {
        c = nextCh();
        if (c == -1) return error(ev, "bad escape at end");
        switch (c) {
        case '\"': addChar('\"', i); break;
        case '\\': addChar('\\', i); break;
        case '/': addChar('/', i); break;
        case 'n': addChar('\n', i); break;
        case 'r': addChar('\r', i); break;
        case 't': addChar('\t', i); break;
        case 'b': addChar('\b', i); break;
        case 'f': addChar('\f', i); break;
        case 'u': {
          // Pass through literally as \uXXXX to avoid heavy UTF-8 logic
          addChar('\\', i);
          addChar('u', i);
          for (int k = 0; k < 4; k++) {
            int h = nextCh();
            if (h == -1) return error(ev, "bad \\u escape");
            addChar((char) h, i);
          }
        } break;
        default: return error(ev, "unsupported escape");
        }
      } else {
        addChar((char) c, i);
      }
      if (i >= tokenBufLen_ - 1) return error(ev, "string token too long");
    }
    tokenBuf_[i] = '\0';
    if (isKey) {
      if (ev.onKey) ev.onKey(ev.user, tokenBuf_);
    } else {
      if (ev.onString) ev.onString(ev.user, tokenBuf_);
    }
    return true;
  }

  template <typename NextCh> bool parseLiteral(NextCh nextCh, int first, Events& ev) {
    // first is one of 't','f','n'
    if (first == 't') {
      if (!expectWord(nextCh, "rue")) return error(ev, "bad 'true'");
      if (ev.onBool) ev.onBool(ev.user, true);
      return true;
    } else if (first == 'f') {
      if (!expectWord(nextCh, "alse")) return error(ev, "bad 'false'");
      if (ev.onBool) ev.onBool(ev.user, false);
      return true;
    } else {
      if (!expectWord(nextCh, "ull")) return error(ev, "bad 'null'");
      if (ev.onNull) ev.onNull(ev.user);
      return true;
    }
  }

  template <typename NextCh> bool parseNumber(NextCh nextCh, int first, Events& ev) {
    // Collect into tokenBuf_, then strtod
    size_t i = 0;
    addChar((char) first, i);
    for (;;) {
      int c = nextCh();
      if (c == -1) break;
      if (isdigit(c) || c == '+' || c == '-' || c == '.' || c == 'e' || c == 'E') {
        addChar((char) c, i);
        if (i >= tokenBufLen_ - 1) return error(ev, "number token too long");
      } else { // delimiter
        // simple pushback emulation: we cannot unread universally; assume caller accounts for delimiter via
        // higher-level logic In our design, callers re-read from last point, so we do nothing.
        break;
      }
    }
    tokenBuf_[i] = '\0';
    char* endp = nullptr;
    double val = strtod(tokenBuf_, &endp);
    if (endp == tokenBuf_) return error(ev, "invalid number");
    if (ev.onNumber) ev.onNumber(ev.user, val);
    return true;
  }

  template <typename NextCh> bool expectWord(NextCh nextCh, const char* tail) {
    for (const char* p = tail; *p; ++p) {
      int c = nextCh();
      if (c != *p) return false;
    }
    return true;
  }

  bool pushDepth(Events& ev) {
    if (depth_ >= MAX_DEPTH) return error(ev, "nesting too deep");
    depth_++;
    return true;
  }
  bool popDepth(Events& ev) {
    if (depth_ == 0) return error(ev, "mismatched container end");
    depth_--;
    return true;
  }

  bool error(Events& ev, const char* msg) {
    if (ev.onError) ev.onError(ev.user, msg);
    return false;
  }

  void addChar(char c, size_t& i) {
    if (i < tokenBufLen_ - 1) tokenBuf_[i++] = c;
  }

private:
  uint8_t depth_ = 0;
  size_t tokenBufLen_ = sizeof(tokenBuf_);
  char tokenBuf_[128]; // default max token size for streaming; truncated if smaller
};

#endif // __GAVEL_TINY_JSON_PARSER_H
