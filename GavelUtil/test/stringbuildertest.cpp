#include "../src/stringbuilder.h"

#include "../src/stringbuilder.cpp"
#include "../src/stringutils.cpp"

#include <assert.h>
#include <stdio.h>
#include <string.h>

// A tiny helper to check exact string equality and print a helpful message.
static void expect_eq(const char* actual, const char* expected, const char* msg) {
  if (strcmp(actual, expected) != 0) {
    printf("❌ %s\n   expected: \"%s\"\n   actual:   \"%s\"\n", msg, expected, actual);
  }
  assert(strcmp(actual, expected) == 0);
}

static void test_default_ctor_and_clear() {
  printf("Running %s...\n", __func__);
  StringBuilder sb;
  expect_eq(sb.c_str(), "", "default ctor should start empty");
  sb + "abc";
  expect_eq(sb.c_str(), "abc", "append after default ctor");
  sb.clear();
  expect_eq(sb.c_str(), "", "clear() should reset to empty");
  sb.clear(); // idempotent
  expect_eq(sb.c_str(), "", "clear() should be idempotent");
}

static void test_string_ctors() {
  printf("Running %s...\n", __func__);
  StringBuilder a((const char*) "hi");
  expect_eq(a.c_str(), "hi", "const char* ctor");

  // Note: header also has char* ctor (non-const). Test via a mutable buffer.
  char buf[] = "mutable";
  StringBuilder b(buf);
  expect_eq(b.c_str(), "mutable", "char* ctor");

  StringBuilder c('A');
  // Per implementation, char -> long -> decimal digits of ASCII code of 'A' (65)
  expect_eq(c.c_str(), "65", "char ctor should append numeric value");

  StringBuilder d((unsigned char) 250);
  expect_eq(d.c_str(), "250", "unsigned char ctor numeric");
}

static void test_numeric_ctors() {
  printf("Running %s...\n", __func__);
  StringBuilder si((int) -42);
  expect_eq(si.c_str(), "-42", "int ctor");

  StringBuilder su((unsigned int) 42);
  expect_eq(su.c_str(), "42", "unsigned int ctor");

  StringBuilder sl((long) -123456);
  expect_eq(sl.c_str(), "-123456", "long ctor");

  StringBuilder sul((unsigned long) 123456ul);
  expect_eq(sul.c_str(), "123456", "unsigned long ctor");

  StringBuilder ss((short) -7);
  expect_eq(ss.c_str(), "-7", "short ctor");

  StringBuilder sus((unsigned short) 7);
  expect_eq(sus.c_str(), "7", "unsigned short ctor");

  StringBuilder sf((float) 3.5f);
  // double formatting is fixed to 1 decimal place in numToA(double)
  expect_eq(sf.c_str(), "3.5", "float ctor -> 1 decimal");

  StringBuilder sd((double) 9.0);
  expect_eq(sd.c_str(), "9.0", "double ctor -> 1 decimal");
}

static void test_assignment_operators() {
  printf("Running %s...\n", __func__);
  StringBuilder sb;
  sb = (const char*) "abc";
  expect_eq(sb.c_str(), "abc", "assign const char*");

  char tmp[] = "XYZ";
  sb = tmp;
  expect_eq(sb.c_str(), "XYZ", "assign char*");

  sb = (int) 99;
  expect_eq(sb.c_str(), "99", "assign int");

  sb = (double) 1.2;
  expect_eq(sb.c_str(), "1.2", "assign double (1 dp)");

  sb = true; // uses "true"
  expect_eq(sb.c_str(), "true", "assign bool true");
  sb = false; // uses "false"
  expect_eq(sb.c_str(), "false", "assign bool false");
}

static void test_append_overloads_and_chaining() {
  printf("Running %s...\n", __func__);
  StringBuilder sb;

  // Start with string
  sb + "temp";
  expect_eq(sb.c_str(), "temp", "append const char*");

  // Append numbers of all widths
  sb + (int) 5 + (unsigned) 6 + (short) -3 + (unsigned short) 2 + (long) -10 + (unsigned long) 88;
  expect_eq(sb.c_str(), "temp56-32-1088", "append numeric chain");

  // Append double -> 1 decimal
  sb + (double) 2.0;
  expect_eq(sb.c_str(), "temp56-32-10882.0", "append double");

  // Append char and unsigned char (should append numeric value)
  sb + (char) 'A' + (unsigned char) 255;
  expect_eq(sb.c_str(), "temp56-32-10882.065255", "append char/uchar numeric");

  // Append C-string and bools
  sb + " END " + true + false;
  // true -> "True", false -> "FALSE"
  expect_eq(sb.c_str(), "temp56-32-10882.065255 END truefalse", "append strings and bools");
}

static void test_c_str_stability() {
  printf("Running %s...\n", __func__);
  StringBuilder sb("hello");
  const char* p = sb.c_str();
  expect_eq(p, "hello", "c_str() returns internal buffer");

  // Append and ensure pointer still points into same internal storage region.
  // We can't check the same address portably, but we can check updated content.
  sb + " world";
  expect_eq(sb.c_str(), "hello world", "c_str() content after append");
}

static void test_capacity_limit_truncation() {
  printf("Running %s...\n", __func__);
  StringBuilder sb;

  // Fill close to the max
  // STRINGBUILDER_MAX is 120 including '\0'.
  // We'll build a 119-char string and confirm it doesn't overflow and remains NUL-terminated.
  const int TARGET_LEN = StringBuilder::STRINGBUILDER_MAX - 1; // 119
  char chunk[33];
  memset(chunk, 'x', sizeof(chunk) - 1);
  chunk[sizeof(chunk) - 1] = '\0';

  // Append in chunks; safeAppend should stop at capacity.
  while ((int) strlen(sb.c_str()) < TARGET_LEN) {
    sb + chunk;
    if ((int) strlen(sb.c_str()) > TARGET_LEN) {
      // Ensure we never exceed capacity - 1 (NUL).
      printf("❌ length exceeded capacity: %zu\n", strlen(sb.c_str()));
      assert(0);
    }
  }

  size_t len = strlen(sb.c_str());
  assert(len <= TARGET_LEN);
  // Try pushing beyond capacity; content should remain <= TARGET_LEN
  sb + "OVERFLOW_SENTINEL";
  size_t newLen = strlen(sb.c_str());
  assert(newLen <= TARGET_LEN);

  // Ensure NUL-termination by comparing via printf and by accessing last char.
  printf("Final length=%zu (cap=%d)\n", newLen, TARGET_LEN);
  assert(sb.c_str()[newLen] == '\0');
}

static void test_assignment_resets_content() {
  printf("Running %s...\n", __func__);
  StringBuilder sb("prefix");
  sb + 123;
  expect_eq(sb.c_str(), "prefix123", "precondition before assignment");

  sb = "reset";
  expect_eq(sb.c_str(), "reset", "assignment should clear and set new value");

  sb = 77;
  expect_eq(sb.c_str(), "77", "assignment to int after previous string");
}

int main() {
  printf("== StringBuilder unit tests ==\n");

  test_default_ctor_and_clear();
  test_string_ctors();
  test_numeric_ctors();
  test_assignment_operators();
  test_append_overloads_and_chaining();
  test_c_str_stability();
  test_capacity_limit_truncation();
  test_assignment_resets_content();

  printf("✅ All tests passed!\n");
  return 0;
}
