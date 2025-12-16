#ifndef __GAVEL_TINY_JSON_PARSER_H
#define __GAVEL_TINY_JSON_PARSER_H

#include <Arduino.h>

/**
 * TinyJsonParser — minimal, event-driven JSON parser for Arduino.
 *
 * Goals:
 *  - Very small footprint, no dynamic allocation.
 *  - Streaming input from a char* buffer or a Stream* (Serial, WiFiClient, etc.).
 *  - Event callbacks for structure and values (SAX style).
 *  - Handles objects, arrays, strings, numbers, booleans, and null.
 *  - Tolerates reasonable nesting (configurable) and UTF-8 bytes in strings.
 *
 * Non-goals:
 *  - Full JSON Schema validation.
 *  - Big-number and exact floating parsing; numbers are reported as slices.
 *
 * Usage (buffer):
 *    TinyJsonParser parser;
 *    struct MyHandler : TinyJsonHandler { /* override callbacks */ }
h;
*parser.setHandler(&h);
*parser.parse("{\"ok\":true,\"n\":12,\"s\":\"hi\"}");
**Usage(stream) : *parser.setInput(&Serial);
*parser.parse(); // reads until eof (or bytesAvailable == 0)
**Integration with TinyJsonBuilder : *-TinyJsonBuilder builds outbound JSON;
TinyJsonParser parses inbound JSON.*-Numbers are passed to callbacks as(const char* slice, size_t len) to avoid* float /
    double conversion costs on small MCUs.Helpers are provided.*/

    // ---------------------------------------------------------------
    // Handler interface (override what you need)
    // ---------------------------------------------------------------
    struct TinyJsonHandler {
  virtual void onDocumentStart() {}
  virtual void onDocumentEnd() {}

  virtual void onObjectStart() {}
  virtual void onObjectEnd() {}
  virtual void onArrayStart() {}
  virtual void onArrayEnd() {}

  // Called before a value in an object: key is provided.
  // key is a zero-terminated Arduino String-view (temporary); if streaming, it
  // is built incrementally and valid only for the duration of the callback.
  virtual void onKey(const char* key) {}

  // Value callbacks
  virtual void onString(const char* value) {}
  virtual void onNumberSlice(const char* begin, size_t len) {}
  virtual void onBool(bool value) {}
  virtual void onNull() {}

  virtual ~TinyJsonHandler() {}
};

// ---------------------------------------------------------------
// Parser
// ---------------------------------------------------------------
class TinyJsonParser {
public:
  explicit TinyJsonParser(Stream* in = nullptr) : in_(in) {}

  void setInput(Stream* in) { in_ = in; }
  void setHandler(TinyJsonHandler* h) { handler_ = h; }

  // Configuration
  void setMaxDepth(uint8_t d) { maxDepth_ = d; }

  // Parse from a null-terminated buffer.
  bool parse(const char* json) {
    reset();
    src_ = json;
    streaming_ = false;
    return parseImpl();
  }

  // Parse from the configured Stream.
  bool parse() {
    if (!in_) return false;
    reset();
    streaming_ = true;
    return parseImpl();
  }

  // Helpers: convert a number slice to long/unsigned long/float
  static bool toLong(const char* b, size_t n, long* out) {
    // Simple base-10 parsing; handles sign
    long sign = 1;
    size_t i = 0;
    if (n && (b[0] == '-' || b[0] == '+')) {
      sign = (b[0] == '-' ? -1 : 1);
      i++;
    }
    long val = 0;
    for (; i < n; ++i) {
      char c = b[i];
      if (c < '0' || c > '9') return false; // stop on non-digit
      val = val * 10 + (c - '0');
    }
    *out = val * sign;
    return true;
  }

  static bool toULong(const char* b, size_t n, unsigned long* out) {
    unsigned long val = 0;
    for (size_t i = 0; i < n; ++i) {
      char c = b[i];
      if (c < '0' || c > '9') return false;
      val = val * 10 + (c - '0');
    }
    *out = val;
    return true;
  }

  static bool toFloat(const char* b, size_t n, float* out) {
    // Fallback: use Arduino String for compact conversion
    String s;
    s.reserve(n);
    for (size_t i = 0; i < n; ++i) s += b[i];
    *out = s.toFloat();
    return true; // String::toFloat returns 0 on failure, caller can validate
  }

private:
  enum class Tok { LBrace, RBrace, LBracket, RBracket, Colon, Comma, String, Number, True, False, Null, End, Bad };

