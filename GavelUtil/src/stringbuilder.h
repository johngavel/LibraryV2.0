#ifndef GAVEL_STRINGBUILDER_H
#define GAVEL_STRINGBUILDER_H

class StringBuilder {
public:
  static constexpr int STRINGBUILDER_MAX = 120;
  static constexpr int kCapacity = STRINGBUILDER_MAX - 1;

  StringBuilder() { clear(); };
  StringBuilder(const StringBuilder& sb) { *this = sb.c_str(); };
  StringBuilder(bool b) { *this = b; };
  StringBuilder(const char* __string) { *this = __string; };
  StringBuilder(char c) { *this = c; };
  StringBuilder(unsigned char c) { *this = c; };
  StringBuilder(long l) { *this = l; };
  StringBuilder(unsigned long l) { *this = l; };
  StringBuilder(int i) { *this = i; };
  StringBuilder(unsigned int i) { *this = i; };
  StringBuilder(short s) { *this = s; };
  StringBuilder(unsigned short s) { *this = s; };
  StringBuilder(float f) { *this = f; };
  StringBuilder(double d) { *this = d; };

  StringBuilder& operator=(StringBuilder& sb);
  StringBuilder& operator=(bool b);
  StringBuilder& operator=(const char* __string);
  StringBuilder& operator=(char c);
  StringBuilder& operator=(unsigned char c);
  StringBuilder& operator=(long l);
  StringBuilder& operator=(unsigned long l);
  StringBuilder& operator=(int i);
  StringBuilder& operator=(unsigned int i);
  StringBuilder& operator=(short s);
  StringBuilder& operator=(unsigned short s);
  StringBuilder& operator=(float f);
  StringBuilder& operator=(double d);

  StringBuilder& operator+=(StringBuilder& sb);
  StringBuilder& operator+=(bool b);
  StringBuilder& operator+=(const char* __string);
  StringBuilder& operator+=(char c);
  StringBuilder& operator+=(unsigned char c);
  StringBuilder& operator+=(long l);
  StringBuilder& operator+=(unsigned long l);
  StringBuilder& operator+=(int i);
  StringBuilder& operator+=(unsigned int i);
  StringBuilder& operator+=(short s);
  StringBuilder& operator+=(unsigned short s);
  StringBuilder& operator+=(float f);
  StringBuilder& operator+=(double d);

  StringBuilder& operator+(StringBuilder& sb);
  StringBuilder& operator+(bool b);
  StringBuilder& operator+(const char* __string);
  StringBuilder& operator+(char c);
  StringBuilder& operator+(unsigned char c);
  StringBuilder& operator+(long l);
  StringBuilder& operator+(unsigned long l);
  StringBuilder& operator+(int i);
  StringBuilder& operator+(unsigned int i);
  StringBuilder& operator+(short s);
  StringBuilder& operator+(unsigned short s);
  StringBuilder& operator+(float f);
  StringBuilder& operator+(double d);
  const char* c_str() const noexcept;
  void clear() noexcept;
  int size() const noexcept;      // current logical length
  int capacity() const noexcept;  // STRINGBUILDER_MAX - 1
  int remaining() const noexcept; // capacity() - size()

private:
  StringBuilder& append_raw(const char* s) noexcept;
  char buffer_[STRINGBUILDER_MAX];
  int size_ = 0;
};

#endif // GAVEL_STRINGBUILDER_H