#include "fiber.h"
#include <atomic>

#include "config.h"
#include "log.h"
#include "macro.h"
#include "scheduler.h"

namespace ygj_server {
	static Logger::ptr g_logger = YGJ_LOG_NAME("system");
static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};

static thread_local Fiber* t_fiber = nullptr;
static thread_local Fiber::ptr t_thread_fiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_starck_size =
Config::lookup<uint32_t>("fiber.stack_size", 1024 * 1024, "fiber stack size");

class MallStackAllocator {
public:
	static void* Alloc(size_t size) {
		return malloc(size);
	}
	static void Dealloc(void* vp, size_t size) {
		return free(vp);
	}
};

using StackAllocator = MallStackAllocator;

Fiber::Fiber() {
	m_state = EXEC;
	SetThis(this);
#ifdef _WIN32
#else
	if (getcontext(&m_ctx)) {
		YGJ_ASSERT2(false, "getcontext");
	}
#endif
	++s_fiber_count;
	YGJ_LOG_DEBUG(g_logger) << "Fiber::Fiber, id=" << m_id;
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller)
: m_id(++s_fiber_id), m_cb(cb) {
	++s_fiber_count;
	m_stacksize = stacksize ? stacksize : g_fiber_starck_size->get_value();
	m_stack = StackAllocator::Alloc(m_stacksize);
#ifdef _WIN32
#else
	if (getcontext(&m_ctx)) {
		YGJ_ASSERT2(false, "getcontext");
	}
	m_ctx.uc_link = nullptr;
	m_ctx.uc_stack.ss_sp = m_stack;
	m_ctx.uc_stack.ss_size = m_stacksize;
	if(!use_caller) {
		makecontext(&m_ctx, &Fiber::MainFunc, 0);
	} else {
		makecontext(&m_ctx, &Fiber::CallMainFunc, 0);
	}
#endif
	YGJ_LOG_DEBUG(g_logger) << "Fiber::Fiber, id=" << m_id;
}
Fiber::~Fiber() {
	--s_fiber_count;
	if(m_stack) {
		YGJ_ASSERT(m_state == TERM || m_state == EXCEPT || m_state == INIT);
		StackAllocator::Dealloc(m_stack, m_stacksize);
	} else {
		YGJ_ASSERT(!m_cb);
		YGJ_ASSERT(m_state == EXEC);

		Fiber* cur = t_fiber;
		if(cur == this) {
			SetThis(nullptr);
		}
	}
	YGJ_LOG_DEBUG(g_logger) << "Fiber::~Fiber, id=" << m_id;
}

void Fiber::reset(std::function<void()> cb) {
	YGJ_ASSERT(m_stack);
	YGJ_ASSERT(m_state == TERM || m_state == EXCEPT || m_state == INIT);

	m_cb = cb;
#ifdef _WIN32
#else
	if (getcontext(&m_ctx)) {
		YGJ_ASSERT2(false, "getcontext");
	}
	m_ctx.uc_link = nullptr;
	m_ctx.uc_stack.ss_sp = m_stack;
	m_ctx.uc_stack.ss_size = m_stacksize;
	makecontext(&m_ctx, &Fiber::MainFunc, 0);
#endif
	m_state = INIT;
}

void Fiber::call() {
	SetThis(this);

	m_state = EXEC;
#ifdef _WIN32
#else
	if (swapcontext(&t_thread_fiber ->m_ctx, &m_ctx)) {
		YGJ_ASSERT2(false, "swapcontext");
	}
#endif
}

void Fiber::back() {
	SetThis(t_thread_fiber.get());
#ifdef _WIN32
#else
	if (swapcontext(&m_ctx, &t_thread_fiber->m_ctx)) {
		YGJ_ASSERT2(false, "swapcontext");
	}
#endif
}

void Fiber::swap_in() {
	SetThis(this);
	YGJ_ASSERT(m_state != EXEC);
	m_state = EXEC;
#ifdef _WIN32
#else
	if (swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx)) {
		YGJ_ASSERT2(false, "swapcontext");
	}
#endif
}

void Fiber::swap_out() {
	SetThis(Scheduler::GetMainFiber());
#ifdef _WIN32
#else
	
	if (swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx)) {
		YGJ_ASSERT2(false, "swapcontext");
	}
#endif
}
void Fiber::SetThis(Fiber* f) {
	t_fiber = f;
}
Fiber::ptr Fiber::GetThis() {
	if(t_fiber) {
		return t_fiber->shared_from_this();
	}
	Fiber::ptr main_fiber(new Fiber);
	YGJ_ASSERT(t_fiber == main_fiber.get());
	t_thread_fiber = main_fiber;
	return t_fiber->shared_from_this();
}

void Fiber::YieldToReady() {
	Fiber::ptr cur = GetThis();
	cur->m_state = READY;
	cur->swap_out();
}

void Fiber::YeidToHold() {
	Fiber::ptr cur = GetThis();
	cur->m_state = HOLD;
	cur->swap_out();
}

uint64_t Fiber::TotalFibers() {
	return s_fiber_count;
}
uint64_t Fiber::GetFiberId() {
	if (t_fiber) {
		return t_fiber->get_id();
	}
	return 0;
}
void Fiber::MainFunc() {
	Fiber::ptr cur = GetThis();
	YGJ_ASSERT(cur);
	try {
		cur->m_cb();
		cur->m_cb = nullptr;
		cur->m_state = TERM;
	}catch (std::exception& ex) {
		cur->m_state = EXCEPT;
		YGJ_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
			<< " fiber_id=" << cur->get_id() << std::endl << ygj_server::BacktraceToString();
	}
	catch (...) {
		cur->m_state = EXCEPT;
		YGJ_LOG_ERROR(g_logger) << "Fiber Except"
			<< " fiber_id=" << cur->get_id() << std::endl << ygj_server::BacktraceToString();
	}
	auto raw_ptr = cur.get();
	cur.reset();
	raw_ptr->swap_out();
	YGJ_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->get_id()));
}



void Fiber::CallMainFunc() {
	Fiber::ptr cur = GetThis();
	YGJ_ASSERT(cur);
	try {
		cur->m_cb();
		cur->m_cb = nullptr;
		cur->m_state = TERM;
	}
	catch (std::exception& ex) {
		cur->m_state = EXCEPT;
		YGJ_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
			<< " fiber_id=" << cur->get_id() << std::endl << ygj_server::BacktraceToString();
	}
	catch (...) {
		cur->m_state = EXCEPT;
		YGJ_LOG_ERROR(g_logger) << "Fiber Except"
			<< " fiber_id=" << cur->get_id() << std::endl << ygj_server::BacktraceToString();
	}
	auto raw_ptr = cur.get();
	cur.reset();
	raw_ptr->back();
	YGJ_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->get_id()));
}

}
