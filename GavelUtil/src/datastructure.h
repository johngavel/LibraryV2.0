#ifndef __GAVEL_DATA_STRUCTURE_H
#define __GAVEL_DATA_STRUCTURE_H

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
  virtual bool push(void* element) override;
  virtual bool pop(void* element = nullptr) override;
  virtual bool get(unsigned long index, void* element) override;
  virtual void* get(unsigned long index) override;
  virtual void clear() override;

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
  virtual bool push(void* element) override;
  virtual bool pop(void* element = nullptr) override;
  virtual bool get(unsigned long index, void* element) override;
  virtual void* get(unsigned long index) override;
  virtual void clear() override;

private:
};

class ClassicSortList : public ClassicStack {
public:
  ClassicSortList(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory = nullptr);
  ~ClassicSortList();
  ClassicSortList(const ClassicSortList&) = delete;
  ClassicSortList& operator=(const ClassicSortList&) = delete;
  bool setSortFunction(int (*cmp)(const void*, const void*));
  bool sort();
  bool sort(int (*cmp)(const void*, const void*)); // one-off comparator
  bool swap(unsigned long i, unsigned long j);

private:
  unsigned char* tempSwapSpace;
  int (*cmpFn)(const void*, const void*) = nullptr;
};

#endif // __GAVEL_DATA_STRUCTURE_H