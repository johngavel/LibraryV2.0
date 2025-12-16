#ifndef __GAVEL_TINY_JSON_BUILDER_H
#define __GAVEL_TINY_JSON_BUILDER_H

#include <Arduino.h>

/**
 * TinyJsonBuilder — minimal JSON builder for Arduino.
 *
 * Features:
 *  - Build JSON into a String OR stream directly to a Print* (Serial, Client*, etc.).
 *  - Objects: beginObject()/endObject(), beginObject("name") for named nested objects.
 *  - Arrays:  beginArray()/endArray(), beginArray("name") for named nested arrays.
 *  - Add fields: add("name", <value>) for strings/numbers/bool/null.
 *  - Add array items: addArrayString(...), addArrayNumber(...), addArrayBool(...), addArrayNull().
 *  - Proper comma insertion using a per-level stack (safe for nesting).
 *  - Basic string escaping for quotes, backslashes, and control characters.
 *
 * Example:
 *   TinyJsonBuilder jb;
 *   jb.beginObject();
 *     jb.add("ok", true);
 *     jb.add("count", 3);
 *     jb.beginArray("values");
 *       jb.addArrayNumber(12);
 *       jb.addArrayString("foo");
 *     jb.endArray();
 *     jb.beginObject("meta");
 *       jb.add("device", "pico-w");
 *     jb.endObject();
 *   jb.endObject();
 *   Serial.println(jb.str());
 */

class TinyJsonBuilder {
public:
  explicit TinyJsonBuilder(Print* out = nullptr) : out_(out) {}

  // Switch to streaming output (saves RAM).
  void setOutput(Print* out) { out_ = out; }

  // Clear buffer and state.
  void reset() {
    buf_.remove(0);
    stackDepth_ = 0;
    for (uint8_t i = 0; i < MAX_DEPTH; ++i) firstItemStack_[i] = true;
  }

  // ---------- Object builders ----------
  // Root or unnamed nested object
  void beginObject() {
    writeChar('{');
    pushContainer();
    // For root or unnamed nested object, no parent "value complete" yet.
  }

  void endObject() {
    // Close current container
    writeChar('}');
    popContainer();
    // Mark parent (if any) as having a value written (the object itself)
    afterValueWrittenAtParent();
  }

  // Named nested object: {"name":{...}}
  void beginObject(const char* name) {
    writeFieldName(name);
    writeChar('{');
    pushContainer();
  }

  // ---------- Array builders ----------
  void beginArray() {
    writeChar('[');
    pushContainer();
  }

  void endArray() {
    writeChar(']');
    popContainer();
    afterValueWrittenAtParent();
  }

  // Named array: {"name":[...]}
  void beginArray(const char* name) {
    writeFieldName(name);
    writeChar('[');
    pushContainer();
  }

  // ---------- Field adders (for current object) ----------
  void add(const char* name, const char* value) {
    writeFieldName(name);
    writeEscapedString(value);
    afterValue();
  }

  void add(const char* name, const String& value) { add(name, value.c_str()); }

  void add(const char* name, bool value) {
    writeFieldName(name);
    writeRaw(value ? F("true") : F("false"));
    afterValue();
  }

  // Integers (explicitly cast to avoid ambiguity on Arduino)
  void add(const char* name, int value) {
    writeFieldName(name);
    writeNumber(static_cast<long>(value));
    afterValue();
  }

  void add(const char* name, unsigned value) {
    writeFieldName(name);
    writeNumber(static_cast<unsigned long>(value));
    afterValue();
  }

  void add(const char* name, long value) {
    writeFieldName(name);
    writeNumber(value);
    afterValue();
  }

  void add(const char* name, unsigned long value) {
    writeFieldName(name);
    writeNumber(value);
    afterValue();
  }

  // Floats/doubles
  void add(const char* name, float value, uint8_t digits = 2) {
    writeFieldName(name);
    writeFloat(value, digits);
    afterValue();
  }

  void add(const char* name, double value, uint8_t digits = 2) {
    writeFieldName(name);
    writeFloat(static_cast<float>(value), digits);
    afterValue();
  }

  void addNull(const char* name) {
    writeFieldName(name);
    writeRaw(F("null"));
    afterValue();
  }

  // ---------- Array item adders (for current array) ----------
  void addArrayString(const char* value) {
    writeCommaIfNeeded();
    writeEscapedString(value);
    afterValue();
  }

  void addArrayString(const String& value) { addArrayString(value.c_str()); }

  void addArrayBool(bool value) {
    writeCommaIfNeeded();
    writeRaw(value ? F("true") : F("false"));
    afterValue();
  }

