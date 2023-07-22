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

#include "mutex.h"
#include "noncopyable.h"


namespace ygj_server {

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
