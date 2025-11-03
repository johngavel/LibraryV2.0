#include "datastructure.h"

#include <cstdlib>
#include <cstring>

unsigned long ClassicList::highWaterMark() {
  unsigned long i = hwm;
  hwm = 0;
  return i;
};

unsigned long ClassicList::count() {
  return countOfElements;
};

bool ClassicList::full() {
  return (count() == capacity);
};

bool ClassicList::empty() {
  return (count() == 0);
};

bool ClassicList::error() {
  return (memError | listError);
};

ClassicQueue::ClassicQueue(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory) {
  capacity = __capacity;
  sizeOfElement = __sizeOfElement;
  listError = false;
  allocatedMemory = false;
  if (__memory == nullptr) {
    memory = (unsigned char*) malloc(capacity * sizeOfElement);
    allocatedMemory = true;
  } else
    memory = (unsigned char*) __memory;
  if (memory != nullptr) {
    memError = false;
  } else {
    sizeOfElement = 0;
    capacity = 0;
    memError = true;
    allocatedMemory = false;
  }
  clear();
}

ClassicQueue::~ClassicQueue() {
  clear();
  if (allocatedMemory) free(memory);
}

bool ClassicQueue::push(void* element) {
  if (!full()) {
    backIndex++;
    backIndex = backIndex % capacity;
    countOfElements++;
    memcpy(&memory[backIndex * sizeOfElement], element, sizeOfElement);
    hwm = (hwm > countOfElements) ? hwm : countOfElements;
    return true;
  } else {
    listError = true;
    return false;
  }
}

bool ClassicQueue::pop(void* element) {
  if (!empty()) {
    if (element != nullptr) { memcpy(element, &memory[frontIndex * sizeOfElement], sizeOfElement); }
    memset(&memory[frontIndex * sizeOfElement], 0, sizeOfElement);
    frontIndex++;
    frontIndex = frontIndex % capacity;
    countOfElements--;
    return true;
  } else {
    listError = true;
    return false;
  }
}

bool ClassicQueue::get(unsigned long index, void* element) {
  void* address = get(index);
  if (address != nullptr) {
    memcpy(element, address, sizeOfElement);
    return true;
  } else {
    listError = true;
    return false;
  }
}

void* ClassicQueue::get(unsigned long index) {
  unsigned long indexPtr = index;
  if ((!empty()) && (index < count())) {
    indexPtr += frontIndex;
    indexPtr = indexPtr % capacity;
    return &memory[indexPtr * sizeOfElement];
  } else {
    listError = true;
    return nullptr;
  }
}

void ClassicQueue::clear() {
  frontIndex = 0;
  backIndex = capacity - 1;
  countOfElements = 0;
  hwm = 0;
  listError = false;
  if (memory != nullptr) memset(memory, 0, capacity * sizeOfElement);
}

ClassicStack::ClassicStack(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory) {
  capacity = __capacity;
  sizeOfElement = __sizeOfElement;
  listError = false;
  allocatedMemory = false;
  countOfElements = 0;
  if (__memory == nullptr) {
    memory = (unsigned char*) malloc(capacity * sizeOfElement);
    allocatedMemory = true;
  } else
    memory = (unsigned char*) __memory;
  if (memory != nullptr) {
    memError = false;
  } else {
    sizeOfElement = 0;
    capacity = 0;
    memError = true;
    allocatedMemory = false;
  }
  clear();
}

ClassicStack::~ClassicStack() {
  clear();
  if (allocatedMemory) free(memory);
}

bool ClassicStack::push(void* element) {
  if (!full()) {
    memcpy(&memory[countOfElements * sizeOfElement], element, sizeOfElement);
    countOfElements++;
    hwm = (hwm > countOfElements) ? hwm : countOfElements;
    return true;
  } else {
    listError = true;
    return false;
  }
}

bool ClassicStack::pop(void* element) {
  if (!empty()) {
    countOfElements--;
    if (element != nullptr) { memcpy(element, &memory[countOfElements * sizeOfElement], sizeOfElement); }
    memset(&memory[countOfElements * sizeOfElement], 0, sizeOfElement);
    return true;
  } else {
    listError = true;
    return false;
  }
}

bool ClassicStack::get(unsigned long index, void* element) {
  void* address = get(index);
  if (address != nullptr) {
    memcpy(element, address, sizeOfElement);
    return true;
  } else {
    listError = true;
    return false;
  }
}

void* ClassicStack::get(unsigned long index) {
  if ((!empty()) && (index < count())) {
    return &memory[index * sizeOfElement];
  } else {
    listError = true;
    return nullptr;
  }
}

void ClassicStack::clear() {
  countOfElements = 0;
  hwm = 0;
  listError = false;
  if (memory != nullptr) memset(memory, 0, capacity * sizeOfElement);
}
