#include "thread.h"


#include <pthread.h>


#include "log.h"
#include "util.h"

namespace ygj_server {
static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";

static ygj_server::Logger::ptr g_logger = YGJ_LOG_NAME("system");

Semaphore::Semaphore(uint32_t count ) {
	if(sem_init(&m_semaphore, 0, count)) {
		throw std::logic_error("sem_init error");
	}
}
Semaphore::~Semaphore() {
	sem_destroy(&m_semaphore);
}
void Semaphore::wait() {
	if (sem_wait(&m_semaphore)) {
		//throw std::logic_error("sem_wait error");
	}
}
	
void Semaphore::notify() {
	if(sem_post(&m_semaphore)) {
		throw std::logic_error("sem_post error");
	}
}

Thread* Thread::GetThis() {
	return t_thread;
}
const std::string& Thread::GetName() {
	return t_thread_name;
}


void Thread::SetName(const std::string& name) {
	if(t_thread) {
		t_thread->m_name = name;
	}
	t_thread_name = name;
}

Thread::Thread(std::function<void()> cb, const std::string& name)
	:m_cb(cb), m_name(name) {
	if(name.empty()) {
		m_name = "UNKNOW";
	}
	int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
	if(rt) {
		YGJ_LOG_ERROR(g_logger) << "pthread_create thread fail, rt=" << rt << " name" << name;
		throw std::logic_error("pthread_create thread error");
	}
	m_semaphore.wait();
}
Thread::~Thread() {
#ifdef _WIN32
	if (m_thread.x) {
#else
	if (m_thread) {
#endif
		pthread_detach(m_thread);
	}
	
}
void* Thread::run(void* arg) {
	Thread* thread = (Thread*)arg;
	t_thread = thread;
	thread->m_id = ygj_server::get_thread_id();
#ifdef _WIN32

#else
	pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());
#endif
	std::function<void()> cb;
	cb.swap(thread->m_cb);
	thread->m_semaphore.notify();
	cb();
	return 0;
}
void Thread::join() {
#ifdef _WIN32
	if (m_thread.x) {
#else
	if (m_thread) {
#endif
		int rt = pthread_join(m_thread, nullptr);
		if (rt) {
			YGJ_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" << rt << " name" << m_name;
			throw std::logic_error("pthread_join thread error");
		}

#ifdef _WIN32
		m_thread.x = 0;
#else
		m_thread = 0;
#endif
		
	}
}

}
