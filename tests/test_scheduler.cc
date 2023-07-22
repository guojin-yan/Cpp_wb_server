#include "../wb_server/wb_server.h"
#include <assert.h>

ygj_server::Logger::ptr g_logger_scheduler = YGJ_LOG_ROOT();

void test_fiber11() {
	YGJ_LOG_INFO(g_logger_scheduler) << "test in fiber11";

}


int main(int argc, char** argv) {
	YGJ_LOG_INFO(g_logger_scheduler) << "main";
	ygj_server::Scheduler sc;
	sc.scheduler(&test_fiber11);
	sc.start();

	sc.stop();
	YGJ_LOG_INFO(g_logger_scheduler) << "over";
	return  0;
}