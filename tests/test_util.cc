#include "../wb_server/wb_server.h"
#include <assert.h>

ygj_server::Logger::ptr g_logger_uitl = YGJ_LOG_ROOT();
void test_assert() {
	YGJ_LOG_INFO(g_logger_uitl) << ygj_server::BacktraceToString(10);
	YGJ_ASSERT2(0==1,"abcef  xx");
}
int main4(int argc, char** argv) {
	test_assert();
	return  0;
}