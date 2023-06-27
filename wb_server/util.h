#ifndef _YGJ_UTIL_H_
#define _YGJ_UTIL_H_


#include <stdint.h>

#ifdef _WIN32
#else
#include <cxxabi.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#endif


namespace ygj_server {

/// @brief 获取线程号
/// @return 线程号
uint32_t get_thread_id();

uint32_t get_fiber_id();

}

#endif // !_YGJ_UTIL_H_

