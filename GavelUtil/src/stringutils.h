#ifndef GAVEL_STRING_H
#define GAVEL_STRING_H

bool isValidCString(const char* str, unsigned int maxLen = 256);

int safeCompare(char* string1, char* string2);

char* safeAppend(char* dest, const char* src, int destSize);

char* tab(int tabLength, char* buffer, int size);

char* hexByteString(unsigned char value, char* buffer, int size);

char* decByteString(unsigned char value, char* buffer, int size);

char* getMacString(unsigned char* mac, char* buffer, int size);

char* getIPString(unsigned char* ip, char* buffer, int size);

char* timeString(unsigned long seconds, char* buffer, int size);

char* trimWhitespace(char* buffer, int size);

char* numToA(int n, char* buffer, int size);
char* numToA(unsigned int n, char* buffer, int size);
char* numToA(char n, char* buffer, int size);
char* numToA(unsigned char n, char* buffer, int size);
char* numToA(short n, char* buffer, int size);
char* numToA(unsigned short n, char* buffer, int size);
char* numToA(long n, char* buffer, int size);
char* numToA(unsigned long n, char* buffer, int size);
char* numToA(float n, char* buffer, int size);
char* numToA(double n, char* buffer, int size);

#define STRINGBUILDER_MAX 120
class StringBuilder {
public:
  StringBuilder() { clear(); };
  StringBuilder(char* __string) { *this = __string; };
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

  StringBuilder& operator=(char* __string);
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

  StringBuilder& operator+(char* __string);
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
  char* c_str();
  void clear();

private:
  char string[STRINGBUILDER_MAX];
};

#endif // GAVEL_STRING_H