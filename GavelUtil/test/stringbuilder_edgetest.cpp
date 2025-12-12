#include "../src/stringbuilder.cpp"
#include "../src/stringbuilder.h"
#include "../src/stringutils.cpp"

#include <assert.h>
#include <stdio.h>
#include <string.h>

// Helper for readable output
static void expect_eq(const char* actual, const char* expected, const char* msg) {
  if (strcmp(actual, expected) != 0) {
    printf("❌ %s\n   expected: \"%s\"\n   actual:   \"%s\"\n", msg, expected, actual);
  }
  assert(strcmp(actual, expected) == 0);
}

// Test: Self-append (sb += sb.c_str())
void test_self_append() {
  printf("Running %s...\n", __func__);
  StringBuilder sb("abc");
  sb += sb.c_str(); // Should append "abc" once, not duplicate or corrupt
  expect_eq(sb.c_str(), "abcabc", "self-append basic");

  // Fill to near capacity, then self-append
  StringBuilder sb2("xyz");
  for (int i = 0; i < 38; ++i) sb2 += "xyz"; // 3*39 = 117, nearly full
  int before = sb2.size();
  sb2 += sb2.c_str(); // Should append only up to capacity, no overflow
  int after = sb2.size();
  assert(after <= sb2.capacity());
  assert(sb2.c_str()[after] == '\0');
  printf("Self-append near capacity: before=%d after=%d\n", before, after);
}

// Test: Char append edge cases
void test_char_append() {
  printf("Running %s...\n", __func__);
  StringBuilder sb;
  sb += 'A';
  expect_eq(sb.c_str(), "65", "append printable char");

  sb.clear();
  sb += '\0'; // NUL char, should not append (or should append but string ends at first NUL)
  expect_eq(sb.c_str(), "0", "append NUL char");

  sb.clear();
  sb += '\n'; // Newline
  expect_eq(sb.c_str(), "10", "append newline char");

  sb.clear();
  sb += (char) 127; // DEL (non-printable)
  assert(sb.size() == 3);
  assert(sb.c_str()[0] == '1');

  sb.clear();
  sb += (unsigned char) 255; // Extended ASCII
  assert(sb.size() == 3);
  assert((unsigned char) sb.c_str()[0] == '2');

  // Fill to capacity with chars
  sb.clear();
  for (int i = 0; i < sb.capacity(); ++i) sb += "x";
  assert(sb.size() == sb.capacity());
  sb += "y"; // Should not append, buffer full
  assert(sb.size() == sb.capacity());
  expect_eq(sb.c_str()[sb.capacity() - 1] == 'x' ? "ok" : "fail", "ok", "char append at capacity");
}

static void test_char_decimal_basic() {
  printf("Running %s...\n", __func__);
  StringBuilder sb;

  sb += 'A'; // ASCII 65
  expect_eq(sb.c_str(), "65", "char 'A' → 65");

  sb.clear();
  sb += '\n'; // ASCII 10
  expect_eq(sb.c_str(), "10", "char '\\n' → 10");

  sb.clear();
  sb += (unsigned char) 0; // 0
  expect_eq(sb.c_str(), "0", "uchar 0 → 0");

  sb.clear();
  sb += (unsigned char) 255; // 255
  expect_eq(sb.c_str(), "255", "uchar 255 → 255");
}

// This test ensures behavior is stable regardless of whether 'char' is signed or unsigned.
static void test_char_signedness_is_neutralized() {
  printf("Running %s...\n", __func__);
  StringBuilder sb;

  // Force a value that would be negative if 'char' is signed:
  char neg_like = (char) 0xFF; // -1 on signed-char platforms
  sb += neg_like;
  expect_eq(sb.c_str(), "255", "char(0xFF) prints as 255 regardless of signedness");

  sb.clear();
  char c128 = (char) 0x80; // -128 on signed-char platforms
  sb += c128;
  expect_eq(sb.c_str(), "128", "char(0x80) prints as 128 regardless of signedness");
}

// Large sequences to ensure capacity logic remains correct for decimal char printing
static void test_char_decimal_capacity() {
  printf("Running %s...\n", __func__);
  StringBuilder sb;

  // Append 50 times '0' (ASCII 48). Each append adds "48" (2 chars).
  for (int i = 0; i < 50; ++i) sb += '0';
  // Length should be 100 or capped by capacity; string must be NUL-terminated.
  assert(sb.size() <= sb.capacity());
  assert(sb.c_str()[sb.size()] == '\0');

  // Try to overflow: keep appending until no space remains
  while (sb.remaining() > 0) sb += 'X'; // adds "88"
  int len = sb.size();
  assert(len == sb.capacity());
  assert(sb.c_str()[len] == '\0');
}

int main() {
  printf("== StringBuilder char-decimal tests ==\n");
  test_char_decimal_basic();
  test_char_signedness_is_neutralized();
  test_char_decimal_capacity();
  printf("✅ All char-decimal tests passed!\n");
  printf("== StringBuilder edge case tests ==\n");
  test_self_append();
  test_char_append();
  printf("✅ All edge case tests passed!\n");
  return 0;
}
