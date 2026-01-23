#ifndef __GAVEL_API_H
#define __GAVEL_API_H
// api.h — C-library–only query parser, persistent storage (no <stddef.h>)

#include <GavelInterfaces.h> // JsonInterface
#include <GavelUtil.h>
#include <ctype.h>  // isxdigit, tolower
#include <stdlib.h> // NULL
#include <string.h> // strlen, memcpy, strchr, strcmp

class QueryString {
public:
  enum { MAX_PARAMS = 32, STORAGE_SIZE = 1024 };

  QueryString() : used_(0u), nparams_(0u) { storage_[0] = '\0'; }

  // Parses and stores the query string for later access.
  // Accepts with or without leading '?'.
  // Returns false on invalid percent-encoding or capacity overflow.
  bool processQueryString(const char* query) {
    clear();
    if (query == NULL) return true;

    // Skip leading '?'
    if (query[0] == '?') ++query;

    // Copy into our storage buffer (truncate if needed)
    unsigned int len = (unsigned int) strlen(query);
    if (len >= (unsigned int) STORAGE_SIZE) len = (unsigned int) STORAGE_SIZE - 1u;
    memcpy(storage_, query, len);
    storage_[len] = '\0';
    used_ = len;

    // Stop at fragment '#'
    char* frag = strchr(storage_, '#');
    if (frag) *frag = '\0';

    // Iterate `&`-separated segments
    char* s = storage_;
    while (s && *s) {
      if (nparams_ >= (unsigned int) MAX_PARAMS) return false;

      char* part = s;
      char* amp = strchr(s, '&');
      if (amp) {
        *amp = '\0';
        s = amp + 1;
      } else {
        s = NULL;
      }

      if (*part == '\0') continue; // skip empty

      // Split on first '=' into key/value
      char* eq = strchr(part, '=');
      char* k = part;
      char* v = (eq ? (*eq = '\0', eq + 1) : (char*) "");

      if (!percentDecodeInPlace(k, 1)) return false; // form-style '+'
      if (!percentDecodeInPlace(v, 1)) return false;

      params_[nparams_].key = k;
      params_[nparams_].value = v;
      ++nparams_;
    }
    return true;
  }

  // Lookups
  const char* getFirst(const char* key) const {
    for (unsigned int i = 0; i < nparams_; ++i)
      if (strcmp(params_[i].key, key) == 0) return params_[i].value;
    return NULL;
  }

  unsigned int count(const char* key) const {
    unsigned int c = 0u;
    for (unsigned int i = 0; i < nparams_; ++i)
      if (strcmp(params_[i].key, key) == 0) ++c;
    return c;
  }

  const char* getAt(const char* key, unsigned int index) const {
    for (unsigned int i = 0; i < nparams_; ++i) {
      if (strcmp(params_[i].key, key) == 0) {
        if (index == 0u) return params_[i].value;
        --index;
      }
    }
    return NULL;
  }

  // Raw iteration if needed
  unsigned int size() const { return nparams_; }
  const char* keyAt(unsigned int i) const { return (i < nparams_) ? params_[i].key : NULL; }
  const char* valueAt(unsigned int i) const { return (i < nparams_) ? params_[i].value : NULL; }

  void clear() {
    used_ = 0u;
    nparams_ = 0u;
    storage_[0] = '\0';
  }

private:
  struct Pair {
    char* key;
    char* value;
  };

  static int isHex(char c) {
    unsigned char uc = (unsigned char) c;
    return (uc >= '0' && uc <= '9') || (uc >= 'A' && uc <= 'F') || (uc >= 'a' && uc <= 'f');
  }

  static unsigned int hexToNibble(char c) {
    unsigned char uc = (unsigned char) c;
    if (uc >= '0' && uc <= '9') return (unsigned int) (uc - '0');
    if (uc >= 'A' && uc <= 'F') return 10u + (unsigned int) (uc - 'A');
    return 10u + (unsigned int) (uc - 'a');
  }

  // Decode %XX in place; if form_style != 0, convert '+' to ' '.
  static int percentDecodeInPlace(char* s, int form_style) {
    char* r = s;
    char* w = s;
    while (*r) {
      if (form_style && *r == '+') {
        *w++ = ' ';
        ++r;
      } else if (*r == '%') {
        char h1 = *(r + 1);
        char h2 = *(r + 2);
        if (!isHex(h1) || !isHex(h2)) return 0; // invalid escape
        *w++ = (char) ((hexToNibble(h1) << 4) | hexToNibble(h2));
        r += 3;
      } else {
        *w++ = *r++;
      }
    }
    *w = '\0';
    return 1;
  }

private:
  char storage_[STORAGE_SIZE];
  unsigned int used_;
  Pair params_[MAX_PARAMS];
  unsigned int nparams_;
};

class MetaHeaders {
public:
  enum { MAX_HEADERS = 32, STORAGE_SIZE = 1024 };

