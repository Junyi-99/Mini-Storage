#pragma once
#include <deque>
#include <assert.h>
#include "Mutex.h"
#include "Condition.h"
template <typename T>
class BlockingQueue
{
	public:
		BlockingQueue()
			:  _mutex()
			  , _cond(_mutex)
	{}

		int put(const T& task)
		{
			MutexLock lock(_mutex);
			_queue.push_back(task);
			_cond.notify();
			return 0;
		}

		T take()
		{
			MutexLock lock(_mutex);
			while(_queue.empty())
			{
				_cond.wait();
			}
			assert(!_queue.empty());
			T front = _queue.front();
			_queue.pop_front();
			return front;
		}

		size_t size() const
		{
			MutexLockGuard lock(_mutex);
			return _queue.size();
		}

		void notifyAll() {  _cond.notifyAll();  }

	private:
		MutexLock _mutex;
		Condition _cond;
		std::deque<T> _queue;
};



