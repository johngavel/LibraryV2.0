#ifndef __GAVEL_LOCK
#define __GAVEL_LOCK

#include <FreeRTOS.h>
#include <pico/sem.h>
#include <semphr.h>

class Lock {
public:
  virtual void take() = 0;
  virtual void give() = 0;
};

class Mutex : public Lock {
public:
  Mutex() : mutex(xSemaphoreCreateMutex()){};
  void take();
  void give();

private:
  SemaphoreHandle_t mutex;
};

class SemLock : public Lock {
public:
  SemLock();
  void take();
  void give();

private:
  semaphore_t semLock;
};

#endif