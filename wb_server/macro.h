#ifndef _YGJ_MACRO_H_
#define _YGJ_MACRO_H_

#include <assert.h>
#include <string.h>

#define YGJ_ASSERT(x) \
	if(!(x)){ \
		YGJ_LOG_ERROR(YGJ_LOG_ROOT()) << "ASSERTION: " #x \
			<< "\nbacktrace:\n" \
			<< ygj_server::BacktraceToString(100,2,"    "); \
		assert(x);\
	}

#define YGJ_ASSERT2(x, w) \
	if(!(x)){ \
		YGJ_LOG_ERROR(YGJ_LOG_ROOT()) << "ASSERTION: " #x \
			<< "\n" << w \
			<< "\nbacktrace:\n" \
			<< ygj_server::BacktraceToString(100,2,"    "); \
		assert(x);\
	}
#endif
