#include "util.h"

#include <thread>

#include <stdint.h>

#ifdef _WIN32
#else
#include <cxxabi.h>
#include <execinfo.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#endif

#include "fiber.h"
#include "log.h"

namespace ygj_server {

	ygj_server::Logger::ptr g_logger = YGJ_LOG_NAME("system");

pid_t get_thread_id() {

#ifdef _WIN32

	std::thread::id id = std::this_thread::get_id();
	int thread_id = *(uint32_t*)&id;
	return thread_id;

#else

	return syscall(SYS_gettid);

#endif
}

uint32_t get_fiber_id() {
	return Fiber::GetFiberId();
}

void Backtrace(std::vector<std::string>& bt, int size, int skip) {
#ifdef _WIN32
#else
	void** array = (void**)malloc(sizeof(void*) * size);
	size_t s = ::backtrace(array, size);
	char** strings = backtrace_symbols(array, s);
	if(strings == NULL) {
		YGJ_LOG_ERROR(g_logger) << "backtrace_symbols error";
		return;
	}
	for(size_t i = skip; i < s; ++i) {
		bt.push_back(strings[i]);
	}
	free(strings);
	free(array);
#endif

}
std::string BacktraceToString(int size, int skip, const std::string& prefix) {
	std::vector<std::string> bt;
	Backtrace(bt, size, skip);
	std::stringstream ss;
	for(size_t i = 0; i < bt.size(); ++i) {
		ss << prefix << bt[i] << std::endl;
	}
	return ss.str();
}

}
