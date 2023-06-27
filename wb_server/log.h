/// @file log.h
/// @brief 日志模块封装
/// @author yanguojin
/// @email 1023438782@qq.com
/// @date 2023-06-24
#ifndef __YGJ_LOG_H__
#define __YGJ_LOG_H__

#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <stdint.h>

#include "singleton.h"


#define YGJ_LOG_LEVEL(logger, level) \
	if(logger->get_level() <= level) \
		ygj_server::LogEventWrap(ygj_server::LogEvent::ptr( new ygj_server::LogEvent(logger, level, \
		__FILE__, __LINE__, 0, ygj_server::get_thread_id(), ygj_server::get_fiber_id(), \
		time(0)))).get_ss()

#define YGJ_LOG_DEBUG(logger) YGJ_LOG_LEVEL(logger, ygj_server::LogLevel::DEBUG)
#define YGJ_LOG_INFO(logger) YGJ_LOG_LEVEL(logger, ygj_server::LogLevel::INFO)
#define YGJ_LOG_WARN(logger) YGJ_LOG_LEVEL(logger, ygj_server::LogLevel::WARN)
#define YGJ_LOG_ERROR(logger) YGJ_LOG_LEVEL(logger, ygj_server::LogLevel::ERROR)
#define YGJ_LOG_FATAL(logger) YGJ_LOG_LEVEL(logger, ygj_server::LogLevel::FATAL)

#define YGJ_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->get_level() <= level) \
        ygj_server::LogEventWrap(ygj_server::LogEvent::ptr(new ygj_server::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, ygj_server::get_thread_id(),\
                ygj_server::get_fiber_id(), time(0)))).get_event()->format(fmt, __VA_ARGS__)

#define YGJ_LOG_FMT_DEBUG(logger, fmt, ...) YGJ_LOG_FMT_LEVEL(logger, ygj_server::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define YGJ_LOG_FMT_INFO(logger, fmt, ...)  YGJ_LOG_FMT_LEVEL(logger, ygj_server::LogLevel::INFO, fmt, __VA_ARGS__)
#define YGJ_LOG_FMT_WARN(logger, fmt, ...)  YGJ_LOG_FMT_LEVEL(logger, ygj_server::LogLevel::WARN, fmt, __VA_ARGS__)
#define YGJ_LOG_FMT_ERROR(logger, fmt, ...) YGJ_LOG_FMT_LEVEL(logger, ygj_server::LogLevel::ERROR, fmt, __VA_ARGS__)
#define YGJ_LOG_FMT_FATAL(logger, fmt, ...) YGJ_LOG_FMT_LEVEL(logger, ygj_server::LogLevel::FATAL, fmt, __VA_ARGS__)


namespace ygj_server {
class Logger;
class LogEvent;
class LogEventWrap;

/// @brief 日志级别
class LogLevel {
public:
	enum Level { // 日志级别
		UNKNOW = 0,
		DEBUG = 1,
		INFO = 2,
		WARN = 3,
		ERROR = 4,
		FATAL = 5
	};
	static const char* tostring(LogLevel::Level level);
};

/// @brief 日志事件
class LogEvent {
public:
	typedef std::shared_ptr<LogEvent> ptr;
	LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, 
		uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time);
	const char* get_file_name() const { return m_file; }
	int32_t get_line() { return m_line; }
	uint32_t get_elapse() { return m_elapse; }
	int32_t get_thread() { return m_thread; }
	uint32_t get_fiberi() { return m_fiberid; }
	uint64_t get_time() { return m_time; }
	std::string get_content() const { return m_ss.str(); } 
	std::shared_ptr<Logger> get_logger() { return m_logger; }
	LogLevel::Level get_level() { return m_level; }
	std::stringstream& get_ss() { return m_ss; }
	void format(const char* fmt, ...);
	void format(const char* fmt, va_list al);
private:
	std::shared_ptr<Logger> m_logger;
	LogLevel::Level m_level;
	const char* m_file = nullptr; // 文件名你
	int32_t m_line = 0; // 行号
	uint32_t m_elapse = 0; // 程序启动开始到现在的毫秒数
	int32_t m_thread = 0; // 线程ID
	uint32_t m_fiberid = 0; //携程ID
	uint64_t m_time; // 时间戳
	std::stringstream m_ss; // 线程名称

};


