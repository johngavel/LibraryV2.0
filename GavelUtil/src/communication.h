#ifndef __GAVEL_COMMUNICATION
#define __GAVEL_COMMUNICATION

#include "datastructure.h"
#include "lock.h"

class MutexQueue : public ClassicList {
public:
  MutexQueue(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory = nullptr) : queue(__capacity, __sizeOfElement, __memory){};
  bool full();
  bool empty();
  bool push(void* element);
  bool pop(void* element = nullptr);
  bool get(unsigned long index, void* element);
  unsigned long count();
  unsigned long highWaterMark();
  void clear();
  bool error();

private:
  Mutex mutex;
  ClassicQueue queue;
};

class SemQueue : public ClassicList {
public:
  SemQueue(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory = nullptr) : queue(__capacity, __sizeOfElement, __memory){};
  bool full();
  bool empty();
  bool push(void* element);
  bool pop(void* element = nullptr);
  bool get(unsigned long index, void* element);
  unsigned long count();
  unsigned long highWaterMark();
  void clear();
  bool error();

private:
  SemLock semaphore;
  ClassicQueue queue;
};

#include <message_buffer.h>
class MessageBuffer : public ClassicList {
public:
  MessageBuffer(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory = nullptr);
  bool full();
  bool empty();
  bool push(void* element);
  bool pop(void* element = nullptr);
  unsigned long count();
  unsigned long highWaterMark() { return 0; };
  bool error() { return (memError & queueError); };
  void clear() {};

private:
  StaticMessageBuffer_t xMessageBufferStruct;
  MessageBufferHandle_t xMessageBuffer;
  unsigned char* memory;
  unsigned long capacity;
  unsigned long sizeOfElement;
  bool memError;
  bool queueError;
};

#endif