  struct State {
    enum Ctx { InNone, InObject, InArray } ctx = InNone;
  };

  // Reset parser state
  void reset() {
    depth_ = 0;
    hadError_ = false;
    buf_.remove(0);
  }

  // Core parse loop
  bool parseImpl() {
    if (handler_) handler_->onDocumentStart();
    Tok t;
    State::Ctx ctx = State::InNone;
    bool expectingValue = true; // after key or at start of array
    bool inObjectKey = false;

    // Stack remembers whether the current container is Object or Array
    for (;;) {
      t = nextToken();
      if (t == Tok::Bad) {
        hadError_ = true;
        break;
      }
      if (t == Tok::End) break;

      switch (t) {
      case Tok::LBrace: {
        if (!push(State::InObject)) {
          hadError_ = true;
          goto done;
        }
        if (handler_) handler_->onObjectStart();
        ctx = State::InObject;
        inObjectKey = true;     // first thing in object can be a key or close
        expectingValue = false; // we expect a key first
      } break;
      case Tok::RBrace: {
        if (!pop(State::InObject)) {
          hadError_ = true;
          goto done;
        }
        if (handler_) handler_->onObjectEnd();
        ctx = containerCtx();
        // after closing a container, next state depends on parent
        inObjectKey = (ctx == State::InObject);
        expectingValue = (ctx == State::InArray);
      } break;
      case Tok::LBracket: {
        if (!push(State::InArray)) {
          hadError_ = true;
          goto done;
        }
        if (handler_) handler_->onArrayStart();
        ctx = State::InArray;
        expectingValue = true;
        inObjectKey = false;
      } break;
      case Tok::RBracket: {
        if (!pop(State::InArray)) {
          hadError_ = true;
          goto done;
        }
        if (handler_) handler_->onArrayEnd();
        ctx = containerCtx();
        inObjectKey = (ctx == State::InObject);
        expectingValue = (ctx == State::InArray);
      } break;
      case Tok::Comma: {
        // comma separates items; context determines expectation
        if (ctx == State::InObject) {
          inObjectKey = true;
          expectingValue = false;
        } else if (ctx == State::InArray) {
          expectingValue = true;
        }
      } break;
      case Tok::Colon: {
        // colon comes after a key; next token must be a value
        expectingValue = true;
        inObjectKey = false;
      } break;
      case Tok::String: {
        if (inObjectKey) {
          if (handler_) handler_->onKey(buf_.c_str());
          inObjectKey = false; // value should follow after colon
        } else {
          if (handler_) handler_->onString(buf_.c_str());
          expectingValue = (ctx == State::InArray);
        }
        buf_.remove(0);
      } break;
      case Tok::Number: {
        if (handler_) handler_->onNumberSlice(buf_.c_str(), buf_.length());
        buf_.remove(0);
        expectingValue = (ctx == State::InArray);
      } break;
      case Tok::True: {
        if (handler_) handler_->onBool(true);
        expectingValue = (ctx == State::InArray);
      } break;
      case Tok::False: {
        if (handler_) handler_->onBool(false);
        expectingValue = (ctx == State::InArray);
      } break;
      case Tok::Null: {
        if (handler_) handler_->onNull();
        expectingValue = (ctx == State::InArray);
      } break;
      default: break;
      }
    }
  done:
    if (handler_) handler_->onDocumentEnd();
    return !hadError_;
  }

  // Container stack
  bool push(State::Ctx c) {
    if (depth_ >= maxDepth_) return false;
    stack_[depth_++] = c;
    return true;
  }
  bool pop(State::Ctx c) {
    if (depth_ == 0) return false;
    if (stack_[depth_ - 1] != c) return false;
    --depth_;
    return true;
  }
  State::Ctx containerCtx() const {
    if (depth_ == 0) return State::InNone;
    return stack_[depth_ - 1];
  }

  // Tokenizer
  Tok nextToken() {
    skipWs();
    int ch = read();
    if (ch < 0) return Tok::End;
    switch (ch) {
    case '{': return Tok::LBrace;
    case '}': return Tok::RBrace;
    case '[': return Tok::LBracket;
    case ']': return Tok::RBracket;
    case ':': return Tok::Colon;
    case ',': return Tok::Comma;
    case '"': return readString();
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': unread(ch); return readNumber();
    case 't': return readKeyword("rue", Tok::True);
    case 'f': return readKeyword("alse", Tok::False);
    case 'n': return readKeyword("ull", Tok::Null);
    default: return Tok::Bad;
    }
  }

