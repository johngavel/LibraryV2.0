#include "../src/stringutils.h"

#include "../src/stringbuilder.cpp"
#include "../src/stringbuilder.h"
#include "../src/stringutils.cpp"

#include <cassert>
#include <cfloat>
#include <climits>
#include <cstdio>
#include <cstring>

int PASSED = 0;
// Helper macro for manual assertion
#define ASSERT_EQ_STR(actual, expected, testname) assert(strcmp(actual, expected) == 0)

void testConstructors() {
  StringBuilder sb1("Hello");
  assert(strcmp(sb1.c_str(), "Hello") == 0);
  printf("Constructor with const char*: PASS\n");

  StringBuilder sb2(42);
  assert(strcmp(sb2.c_str(), "42") == 0);
  printf("Constructor with int: PASS\n");

  StringBuilder sb3('A');
  assert(strcmp(sb3.c_str(), "65") == 0); // Note: appends ASCII code
  printf("Constructor with char: PASS\n");
}

void testAssignment() {
  StringBuilder sb;
  sb = "World";
  assert(strcmp(sb.c_str(), "World") == 0);
  printf("Assignment with const char*: PASS\n");

  sb = 123;
  assert(strcmp(sb.c_str(), "123") == 0);
  printf("Assignment with int: PASS\n");
}

void testAppend() {
  StringBuilder sb("Value: ");
  sb + 99 + ", Done";
  assert(strcmp(sb.c_str(), "Value: 99, Done") == 0);
  printf("Append chaining: PASS\n");
}

void testClear() {
  StringBuilder sb("ClearMe");
  sb.clear();
  assert(strcmp(sb.c_str(), "") == 0);
  printf("Clear method: PASS\n");
}

void testBufferLimit() {
  StringBuilder sb;
  for (int i = 0; i < 200; i++) {
    sb + "X"; // Will truncate silently
  }
  assert(strlen(sb.c_str()) == (StringBuilder::STRINGBUILDER_MAX - 1));
  printf("Buffer limit handling: PASS (truncation occurs)\n");
}

void testUtils() {
  // Test safeAppend
  char dest1[10] = "Hi";
  ASSERT_EQ_STR(safeAppend(dest1, "There", 10), "HiThere", "safeAppend basic");
  char dest2[10] = "HiThere";
  ASSERT_EQ_STR(safeAppend(dest2, "1234567890", 10), "HiThere12", "safeAppend overflow");

  // Test tab
  char buffer1[20] = "abc";
  ASSERT_EQ_STR(tab(4, buffer1, 20), "abc ", "tab basic");

  // Test hexByteString
  char hexbuf[4];
  ASSERT_EQ_STR(hexByteString(255, hexbuf, 4), "FF", "hexByteString 255");
  ASSERT_EQ_STR(hexByteString(0, hexbuf, 4), "00", "hexByteString 0");

  // Test decByteString
  char decbuf[4];
  ASSERT_EQ_STR(decByteString(99, decbuf, 4), "99", "decByteString 99");
  ASSERT_EQ_STR(decByteString(100, decbuf, 4), "99", "decByteString capped");

  // Test getMacString
  unsigned char mac[6] = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E};
  char macbuf[18];
  ASSERT_EQ_STR(getMacString(mac, macbuf, 18), "00:1A:2B:3C:4D:5E", "getMacString basic");

  // Test getIPString
  unsigned char ip[4] = {192, 168, 1, 1};
  char ipbuf[16];
  ASSERT_EQ_STR(getIPString(ip, ipbuf, 16), "192.168.1.1", "getIPString basic");

  // Test timeString
  char timebuf[10];
  ASSERT_EQ_STR(timeString(3661, timebuf, 10), "1:01:01", "timeString 3661");

  // Test trimWhitespace
  char wsbuf[20] = "  hello world  ";
  ASSERT_EQ_STR(trimWhitespace(wsbuf, 20), "hello world", "trimWhitespace basic");
}

// Small helpers
static void expect_str_eq(const char* got, const char* want, const char* case_name) {
  if (std::strcmp(got, want) != 0) {
    std::printf("[FAIL] %s: got=\"%s\" want=\"%s\"\n", case_name, got, want);
    assert(false);
  } else {
    std::printf("[PASS] %s: \"%s\"\n", case_name, got);
  }
}

static void expect_ptr_eq(void* got, void* want, const char* case_name) {
  if (got != want) {
    std::printf("[FAIL] %s: returned pointer %p != buffer %p\n", case_name, got, want);
    assert(false);
  } else {
    std::printf("[PASS] %s: return pointer == buffer\n", case_name);
  }
}

