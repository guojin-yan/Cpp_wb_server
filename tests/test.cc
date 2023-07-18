#include <iostream>
#include <thread>
#ifdef _WIN32
#include "../wb_server/log.h"
#include "../wb_server/util.h"
#else
#include "../wb_server/log.h"
#include "../wb_server/util.h"
#endif



int main1(int argc, char** argv) {

    ygj_server::Logger::ptr logger(new ygj_server::Logger);
    logger->add_appender(ygj_server::LogAppender::ptr(new ygj_server::StdoutLogAppender));

    ygj_server::FileLogAppender::ptr file_appender(new ygj_server::FileLogAppender("./log.txt"));

    ygj_server::LogFormatter::ptr fmt(new ygj_server::LogFormatter("%d%T%p%T%m%n"));
    file_appender->set_formatter(fmt);
    file_appender->set_level(ygj_server::LogLevel::ERROR);
    logger->add_appender(file_appender);

    //ygj_server::LogEvent::ptr event(new ygj_server::LogEvent(__FILE__, __LINE__, 0, ygj_server::get_thread_id(), ygj_server::get_fiber_id(), time(0)));
    //event->get_ss() << "hello ygj server log!";
    //logger->log(ygj_server::LogLevel::DEBUG, event);


    std::cout << "hello ygj server!!" << std::endl;
    YGJ_LOG_DEBUG(logger) << "test macro debug;";
    YGJ_LOG_INFO(logger) << "test macro info;";
    YGJ_LOG_WARN(logger) << "test macro warn;";
    YGJ_LOG_ERROR(logger) << "test macro error;";
    YGJ_LOG_FATAL(logger) << "test macro fatal;";

    YGJ_LOG_FMT_DEBUG(logger, "test macro debug %s","aa");
    //YGJ_LOG_FMT_INFO(logger, "test macro info;", "aa");
    //YGJ_LOG_FMT_WARN(logger, "test macro warn;", "aa");
    //YGJ_LOG_FMT_ERROR(logger, "test macro error;", "aa");
    //YGJ_LOG_FMT_FATAL(logger, "test macro fatal;", "aa");

    auto l = ygj_server::LoggerMgr::get_instance()->get_logger("xx");
    YGJ_LOG_INFO(l) << "xxx";
	return 0;
}
