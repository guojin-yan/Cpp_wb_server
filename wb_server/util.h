#ifndef _YGJ_UTIL_H_
#define _YGJ_UTIL_H_

#include <string>
#include <vector>

#include <stdint.h>
#include <pthread.h>
#ifdef _WIN32
#else
#include <cxxabi.h>

#include <unistd.h>
#include <sys/syscall.h>
#endif


namespace ygj_server {

/// @brief 获取线程号
/// @return 线程号
pid_t get_thread_id();

uint32_t get_fiber_id();

void Backtrace(std::vector<std::string>& bt, int size = 64, int skip=1);
std::string BacktraceToString(int size = 64, int skip=2, const std::string& prefix="");
}

#endif // !_YGJ_UTIL_H_