  void skipWs() {
    for (;;) {
      int ch = peek();
      if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t') {
        read();
        continue;
      }
      break;
    }
  }

  Tok readString() {
    buf_.remove(0);
    for (;;) {
      int ch = read();
      if (ch < 0) return Tok::Bad; // unterminated
      if (ch == '"') return Tok::String;
      if (ch == '\\') {
        int esc = read();
        if (esc < 0) return Tok::Bad;
        switch (esc) {
        case '"': buf_ += '"'; break;
        case '\\': buf_ += '\\'; break;
        case '/': buf_ += '/'; break;
        case 'b': buf_ += '\b'; break;
        case 'f': buf_ += '\f'; break;
        case 'n': buf_ += '\n'; break;
        case 'r': buf_ += '\r'; break;
        case 't': buf_ += '\t'; break;
        case 'u': {
          // Read 4 hex digits; store raw as UTF-8 via simple pass-through for ASCII
          char hex[5];
          for (int i = 0; i < 4; ++i) {
            int h = read();
            if (h < 0) return Tok::Bad;
            hex[i] = (char) h;
          }
          hex[4] = 0;
          // Convert basic BMP ASCII subset (<=0x7F) to char; otherwise keep as '?'
          unsigned val = strtoul(hex, nullptr, 16);
          if (val <= 0x7F)
            buf_ += (char) val;
          else
            buf_ += '?';
        } break;
        default: return Tok::Bad;
        }
      } else {
        buf_ += (char) ch;
      }
    }
  }

  Tok readKeyword(const char* tail, Tok as) {
    for (const char* p = tail; *p; ++p) {
      int ch = read();
      if (ch != *p) return Tok::Bad;
    }
    return as;
  }

  Tok readNumber() {
    buf_.remove(0);
    bool hasDot = false, hasExp = false;
    int ch = read();
    if (ch == '-') {
      buf_ += '-';
      ch = read();
    }
    if (ch < 0) return Tok::Bad;
    if (!isDigit(ch)) return Tok::Bad;
    // integer part
    if (ch == '0') {
      buf_ += '0';
      ch = peek();
    } else {
      do {
        buf_ += (char) ch;
        ch = peek();
      } while (isDigit(ch) && (read(), true));
    }
    // fraction
    if (ch == '.') {
      hasDot = true;
      read();
      buf_ += '.';
      ch = peek();
      if (!isDigit(ch)) return Tok::Bad;
      while (isDigit(ch)) {
        buf_ += (char) ch;
        read();
        ch = peek();
      }
    }
    // exponent
    if (ch == 'e' || ch == 'E') {
      hasExp = true;
      read();
      buf_ += 'e';
      ch = peek();
      if (ch == '+' || ch == '-') {
        buf_ += (char) ch;
        read();
        ch = peek();
      }
      if (!isDigit(ch)) return Tok::Bad;
      while (isDigit(ch)) {
        buf_ += (char) ch;
        read();
        ch = peek();
      }
    }
    return Tok::Number;
  }

  // ---------------------- I/O helpers ----------------------
  int peek() {
    if (streaming_) {
      if (!in_) return -1;
      while (in_->available() == 0) return -1;
      int ch = in_->peek();
      return ch;
    } else {
      if (!src_ || *src_ == 0) return -1;
      return (int) (uint8_t) (*src_);
    }
  }

  int read() {
    if (streaming_) {
      if (!in_) return -1;
      while (in_->available() == 0) return -1;
      return in_->read();
    } else {
      if (!src_ || *src_ == 0) return -1;
      int ch = (int) (uint8_t) (*src_);
      ++src_;
      return ch;
    }
  }

  void unread(int ch) {
    if (streaming_) {
      // Stream cannot unread; we rely on peek() before read() when needed.
      // Here, do nothing.
      (void) ch;
    } else {
      if (src_) --src_;
    }
  }

  static bool isDigit(int ch) { return ch >= '0' && ch <= '9'; }

private:
  Stream* in_ = nullptr;
  TinyJsonHandler* handler_ = nullptr;
  bool streaming_ = false;
  const char* src_ = nullptr; // buffer mode

  // state
  static constexpr uint8_t DEFAULT_MAX_DEPTH = 16;
  uint8_t maxDepth_ = DEFAULT_MAX_DEPTH;
  uint8_t depth_ = 0;
  State::Ctx stack_[DEFAULT_MAX_DEPTH];

  bool hadError_ = false;
  String buf_; // temporary buffer for strings/numbers
};

#endif // __GAVEL_TINY_JSON_PARSER_H
