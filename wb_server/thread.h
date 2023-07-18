/// @file log.h
/// @brief 线程模块封装
/// @author yanguojin
/// @email 1023438782@qq.com
/// @date 2023-07-16
#ifndef __YGJ_THREAD_H__
#define __YGJ_THREAD_H__

#include <atomic>
#include <functional>
#include <memory>
#include <semaphore.h>
#include <string>
#include <thread>

#include <pthread.h>

#include "noncopyable.h"


namespace ygj_server {

class Semaphore : NonCopyable {
public:
	Semaphore(uint32_t count = 0);
	~Semaphore();

	void wait();
	void notify();

private:
	sem_t m_semaphore;
};


template<class T>
struct ScopedLockImli {
public:
	ScopedLockImli(T& mutex) :m_mutex(mutex) {
		m_mutex.lock();
		m_locked = true;
	}
	~ScopedLockImli() {
		unlock();
	}
	void lock() {
		if (!m_locked) {
			m_mutex.lock();
			m_locked = true;
		}
	}
	void unlock() {
		if(m_locked) {
			m_mutex.unlock();
			m_locked = false;
		}
	}
private:
	T& m_mutex;
	bool m_locked;
};

template<class T>
struct ReadScopedLockImli {
public:
	ReadScopedLockImli(T& mutex) :m_mutex(mutex) {
		m_mutex.read_lock();
		m_locked = true;
	}
	~ReadScopedLockImli() {
		unlock();
	}
	void lock() {
		if (!m_locked) {
			m_mutex.read_lock();
			m_locked = true;
		}
	}
	void unlock() {
		if (m_locked) {
			m_mutex.unlock();
			m_locked = false;
		}
	}
private:
	T& m_mutex;
	bool m_locked;
};


template<class T>
struct WriteScopedLockImli {
public:
	WriteScopedLockImli(T& mutex) :m_mutex(mutex) {
		m_mutex.write_lock();
		m_locked = true;
	}
	~WriteScopedLockImli() {
		unlock();
	}
	void lock() {
		if (!m_locked) {
			m_mutex.write_lock();
			m_locked = true;
		}
	}
	void unlock() {
		if (m_locked) {
			m_mutex.unlock();
			m_locked = false;
		}
	}
private:
	T& m_mutex;
	bool m_locked;
};

class Mutex {
public:
	typedef ScopedLockImli<Mutex> Lock;

	Mutex() {
		pthread_mutex_init(&m_mutex, nullptr);
	}
	~Mutex() {
		pthread_mutex_destroy(&m_mutex);
	}
	void lock() {
		pthread_mutex_lock(&m_mutex);
	}
	void unlock() {
		pthread_mutex_unlock(&m_mutex);
	}
private:
	pthread_mutex_t m_mutex;
};

class NULLMutex {
public:
	typedef ScopedLockImli<Mutex> Lock;
	NULLMutex(){}
	~NULLMutex(){}
	void lock(){}
	void unlock(){}
};

class RWMutex {
public:
	typedef ReadScopedLockImli<RWMutex> ReadLock;
	typedef WriteScopedLockImli<RWMutex> WriteLock;
	RWMutex() {
		pthread_rwlock_init(&m_lock, nullptr);
	}
	~RWMutex() {
		pthread_rwlock_destroy(&m_lock);
	}

	void read_lock() {
		pthread_rwlock_rdlock(&m_lock);
	}
	void write_lock() {
		pthread_rwlock_wrlock(&m_lock);
	}
	void unlock() {
		pthread_rwlock_unlock(&m_lock);
	}

private:
	pthread_rwlock_t m_lock;
};

class NULLRWMutex {
public:
	typedef ReadScopedLockImli<NULLRWMutex> ReadLock;
	typedef WriteScopedLockImli<NULLRWMutex> WriteLock;
	NULLRWMutex() {}
	~NULLRWMutex() {}

	void read_lock() {}
	void write_lock() {}
	void unlock() {}
};


class Spinlock {
public:
	typedef ScopedLockImli<Spinlock> Lock;
	Spinlock() {
		pthread_spin_init(&m_mutex, 0);
	}
	~Spinlock() {
		pthread_spin_destroy(&m_mutex);
	}
	void lock() {
		pthread_spin_lock(&m_mutex);
	}
	void unlock() {
		pthread_spin_unlock(&m_mutex);
	}
private:
	pthread_spinlock_t m_mutex;
};

class CASLock : NonCopyable {
public:
	typedef ScopedLockImli<CASLock> Lock;
	CASLock() {
		m_mutex.clear();
	}

	~CASLock() {
	}
	void lock() {
		while (std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
	}
	void unlock() {
		std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
	}
private:
	volatile std::atomic_flag m_mutex;
};


class Thread :NonCopyable {
public:
	typedef std::shared_ptr<Thread> ptr;
	Thread(std::function<void()> cb, const std::string& name);
	~Thread();
	uint32_t get_id() const{ return m_id; }
	const std::string& get_name() const { return m_name; }

	void join();
	static Thread* GetThis();
	static const std::string& GetName();
	static void SetName(const std::string& name);

	static void* run(void* arg);
private:
	pid_t m_id = -1;
	pthread_t m_thread;
	std::function<void()> m_cb;
	std::string m_name;
	Semaphore m_semaphore;

};

}



#endif
