//#include "util.h"




#include <vector>
#include <string>
#include <iomanip>

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>

#include <thread>
#ifdef _WIN32
#else
#include <cxxabi.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#endif

namespace ygj_server {

uint32_t get_thread_id() {

#ifdef _WIN32

	std::thread::id id = std::this_thread::get_id();
	int thread_id = *(uint32_t*)&id;
	return thread_id;

#else

	return syscall(SYS_gettid);

#endif
}

uint32_t get_fiber_id() {
	return 0;
}


}
