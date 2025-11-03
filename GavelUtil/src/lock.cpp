#include "lock.h"

void Mutex::take() {
  xSemaphoreTake(mutex, portMAX_DELAY);
}

void Mutex::give() {
  xSemaphoreGive(mutex);
}

SemLock::SemLock() {
  sem_init(&semLock, 1, 1);
}

void SemLock::take() {
  sem_acquire_blocking(&semLock);
}

void SemLock::give() {
  sem_release(&semLock);
}
