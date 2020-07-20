#pragma once
#include <pthread.h>


class MutexLock 
{
	public:
		MutexLock() {   pthread_mutex_init(&_mutex,NULL);   }
		~MutexLock() {  pthread_mutex_destroy(&_mutex); }

		void lock() {   pthread_mutex_lock(&_mutex);    }

		void unlock() { pthread_mutex_unlock(&_mutex);  }

		pthread_mutex_t* get() {    return &_mutex; }

		friend class Condition;
	private:
		pthread_mutex_t _mutex;
};


class MutexLockGuard
{
	public:
		MutexLockGuard(MutexLock & mutex) : _mutex(mutex) {   _mutex.lock();  }
		~MutexLockGuard() {  _mutex.unlock();  }

	private:
		MutexLock & _mutex;
};




