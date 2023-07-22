#include "../wb_server/wb_server.h"

ygj_server::Logger::ptr g_logger_fiber = YGJ_LOG_ROOT();

void run_in_fiber() {
	YGJ_LOG_INFO(g_logger_fiber) << "run_in_fiber begin";
	ygj_server::Fiber::YeidToHold();
	YGJ_LOG_INFO(g_logger_fiber) << "run_in_fiber end";
	ygj_server::Fiber::YeidToHold();
}

void test_fiber() {
	YGJ_LOG_INFO(g_logger_fiber) << "test_fiber begin -1";
	{
		ygj_server::Fiber::GetThis();
		YGJ_LOG_INFO(g_logger_fiber) << "test_fiber begin";
		ygj_server::Fiber::ptr fiber(new ygj_server::Fiber(run_in_fiber));
		fiber->swap_in();
		YGJ_LOG_INFO(g_logger_fiber) << "test_fiber after swap_in";
		fiber->swap_in();
		YGJ_LOG_INFO(g_logger_fiber) << "test_fiber after end";
		fiber->swap_in();
	}
	YGJ_LOG_INFO(g_logger_fiber) << "test_fiber after end2";
}

int main5(int argc, char** argv) {
	ygj_server::Thread::SetName("main");
	std::vector<ygj_server::Thread::ptr> threads;
	for(int i = 0; i < 3; ++i) {
		threads.push_back(ygj_server::Thread::ptr(
			new ygj_server::Thread(&test_fiber, "name_" + std::to_string(i))));
	}

	for (auto i:threads) {
		i->join();
	}
	return  0;
}