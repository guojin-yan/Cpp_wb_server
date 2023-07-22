#ifndef _YGJ_SCHEDULER_H_
#define _YGJ_SCHEDULER_H_

#include <functional>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "fiber.h"
#include "mutex.h"
#include "thread.h"

namespace ygj_server {

class Scheduler {
public:
	typedef std::shared_ptr<Scheduler> ptr;
	typedef Mutex MutexType;

	Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
	virtual ~Scheduler();

	const std::string& get_name() const { return m_name; }

	static Scheduler* GetThis();
	static Fiber* GetMainFiber();

	void start();
	void stop();

	template<class FiberOrCb>
	void scheduler(FiberOrCb fc, int thread = -1) {
		bool need_tickle = false;
		{
			MutexType::Lock lock(m_mutex);
			need_tickle = scheduler_nolock(fc, thread);
		}
		if(need_tickle) {
			tickel();
		}

	}
	template<class InputIterator>
	void scheduler(InputIterator begin, InputIterator end) {
		bool need_tickle = false;
		{
			MutexType::Lock lock(m_mutex);
			while (begin != end) {
				need_tickle = scheduler_nolock(&*begin) || need_tickle;
				++begin;
			}
		}
		if (need_tickle) {
			tickel();
		}

	}
protected:
	virtual void tickel();
	virtual bool stopping();
	virtual void idle();
	void set_this();
	void run();
private:
	template<class FiberOrCb>
	bool scheduler_nolock(FiberOrCb fc, int thread) {
		bool need_tickle =m_fibers.empty();
		FiberAndThread ft(fc, thread);
		if(ft.fiber || ft.cb) {
			m_fibers.push_back(ft);
		}
		return need_tickle;
	}
private:
	struct FiberAndThread {
		Fiber::ptr fiber;
		std::function<void()> cb;
		int thread;
		FiberAndThread(Fiber::ptr f, int thr) : fiber(f), thread(thr) {
		}
		FiberAndThread(Fiber::ptr* f, int thr) :thread(thr) {
			fiber.swap(*f);
		}
		FiberAndThread(std::function<void()> f, int thr) : cb(f), thread(thr) {
		}
		FiberAndThread(std::function<void()>* f, int thr) : thread(thr) {
			cb.swap(*f);
		}
		FiberAndThread() : thread(-1) {
		}
		void reset() {
			fiber = nullptr;
			cb = nullptr;
			thread = -1;
		}
	};
private:
	MutexType m_mutex;
	std::vector<Thread::ptr> m_threads;
	std::list<FiberAndThread> m_fibers;
	Fiber::ptr m_root_fiber;
	std::string m_name;

protected:
	/// @brief 协程下的线程id数组
	std::vector<int> m_thread_id;
	/// @brief 线程数量
	size_t m_thread_count = 0;
	/// @brief 工作线程数量
	std::atomic<size_t>  m_active_thread_count = { 0 };
	/// @brief 空闲线程数量
	std::atomic<size_t>  m_idle_thread_count = { 0 };
	/// @brief 是否正在停止
	bool m_stoping = true;
	/// @brief 是否自动停止
	bool m_auto_stop = false;
	/// @brief 主线程id(use_caller)
	int m_root_thread = 0;

};
}

#endif