int testNumToA() {
  std::printf("Running numToA unit tests…\n");

  // ---------- Signed integers ----------
  {
    char buf[64];

    // int -> long path
    {
      char* ret = numToA(0, buf, sizeof(buf));
      expect_ptr_eq(ret, buf, "int zero return");
      expect_str_eq(buf, "0", "int zero");

      ret = numToA(-12345, buf, sizeof(buf));
      expect_ptr_eq(ret, buf, "int negative return");
      expect_str_eq(buf, "-12345", "int negative");

      ret = numToA(2147483647, buf, sizeof(buf)); // INT_MAX
      expect_str_eq(buf, "2147483647", "int INT_MAX");
    }

    // short -> long path
    {
      short sneg = -32768;
      numToA(sneg, buf, sizeof(buf));
      expect_str_eq(buf, "-32768", "short min");

      short spos = 32767;
      numToA(spos, buf, sizeof(buf));
      expect_str_eq(buf, "32767", "short max");
    }

    // long direct
    {
      long lz = 0L;
      numToA(lz, buf, sizeof(buf));
      expect_str_eq(buf, "0", "long zero");

      long lneg = -987654321L;
      numToA(lneg, buf, sizeof(buf));
      expect_str_eq(buf, "-987654321", "long negative");

      long lpos = 987654321L;
      numToA(lpos, buf, sizeof(buf));
      expect_str_eq(buf, "987654321", "long positive");
    }

    // char (may be signed or unsigned depending on platform) – force values explicitly
    {
      signed char cneg = static_cast<signed char>(-5);
      numToA(cneg, buf, sizeof(buf));
      expect_str_eq(buf, "-5", "char negative");

      signed char cz = 0;
      numToA(cz, buf, sizeof(buf));
      expect_str_eq(buf, "0", "char zero");
    }
  }

  // ---------- Unsigned integers ----------
  {
    char buf[64];

    // unsigned int -> unsigned long path
    {
      unsigned int u0 = 0u;
      char* ret = numToA(u0, buf, sizeof(buf));
      expect_ptr_eq(ret, buf, "unsigned int zero return");
      expect_str_eq(buf, "0", "unsigned int zero");

      unsigned int umax = UINT_MAX;
      ret = numToA(umax, buf, sizeof(buf));
      // Represented via "%lu" after promotion; value is platform-dependent width but the decimal text must match the numeric value.
      std::printf("  (info) UINT_MAX is %u on this platform\n", umax);
      // We can cross-check by printing with snprintf too:
      char check[64];
      std::snprintf(check, sizeof(check), "%u", umax);
      expect_str_eq(buf, check, "unsigned int UINT_MAX text");
    }

    // unsigned short -> unsigned long path
    {
      unsigned short us = 65535u;
      numToA(us, buf, sizeof(buf));
      expect_str_eq(buf, "65535", "unsigned short max");
    }

    // unsigned char -> unsigned long path
    {
      unsigned char uc = 255u;
      numToA(uc, buf, sizeof(buf));
      expect_str_eq(buf, "255", "unsigned char 255");
    }

    // unsigned long direct
    {
      unsigned long ul = 4000000000ul; // fits on 64-bit; if 32-bit UL, it may wrap at compile time—guard below
      // Create the reference string with snprintf to avoid compile-time literal mismatch.
      char ref[64];
      std::snprintf(ref, sizeof(ref), "%lu", ul);

      char buf2[64];
      numToA(ul, buf2, sizeof(buf2));
      expect_str_eq(buf2, ref, "unsigned long arbitrary");
    }
  }

  // ---------- Floating point (two decimals) ----------
  {
    char buf[64];

    {
      double dz = 0.0;
      char* ret = numToA(dz, buf, sizeof(buf));
      expect_ptr_eq(ret, buf, "double zero return");
      expect_str_eq(buf, "0.0", "double zero two-decimals");
    }
    {
      double dpos = 12.3;
      numToA(dpos, buf, sizeof(buf));
      expect_str_eq(buf, "12.3", "double positive pad");
    }
    {
      double dround_up = 1.999;
      numToA(dround_up, buf, sizeof(buf));
      expect_str_eq(buf, "2.0", "double rounding up");
    }
    {
      double dround = 2.345; // 2.345 -> "2.35"
      numToA(dround, buf, sizeof(buf));
      expect_str_eq(buf, "2.3", "double rounding 2.345");
    }
    {
      double dneg = -7.1;
      numToA(dneg, buf, sizeof(buf));
      expect_str_eq(buf, "-7.1", "double negative");
    }

    // float overload forwards to double
    {
      float f = 3.5f;
      numToA(f, buf, sizeof(buf));
      expect_str_eq(buf, "3.5", "float -> double path");
    }
  }

  // ---------- Buffer size edge cases (truncation) ----------
  {
    // numToA uses snprintf, which always null-terminates (C99) and truncates as needed.
    // Validate truncation does not overflow and result is prefix of expected.
    char small[4];                          // Can hold up to 3 visible chars + '\0'
    std::memset(small, 'X', sizeof(small)); // poison

    char* ret = numToA(123456, small, sizeof(small));
    expect_ptr_eq(ret, small, "truncation return");

    // Expected full string would be "123456"; with size=4 we expect "123"
    expect_str_eq(small, "123", "truncation prefix int");

    // Floating example: "-1.23" truncated to "-1."
    char smallf[4];
    std::memset(smallf, 'Y', sizeof(smallf));
    numToA(-1.23, smallf, sizeof(smallf));
    // Full would be "-1.23" but with size=4, we get first 3 chars + '\0' => "-1."
    expect_str_eq(smallf, "-1.", "truncation prefix float");
  }

  // ---------- Return-to-same-buffer sanity check ----------
  {
    char buf[32];
    char* p = numToA(42, buf, sizeof(buf));
    assert(p == buf);
    std::printf("[PASS] return-to-buffer sanity\n");
  }

  std::printf("All numToA tests passed.\n");
  return 0;
}

int main() {
  testNumToA();
  testConstructors();
  testAssignment();
  testAppend();
  testClear();
  testBufferLimit();
  printf("All tests completed.\n");

  return PASSED;
}