  void addArrayNumber(int value) {
    writeCommaIfNeeded();
    writeNumber(static_cast<long>(value));
    afterValue();
  }

  void addArrayNumber(unsigned value) {
    writeCommaIfNeeded();
    writeNumber(static_cast<unsigned long>(value));
    afterValue();
  }

  void addArrayNumber(long value) {
    writeCommaIfNeeded();
    writeNumber(value);
    afterValue();
  }

  void addArrayNumber(unsigned long value) {
    writeCommaIfNeeded();
    writeNumber(value);
    afterValue();
  }

  void addArrayNumber(float value, uint8_t digits = 2) {
    writeCommaIfNeeded();
    writeFloat(value, digits);
    afterValue();
  }

  void addArrayNull() {
    writeCommaIfNeeded();
    writeRaw(F("null"));
    afterValue();
  }

  // ---------- Output access ----------
  const String& str() const { return buf_; }

private:
  // ---------- Container state ----------
  static constexpr uint8_t MAX_DEPTH = 16;

  void pushContainer() {
    if (stackDepth_ < MAX_DEPTH) {
      // On entering a new container, its "first item" flag starts true.
      firstItemStack_[stackDepth_] = true;
      stackDepth_++;
    }
  }

  void popContainer() {
    if (stackDepth_ > 0) { stackDepth_--; }
  }

  // Mark that the current container now has at least one item written.
  void afterValue() {
    if (stackDepth_ > 0) firstItemStack_[stackDepth_ - 1] = false;
  }

  // When closing a container that was a value of its parent (named object/array),
  // mark the parent as having received its value.
  void afterValueWrittenAtParent() {
    if (stackDepth_ > 0) {
      // We just popped; parent is at stackDepth_ - 1 after pop.
      firstItemStack_[stackDepth_ - 1] = false;
    }
  }

  // ---------- Write helpers ----------
  void writeCommaIfNeeded() {
    if (stackDepth_ == 0) return; // root not inside a container value
    // If current container already has items, prepend comma before next value
    if (!firstItemStack_[stackDepth_ - 1]) { writeChar(','); }
  }

  void writeFieldName(const char* name) {
    // Objects: if this isn't the first field, write a comma
    writeCommaIfNeeded();
    writeEscapedString(name);
    writeChar(':');
    // Do NOT mark "afterValue" here—value follows immediately and will mark it.
    // This keeps logic consistent for nested containers as values.
  }

  void writeChar(char c) {
    if (out_)
      out_->print(c);
    else {
      char s[2] = {c, 0};
      buf_ += s;
    }
  }

  void writeRaw(const __FlashStringHelper* fsh) {
    if (out_)
      out_->print(fsh);
    else
      buf_ += String(fsh);
  }

  void writeRaw(const String& s) {
    if (out_)
      out_->print(s);
    else
      buf_ += s;
  }

  // Number writers (long/unsigned long are primary)
  void writeNumber(long v) { writeRaw(String(v)); }
  void writeNumber(unsigned long v) { writeRaw(String(v)); }

  // Forwarders to remove ambiguity with int/unsigned on AVR/ARM
  void writeNumber(int v) { writeNumber(static_cast<long>(v)); }
  void writeNumber(unsigned int v) { writeNumber(static_cast<unsigned long>(v)); }

  void writeFloat(float value, uint8_t digits) { writeRaw(String(value, digits)); }

  void writeEscapedString(const char* s) {
    writeChar('"');
    for (const char* p = s ? s : ""; *p; ++p) {
      char c = *p;
      switch (c) {
      case '"': writeRaw(F("\\\"")); break;
      case '\\': writeRaw(F("\\\\")); break;
      case '\n': writeRaw(F("\\n")); break;
      case '\r': writeRaw(F("\\r")); break;
      case '\t': writeRaw(F("\\t")); break;
      default:
        // Escape ASCII control characters as \u00XX
        if ((uint8_t) c < 0x20) {
          char hex[7]; // \u00XX + NUL; we output full 4 hex digits
          sprintf(hex, "\\u%04X", (unsigned) c);
          writeRaw(String(hex));
        } else {
          writeChar(c);
        }
        break;
      }
    }
    writeChar('"');
  }

private:
  Print* out_ = nullptr;             // optional streaming target
  String buf_;                       // in-memory buffer when out_ == nullptr
  uint8_t stackDepth_ = 0;           // current nesting depth
  bool firstItemStack_[MAX_DEPTH]{}; // per-level comma flags
};

#endif // __GAVEL_TINY_JSON_BUILDER_H