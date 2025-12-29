#ifndef __GAVEL_STRING_H
#define __GAVEL_STRING_H

bool isValidCString(const char* str, unsigned int maxLen = 256);

int safeCompare(const char* string1, const char* string2);

int safeCompare(const char* string1, const char* string2, int length);

char* safeAppend(char* dest, const char* src, int destSize);

char* tab(int tabLength, char* buffer, int size);

char* hexByteString(unsigned char value, char* buffer, int size);

char* decByteString(unsigned char value, char* buffer, int size);

char* getMacString(unsigned char* mac, char* buffer, int size);

char* getIPString(unsigned char* ip, char* buffer, int size);
bool parseIPAddress(const char* ipString, unsigned char* buffer);

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

#endif // __GAVEL_STRING_H