  MetaHeaders() : used_(0u), count_(0u) { storage_[0] = '\0'; }

  void clear() {
    used_ = 0u;
    count_ = 0u;
    storage_[0] = '\0';
  }

  // Add "key: value" or replace if exists
  bool set(const char* key, const char* value) {
    unsigned int klen = (unsigned int) strlen(key);
    unsigned int vlen = (unsigned int) strlen(value);

    unsigned int needed = klen + 1 + vlen + 1; // "key\0value\0"

    if (used_ + needed >= (unsigned int) STORAGE_SIZE) return false;

    // Check if key exists → replace
    for (unsigned int i = 0; i < count_; ++i) {
      if (strcmp(pairs_[i].key, key) == 0) {
        strcpy(pairs_[i].value, value);
        return true;
      }
    }

    if (count_ >= (unsigned int) MAX_HEADERS) return false;

    char* kptr = storage_ + used_;
    memcpy(kptr, key, klen + 1);
    used_ += klen + 1;

    char* vptr = storage_ + used_;
    memcpy(vptr, value, vlen + 1);
    used_ += vlen + 1;

    pairs_[count_].key = kptr;
    pairs_[count_].value = vptr;
    ++count_;

    return true;
  }

  // Returns nullptr if not found
  const char* get(const char* key) const {
    for (unsigned int i = 0; i < count_; ++i) {
      if (strcmp(pairs_[i].key, key) == 0) return pairs_[i].value;
    }
    return NULL;
  }

  unsigned int size() const { return count_; }
  const char* keyAt(unsigned int i) const { return (i < count_) ? pairs_[i].key : NULL; }
  const char* valueAt(unsigned int i) const { return (i < count_) ? pairs_[i].value : NULL; }

private:
  struct Pair {
    char* key;
    char* value;
  };

  char storage_[STORAGE_SIZE];
  unsigned int used_;
  Pair pairs_[MAX_HEADERS];
  unsigned int count_;
};

class Method {
public:
  enum { MAX_LEN = 16 }; // Plenty for: GET, POST, PUT, DELETE, PATCH...

  Method() { method_[0] = '\0'; }
  void clear() { method_[0] = '\0'; }

  // Set method (e.g., "GET", "POST")
  // Returns false if too long.
  bool set(const char* m) {
    if (m == NULL) {
      method_[0] = '\0';
      return true;
    }
    unsigned int len = (unsigned int) strlen(m);
    if (len >= (unsigned int) MAX_LEN) return false;
    memcpy(method_, m, len + 1);
    return true;
  }

  // Get method string
  const char* get() const { return method_; }

  // Compare method (case‑sensitive)
  bool equals(const char* m) const { return strcmp(method_, m) == 0; }

  // Quick helpers
  bool isGET() const { return equals("GET"); }
  bool isPOST() const { return equals("POST"); }
  bool isPUT() const { return equals("PUT"); }
  bool isDELETE() const { return equals("DELETE"); }
  bool isPATCH() const { return equals("PATCH"); }

private:
  char method_[MAX_LEN];
};

class Body {
public:
  Body(unsigned int bufferSize)
      : _bufSize(bufferSize), _buffer(new unsigned char[_bufSize]), ringBuffer(_buffer, _bufSize) {}

  // Clear content in ring (does not free buffer)
  void clear() { ringBuffer.clear(); }

  // Byte-oriented streaming operations
  int available() { return ringBuffer.available(); }
  int size() { return ringBuffer.available(); }
  int read() { return ringBuffer.pop(); }
  int peek() { return ringBuffer.peek(); }
  int read(unsigned char* out, int n) { return ringBuffer.read(out, n); }
  unsigned int capacity() const { return _bufSize; }

  // Writes return number of bytes written
  unsigned int write(const unsigned char* src, unsigned int n) { return (unsigned int) ringBuffer.write(src, (int) n); }
  unsigned int write(unsigned char c) { return (unsigned int) ringBuffer.push(c); }

  ~Body() {
    delete[] _buffer;
    _buffer = 0;
    _bufSize = 0;
  }

  // Non-copyable
  Body(const Body&) = delete;
  Body& operator=(const Body&) = delete;

protected:
  unsigned int _bufSize;
  unsigned char* _buffer;
  CharRingBuffer ringBuffer;

private:
};

class API : public JsonInterface {
public:
  static const unsigned int DEFAULT_BUFFER_SIZE = 2048;
  static const unsigned int LARGE_BUFFER_SIZE = 16384;
  API() : API(DEFAULT_BUFFER_SIZE){};
  API(unsigned int bufferSize) : body_(bufferSize){};
  virtual JsonDocument createJson() = 0;
  virtual bool parseJson(JsonDocument& doc) = 0;

  void clear() {
    method_.clear();
    query_.clear();
    metaHeaders_.clear();
    body_.clear();
  }

  Method method_;
  QueryString query_;
  MetaHeaders metaHeaders_;
  Body body_;
};

#endif // __GAVEL_API_H