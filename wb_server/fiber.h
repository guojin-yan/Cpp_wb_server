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
	/// @brief Э��״̬
	enum State {
		/// @brief ��ʼ��״̬
		INIT,
		/// @brief ��ͣ״̬
		HOLD,
		/// @brief ִ����״̬
		EXEC,
		/// @brief ����״̬
		TERM,
		/// @brief ��ִ��״̬
		READY,
		/// @brief �쳣״̬
		EXCEPT
	};
private:
	Fiber();
public:
	Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
	~Fiber();

	/// @brief ����Э�̺����� ������״̬
	///	@details INIT TERM
	/// @param cb ����
	void reset(std::function<void()> cb);
	/// @brief �л�����ǰЭ��ִ��
	void swap_in();
	/// @brief �л�����ִ̨��
	void swap_out();

	void call();
	void back();

	/// @brief ����Э��id
	/// @return 
	uint64_t get_id() const { return m_id; }
	/// @brief ����Э��״̬
	/// @return 
	State get_state() const { return m_state; }

public:
	/// @brief ���õ�ǰЭ��
	/// @param f ��ǰЭ��
	static void SetThis(Fiber* f);
	/// @brief ���ص�ǰЭ��
	/// @return ��ǰЭ��
	static ptr GetThis();
	/// @brief Э���л�����̨����������ΪReady״̬
	static void YieldToReady();
	/// @brief Э���л�����̨����������ΪHold״̬
	static void YeidToHold();

	/// @brief ��ȡ��Э����
	/// @return ��Э����
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