class LogEventWrap {
public:
	LogEventWrap(LogEvent::ptr event);
	~LogEventWrap();
	LogEvent::ptr get_event() const { return m_event; }
	std::stringstream& get_ss();
private:
	LogEvent::ptr m_event;
};



/// @brief 日志格式化
class LogFormatter {
public:
	typedef std::shared_ptr<LogFormatter> ptr;
	/// @brief 构造函数
	/// @param[in] pattern 格式模板
	/// @details
	///  %m 消息 %p 日志级别 %r 累计毫秒数 %c 日志名称 %t 线程id %n 换行 %d 时间
	///  %f 文件名 %l 行号 %T 制表符 %F 协程id %N 线程名称
	///  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
	LogFormatter(const std::string& pattern);
	std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt);
	std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
	/// @brief 日志内容项格式化
	class FormatItem{
	public:
		typedef std::shared_ptr<FormatItem> ptr;
		virtual ~FormatItem(){}
		virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) = 0;
	};
private:
	void init();
	std::string m_pattern;
	std::vector<FormatItem::ptr> m_items;
};

// @brief 日志格式化
class LogAppender {
public:
	typedef std::shared_ptr<LogAppender> ptr;
	virtual ~LogAppender() {}
	virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, const LogEvent::ptr evevt) = 0;
	void set_format(LogFormatter::ptr val) { m_formatter = val; }
	LogFormatter::ptr get_format() const { return m_formatter; }
	LogLevel::Level get_level() const { return m_level; }
	void set_level(LogLevel::Level level) { m_level = level; }
protected:
	LogLevel::Level m_level = LogLevel::DEBUG; // 日志级别
	LogFormatter::ptr m_formatter; // 输出格式
};



// @brief 日志
class Logger : public std::enable_shared_from_this<Logger> {
public:
	typedef std::shared_ptr<Logger> ptr;

	Logger(const std::string name = "root");
	void log(LogLevel::Level level, const LogEvent::ptr event);

	void debug(LogEvent::ptr event);
	void info(LogEvent::ptr eventt);
	void warn(LogEvent::ptr event);
	void error(LogEvent::ptr event);
	void fatal(LogEvent::ptr event);
	void add_appender(LogAppender::ptr appender);
	void del_appender(LogAppender::ptr appender);
	LogLevel::Level get_level() const { return m_level; }
	void set_level(LogLevel::Level val) { m_level = val; }

	const std::string get_name() const { return m_name; }
private:
	std::string m_name; // 日志名
	LogLevel::Level m_level; // 日志级别
	std::list<LogAppender::ptr> m_appenders; // Appender集合
	LogFormatter::ptr m_formatter;
};

// @brief 输出到控制台的Appender
class StdoutLogAppender : public LogAppender{
public:
	typedef std::shared_ptr<StdoutLogAppender> ptr;
	void log(std::shared_ptr<Logger> logger, LogLevel::Level level, const LogEvent::ptr event) override;
};

// @brief 输出到文件的Appender
class FileLogAppender : public LogAppender{
public:
	typedef std::shared_ptr<FileLogAppender> ptr;
	FileLogAppender(const std::string file_name);
	void log(std::shared_ptr<Logger> logger, LogLevel::Level level, const LogEvent::ptr event) override;

	bool reopen(); // 重新打开文件，文件打开成功返回true
private:
	std::string m_file_name;
	std::ofstream m_file_stream;
	/// 上次重新打开时间
	uint64_t m_lastTime = 0;
};


class LoggerManager {
public:
	LoggerManager();
	Logger::ptr get_logger(const std::string& name);
	void init();
private:
	std::map<std::string, Logger::ptr> m_loggers;
	Logger::ptr m_root;
};

typedef ygj_server::Singleton<LoggerManager> loggerMgr;
}


#endif // __SYLAR_LOG_H__
