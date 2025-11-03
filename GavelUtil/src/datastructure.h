#ifndef __GAVEL_DATA_STRUCTURE
#define __GAVEL_DATA_STRUCTURE

class ClassicList {
public:
  virtual bool push(void* element) = 0;
  virtual bool pop(void* element = nullptr) = 0;
  virtual bool get(unsigned long index, void* element) = 0;
  virtual void* get(unsigned long index) = 0;
  virtual void clear() = 0;

  unsigned long highWaterMark();
  unsigned long count();
  bool full();
  bool empty();
  bool error();

protected:
  unsigned char* memory;
  bool allocatedMemory;
  unsigned long sizeOfElement;
  unsigned long capacity;
  unsigned long countOfElements;
  unsigned long hwm;
  bool memError;
  bool listError;
};

class ClassicQueue : public ClassicList {
public:
  ClassicQueue(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory = nullptr);
  ~ClassicQueue();
  ClassicQueue(const ClassicQueue&) = delete;
  ClassicQueue& operator=(const ClassicQueue&) = delete;
  bool push(void* element);
  bool pop(void* element = nullptr);
  bool get(unsigned long index, void* element);
  void* get(unsigned long index);
  void clear();

private:
  unsigned long frontIndex;
  unsigned long backIndex;
};

class ClassicStack : public ClassicList {
public:
  ClassicStack(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory = nullptr);
  ~ClassicStack();
  ClassicStack(const ClassicStack&) = delete;
  ClassicStack& operator=(const ClassicStack&) = delete;
  bool push(void* element);
  bool pop(void* element = nullptr);
  bool get(unsigned long index, void* element);
  void* get(unsigned long index);
  void clear();

private:
};

#endif