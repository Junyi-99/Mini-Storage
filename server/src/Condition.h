#pragma once
#include "Mutex.h"
#include <pthread.h>
#include <time.h>

class Condition {
public:
  Condition(MutexLock &mutex) : _mutex(mutex) {
    pthread_cond_init(&_cond, NULL);
  }
  ~Condition() { pthread_cond_destroy(&_cond); }

  void wait() { pthread_cond_wait(&_cond, _mutex.get()); }

  void notify() { pthread_cond_signal(&_cond); }

  void notifyAll() { pthread_cond_broadcast(&_cond); }

private:
  MutexLock &_mutex;
  pthread_cond_t _cond;
};
