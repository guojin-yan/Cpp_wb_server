#ifndef _YGJ_FIBER_H_
#define _YGJ_FIBER_H_

#include <functional>
#include <memory>

#include "mutex.h"

#ifdef _WIN32
#else
#include <ucontext.h>
#endif


namespace ygj_server {
class Fiber : public std::enable_shared_from_this<Fiber>{friend class Scheduler;
public:
	typedef std::shared_ptr<Fiber> ptr;
	/// @brief 协程状态
	enum State {
		/// @brief 初始化状态
		INIT,
		/// @brief 暂停状态
		HOLD,
		/// @brief 执行中状态
		EXEC,
		/// @brief 结束状态
		TERM,
		/// @brief 可执行状态
		READY,
		/// @brief 异常状态
		EXCEPT
	};
private:
	Fiber();
public:
	Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
	~Fiber();

	/// @brief 重置协程函数， 并重置状态
	///	@details INIT TERM
	/// @param cb 函数
	void reset(std::function<void()> cb);
	/// @brief 切换到当前协程执行
	void swap_in();
	/// @brief 切换到后台执行
	void swap_out();

	void call();
	void back();

	/// @brief 返回协程id
	/// @return 
	uint64_t get_id() const { return m_id; }
	/// @brief 返回协程状态
	/// @return 
	State get_state() const { return m_state; }

public:
	/// @brief 设置当前协程
	/// @param f 当前协程
	static void SetThis(Fiber* f);
	/// @brief 返回当前协程
	/// @return 当前协程
	static ptr GetThis();
	/// @brief 协程切换到后台，并且设置为Ready状态
	static void YieldToReady();
	/// @brief 协程切换到后台，并且设置为Hold状态
	static void YeidToHold();

	/// @brief 获取总协程数
	/// @return 总协程数
	static uint64_t TotalFibers();

	static void MainFunc();
	static void CallMainFunc();
	static uint64_t GetFiberId();

private:
	uint64_t m_id = 0;
	uint32_t m_stacksize = 0;
	State m_state = INIT;
	void* m_stack = nullptr;
	std::function<void()> m_cb;
#ifdef _WIN32
#else
	ucontext_t m_ctx;
#endif

};
}

#endif // !_YGJ_FIBER_H_
