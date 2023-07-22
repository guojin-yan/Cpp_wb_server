#include "scheduler.h"

#include "log.h"
#include "macro.h"


namespace ygj_server {
static ygj_server::Logger::ptr g_logger = YGJ_LOG_NAME("system");
static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name) {
	YGJ_ASSERT(threads > 0);

	if(use_caller) {
		ygj_server::Fiber::GetThis();
		--threads;

		YGJ_ASSERT(GetThis() == nullptr);
		t_scheduler = this;
		m_root_fiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
		ygj_server::Thread::SetName(m_name);

		t_fiber = m_root_fiber.get();
		m_root_thread = ygj_server::get_thread_id();
		m_thread_id.push_back(m_root_thread);
	} else {
		m_root_thread = -1;
	}
	m_thread_count = threads;
}
Scheduler::~Scheduler() {
	YGJ_ASSERT(m_stoping);
	if(GetThis()==this) {
		t_scheduler = nullptr;
	}
}

Scheduler* Scheduler::GetThis() {
	return t_scheduler;
}

Fiber* Scheduler::GetMainFiber() {
	return t_fiber;
}

void Scheduler::start() {
	MutexType::Lock lock(m_mutex);
	if(!m_stoping) {
		return;
	}

	m_stoping = false;
	YGJ_ASSERT(m_threads.empty());
	for(size_t i = 0; i < m_thread_count; ++i) {
		m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this), 
			m_name + "-" + std::to_string(i)));
		m_thread_id.push_back(m_threads[i]->get_id());
	}
	lock.unlock();
	if (m_root_fiber) {
		//m_root_fiber->swap_in();
		m_root_fiber->call();
		YGJ_LOG_INFO(g_logger) << "call out " << m_root_fiber->get_state();
	}
}
void Scheduler::stop() {
	m_auto_stop = true;
	if(m_root_fiber && m_thread_count == 0
		&& (m_root_fiber->get_state() == Fiber::TERM  
			|| m_root_fiber->get_state() == Fiber::INIT)) {
		YGJ_LOG_INFO(g_logger) << this << " stopped";
		m_stoping = true;
		if(stopping()) {
			return;
		}
	}

	//bool exit_on_this_fiber = false;
	if(m_root_thread != -1) {
		YGJ_ASSERT(GetThis() == this);
	} else {
		YGJ_ASSERT(GetThis() != this);
	}
	m_stoping = true;
	for(size_t i = 0; i < m_thread_count; ++i) {
		tickel();
	}
	if(m_root_fiber) {
		tickel();
	}
	if(stopping()) {
		return;
	}
}
void Scheduler::set_this() {
	t_scheduler = this;
}
void Scheduler::run() {
	YGJ_LOG_DEBUG(g_logger) << m_name << " run";
	set_this();
	if(ygj_server::get_thread_id() != m_root_thread) {
		t_fiber = Fiber::GetThis().get();
	}
	Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
	Fiber::ptr cb_fiber;
	FiberAndThread ft;
	while(true) {
		ft.reset();
		bool tickle_me = false;
		//bool is_active = false;
		{
			MutexType::Lock lock(m_mutex);
			auto it = m_fibers.begin();
			while(it != m_fibers.end()) {
				if(it->thread != -1 && it->thread != ygj_server::get_thread_id()) {
					++it;
					tickle_me = true;
					continue;
				}

				YGJ_ASSERT(it->fiber || it->cb);
				if(it->fiber && it->fiber->get_state() == Fiber::EXEC) {
					++it;
					continue;
				}
				ft = *it;
				m_fibers.erase(it);
				break;
			}
		}
		if(tickle_me) {
			tickel();
		}
		if(ft.fiber && (ft.fiber->get_state() != Fiber::TERM 
			&& ft.fiber->get_state() != Fiber::EXCEPT)) {

			++m_active_thread_count;
			ft.fiber->swap_in();
			--m_active_thread_count;

			if(ft.fiber->get_state() == Fiber::READY) {
				scheduler(ft.fiber);
			} else if (ft.fiber->get_state() != Fiber::TERM
				&& ft.fiber->get_state() != Fiber::EXCEPT) {
				ft.fiber->m_state = Fiber::HOLD;
			}
			ft.reset();
		} else if (ft.cb) {
			if (cb_fiber) {
				cb_fiber->reset(ft.cb);
			}
			else {
				cb_fiber.reset(new Fiber(ft.cb));
			}
			ft.reset();
			++m_active_thread_count;
			cb_fiber->swap_in();
			--m_active_thread_count;
	
			if(cb_fiber->get_state() == Fiber::READY) {
				scheduler(cb_fiber);
				cb_fiber.reset();
			} else if(cb_fiber->get_state() == Fiber::TERM
				|| cb_fiber->get_state() == Fiber::EXCEPT) {
				cb_fiber->reset(nullptr);
			} else {//if(cb_fiber->get_state() != Fiber::TERM)
				cb_fiber->m_state = Fiber::HOLD;
				cb_fiber.reset();
			}
		} else {
			//if (is_active) {
			//	--m_active_thread_count;
			//	continue;
			//}
			if (idle_fiber->get_state() == Fiber::TERM) {
				YGJ_LOG_INFO(g_logger) << "idle fiber term";
				break;
			}

			++m_idle_thread_count;
			idle_fiber->swap_in();
			--m_idle_thread_count;
			if (idle_fiber->get_state() != Fiber::TERM
				&& idle_fiber->get_state() != Fiber::EXCEPT) {
				idle_fiber->m_state = Fiber::HOLD;
			}
		}
	}
}


void Scheduler::tickel() {
	YGJ_LOG_INFO(g_logger) << "tickle";
}
bool Scheduler::stopping() {
	MutexType::Lock lock(m_mutex);
	return m_auto_stop && m_stoping
		&& m_fibers.empty() && m_active_thread_count == 0;
}
void Scheduler::idle() {
	YGJ_LOG_INFO(g_logger) << "idle";
	//while (!stopping()) {
	//	sylar::Fiber::YieldToHold();
	//}
}

}