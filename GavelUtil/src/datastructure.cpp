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

ClassicSortList::ClassicSortList(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory)
    : ClassicStack(__capacity, __sizeOfElement, __memory) {
  tempSwapSpace = (unsigned char*) malloc(__sizeOfElement);
}

ClassicSortList::~ClassicSortList() {
  if (tempSwapSpace != nullptr) {
    free(tempSwapSpace);
    tempSwapSpace = nullptr;
  }
}

bool ClassicSortList::setSortFunction(int (*cmp)(const void*, const void*)) {
  if (cmp == nullptr) {
    listError = true;
    return false;
  }
  cmpFn = cmp;
  return true;
}

bool ClassicSortList::sort() {
  if (cmpFn == nullptr) {
    listError = true; // comparator not set
    return false;
  }
  return sort(cmpFn);
}

bool ClassicSortList::sort(int (*cmp)(const void*, const void*)) {
  if (memError || sizeOfElement == 0 || cmp == nullptr) {
    listError = true;
    return false;
  }

  if (count() <= 1) return true;

  void* base = (void*) memory;
  if (base == nullptr) {
    listError = true;
    return false;
  }

  qsort(base, count(), sizeOfElement, cmp);
  return true;
}

// Swap two elements via temporary buffer.
bool ClassicSortList::swap(unsigned long i, unsigned long j) {
  if (i >= count() || j >= count() || this->memError || count() == 0) {
    this->listError = true;
    return false;
  }
  if (i == j) return true;
  unsigned char* a = &memory[i * sizeOfElement];
  unsigned char* b = &memory[j * sizeOfElement];

  if (tempSwapSpace == nullptr) {
    listError = true;
    return false;
  }

  memcpy(tempSwapSpace, a, sizeOfElement);
  memcpy(a, b, sizeOfElement);
  memcpy(b, tempSwapSpace, sizeOfElement);

  return true;
}
