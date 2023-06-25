#include <iostream>
#ifdef _WIN32
#include "../wb_server/log.h"
#else
#include "../wb_server/log.h"
#endif



int main(int argc, char** argv) {
    ygj_server::Logger::ptr logger(new ygj_server::Logger);
    logger->add_appender(ygj_server::LogAppender::ptr(new ygj_server::StdoutLogAppender));

    ygj_server::LogEvevt::ptr event(new ygj_server::LogEvevt(__FILE__, __LINE__, 0, 1, 2, time(0)));

    event->get_ss() << "hello ygj server log!";
    logger->log(ygj_server::LogLevel::DEBUG, event);

    std::cout << "hello ygj server!!" << std::endl;
	return 0;
}