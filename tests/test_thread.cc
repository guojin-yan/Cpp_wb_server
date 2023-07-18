#include "../wb_server/wb_server.h"

#ifdef _WIN32
#include<windows.h>
#else
#include <unistd.h>
#endif



ygj_server::Logger::ptr g_logger = YGJ_LOG_NAME("root");

int count = 0;
//ygj_server::RWMutex s_mutex;
ygj_server::Mutex s_mutex;

void fun1() {
	YGJ_LOG_INFO(g_logger) << "thread_name=" << ygj_server::Thread::GetName()
	                       << " this_name=" << ygj_server::Thread::GetThis()->get_name()
	                       << " thread_id=" << ygj_server::get_thread_id()
	                       << " this_id=" << ygj_server::Thread::GetThis()->get_id() << std::endl;
	for(int i = 0; i < 10000000; ++i) {
		//ygj_server::RWMutex::ReadLock lock(s_mutex);
		//ygj_server::Mutex::Lock lock(s_mutex);
		++count;
	}
}
void fun2() {
	while(true) {
		YGJ_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxx";
	}
}
void fun3() {
	while(true) {
		YGJ_LOG_INFO(g_logger) << "====================";
	}
}
int main(int argc, char** argv) {
	YGJ_LOG_INFO(g_logger) << "thread test begin.";
#ifdef _WIN32
	YAML::Node root = YAML::LoadFile("E:\\VMwareShare\\C++高性能服务器框架\\config\\log2.yml");
#else
	YAML::Node root = YAML::LoadFile("/mnt/hgfs/cpp_wb_server/config/log2.yml");
#endif
	ygj_server::Config::load_from_yaml(root);

	std::vector<ygj_server::Thread::ptr> threads;

	for(int i = 0; i < 2; ++i) {
		ygj_server::Thread::ptr thread(new ygj_server::Thread(&fun2, "name_" + std::to_string((i*2))));
		//ygj_server::Thread::ptr thread2(new ygj_server::Thread(&fun3, "name_" + std::to_string((i*2+1))));
		threads.push_back(thread);
		//threads.push_back(thread2);
	}



	for (size_t i = 0; i < threads.size(); ++i) {
		threads[i]->join();
	}

	YGJ_LOG_INFO(g_logger) << "thread test end.";
	//#ifdef _WIN32
	//	Sleep(5000);
	//#else
	//	sleep(5);
	//#endif
	//YGJ_LOG_INFO(g_logger) << "count=" << count;

	return  0;
}