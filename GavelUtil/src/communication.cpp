#include "communication.h"

#include <cstdlib>

bool MutexQueue::push(void* element) {
  mutex.take();
  bool returnBool = queue.push(element);
  mutex.give();
  return returnBool;
}

bool MutexQueue::pop(void* element) {
  mutex.take();
  bool returnBool = queue.pop(element);
  mutex.give();
  return returnBool;
}

bool MutexQueue::get(unsigned long index, void* element) {
  mutex.take();
  bool returnBool = queue.get(index, element);
  mutex.give();
  return returnBool;
}

unsigned long MutexQueue::count() {
  mutex.take();
  unsigned long returnCount = queue.count();
  mutex.give();
  return returnCount;
}

unsigned long MutexQueue::highWaterMark() {
  mutex.take();
  unsigned long returnCount = queue.highWaterMark();
  mutex.give();
  return returnCount;
}

void MutexQueue::clear() {
  mutex.take();
  queue.clear();
  mutex.give();
}

bool MutexQueue::error() {
  mutex.take();
  bool returnBool = queue.error();
  mutex.give();
  return returnBool;
}

bool MutexQueue::full() {
  mutex.take();
  bool returnBool = queue.full();
  mutex.give();
  return returnBool;
}

bool MutexQueue::empty() {
  mutex.take();
  bool returnBool = queue.empty();
  mutex.give();
  return returnBool;
}

bool SemQueue::push(void* element) {
  semaphore.take();
  bool returnBool = queue.push(element);
  semaphore.give();
  return returnBool;
}

bool SemQueue::pop(void* element) {
  semaphore.take();
  bool returnBool = queue.pop(element);
  semaphore.give();
  return returnBool;
}

bool SemQueue::get(unsigned long index, void* element) {
  semaphore.take();
  bool returnBool = queue.get(index, element);
  semaphore.give();
  return returnBool;
}

unsigned long SemQueue::count() {
  semaphore.take();
  unsigned long returnCount = queue.count();
  semaphore.give();
  return returnCount;
}

unsigned long SemQueue::highWaterMark() {
  semaphore.take();
  unsigned long returnCount = queue.highWaterMark();
  semaphore.give();
  return returnCount;
}

void SemQueue::clear() {
  semaphore.take();
  queue.clear();
  semaphore.give();
}

bool SemQueue::error() {
  semaphore.take();
  bool returnBool = queue.error();
  semaphore.give();
  return returnBool;
}

bool SemQueue::full() {
  semaphore.take();
  bool returnBool = queue.full();
  semaphore.give();
  return returnBool;
}

bool SemQueue::empty() {
  semaphore.take();
  bool returnBool = queue.empty();
  semaphore.give();
  return returnBool;
}

MessageBuffer::MessageBuffer(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory) {
  capacity = __capacity;
  sizeOfElement = __sizeOfElement;
  queueError = false;
  if (__memory == nullptr)
    memory = (unsigned char*) malloc(capacity * sizeOfElement);
  else
    memory = (unsigned char*) __memory;
  if (memory != nullptr) {
    memError = false;
    xMessageBuffer = xMessageBufferCreateStatic(capacity * sizeOfElement, memory, &xMessageBufferStruct);
  } else {
    sizeOfElement = 0;
    capacity = 0;
    memError = true;
  }
}

bool MessageBuffer::full() {
  return (xMessageBufferIsFull(xMessageBuffer));
}

bool MessageBuffer::empty() {
  return (xMessageBufferIsEmpty(xMessageBuffer));
}

bool MessageBuffer::push(void* element) {
  if (!full()) {
    unsigned long byteSent = xMessageBufferSend(xMessageBuffer, element, sizeOfElement, portMAX_DELAY);
    queueError |= (byteSent == sizeOfElement);
    return queueError;
  } else {
    queueError = true;
    return false;
  }
}

bool MessageBuffer::pop(void* element) {
  if (!empty()) {
    unsigned long byteRx = xMessageBufferReceive(xMessageBuffer, element, sizeOfElement, portMAX_DELAY);
    queueError |= (byteRx = sizeOfElement);
    return queueError;
  } else {
    queueError = true;
    return false;
  }
}

unsigned long MessageBuffer::count() {
  unsigned long totalSize = capacity * sizeOfElement;
  unsigned long freeSpace = xMessageBufferSpacesAvailable(xMessageBuffer);
  unsigned long usedSpace = totalSize - freeSpace;
  return (usedSpace / (sizeOfElement + 4));
}
