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
#include "thread.h"
#include "util.h"

/// @brief 使用流式方式将日志级别level的日志写入到logger
#define YGJ_LOG_LEVEL(logger, level)\
	if(logger->get_level() <= level)\
		ygj_server::LogEventWrap(ygj_server::LogEvent::ptr( new ygj_server::LogEvent(logger, level,\
		__FILE__, __LINE__, 0, ygj_server::get_thread_id(), ygj_server::get_fiber_id(),\
		time(0)))).get_ss()
/// @brief 使用流式方式将日志级别debug的日志写入到logger
#define YGJ_LOG_DEBUG(logger) YGJ_LOG_LEVEL(logger, ygj_server::LogLevel::DEBUG)
/// @brief 使用流式方式将日志级别INFO的日志写入到logger
#define YGJ_LOG_INFO(logger) YGJ_LOG_LEVEL(logger, ygj_server::LogLevel::INFO)
/// @brief 使用流式方式将日志级别WARN的日志写入到logger
#define YGJ_LOG_WARN(logger) YGJ_LOG_LEVEL(logger, ygj_server::LogLevel::WARN)
/// @brief 使用流式方式将日志级别ERROR的日志写入到logger
#define YGJ_LOG_ERROR(logger) YGJ_LOG_LEVEL(logger, ygj_server::LogLevel::ERROR)
/// @brief 使用流式方式将日志级别FATAL的日志写入到logger
#define YGJ_LOG_FATAL(logger) YGJ_LOG_LEVEL(logger, ygj_server::LogLevel::FATAL)

/// @brief 使用格式化方式将日志级别level的日志写入到logger
#define YGJ_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->get_level() <= level) \
        ygj_server::LogEventWrap(ygj_server::LogEvent::ptr(new ygj_server::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, ygj_server::get_thread_id(),\
                ygj_server::get_fiber_id(), time(0)))).get_event()->format(fmt, __VA_ARGS__)
/// @brief 使用格式化方式将日志级别debug的日志写入到logger
#define YGJ_LOG_FMT_DEBUG(logger, fmt, ...) YGJ_LOG_FMT_LEVEL(logger, ygj_server::LogLevel::DEBUG, fmt, __VA_ARGS__)
/// @brief 使用格式化方式将日志级别INFO的日志写入到logger
#define YGJ_LOG_FMT_INFO(logger, fmt, ...)  YGJ_LOG_FMT_LEVEL(logger, ygj_server::LogLevel::INFO, fmt, __VA_ARGS__)
/// @brief 使用格式化方式将日志级别WARN的日志写入到logger
#define YGJ_LOG_FMT_WARN(logger, fmt, ...)  YGJ_LOG_FMT_LEVEL(logger, ygj_server::LogLevel::WARN, fmt, __VA_ARGS__)
/// @brief 使用格式化方式将日志级别ERROR的日志写入到logger
#define YGJ_LOG_FMT_ERROR(logger, fmt, ...) YGJ_LOG_FMT_LEVEL(logger, ygj_server::LogLevel::ERROR, fmt, __VA_ARGS__)
/// @brief 使用格式化方式将日志级别FATAL的日志写入到logger
#define YGJ_LOG_FMT_FATAL(logger, fmt, ...) YGJ_LOG_FMT_LEVEL(logger, ygj_server::LogLevel::FATAL, fmt, __VA_ARGS__)

/// @brief 获取主日志器
#define YGJ_LOG_ROOT() ygj_server::LoggerMgr::get_instance()->get_root()

/// @brief 获取name的日志器
#define YGJ_LOG_NAME(name) ygj_server::LoggerMgr::get_instance()->get_logger(name)

namespace ygj_server {
class Logger;
class LogEvent;
class LogEventWrap;
class LoggerManager;

/// @brief 日志级别
class LogLevel {
public:
	/// @brief 日志级别枚举
	enum Level { // 日志级别
		UNKNOW = 0,
		DEBUG = 1,
		INFO = 2,
		WARN = 3,
		ERROR = 4,
		FATAL = 5
	};
	/// @brief 将日志级别转成文本输出
	/// @param[in] level 日志级别
	/// @return 日志级别文本
	static const char* to_string(LogLevel::Level level);
	static LogLevel::Level from_string(const std::string& str);
};

/// @brief 日志事件
class LogEvent {
public:
	typedef std::shared_ptr<LogEvent> ptr;
	/// @brief 构造函数
	/// @param[in] logger 日志器
	/// @param[in] level 日志级别
	/// @param[in] file 文件名
	/// @param[in] line 文件行号
	/// @param[in] elapse 程序启动依赖的耗时(毫秒)
	/// @param[in] thread_id 线程id
	/// @param[in] fiber_id 协程id
	/// @param[in] time 日志事件(秒)
	LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, 
		uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time);
	/// @brief 获取文件名
	/// @return 文件名
	const char* get_file_name() const { return m_file; }
	/// @brief 获取行号
	/// @return 行号
	int32_t get_line() { return m_line; }
	/// @brief 获取耗时
	/// @return 耗时
	uint32_t get_elapse() { return m_elapse; }
	/// @brief 获取线程ID
	/// @return 线程ID
	int32_t get_thread() { return m_thread; }
	/// @brief 获取协程ID
	/// @return 协程ID
	uint32_t get_fiberi() { return m_fiberid; }
	/// @brief 获取时间
	/// @return 时间
	uint64_t get_time() { return m_time; }
	/// @brief 获取日志内容
	/// @return 日志内容
	std::string get_content() const { return m_ss.str(); }
	/// @brief 获取日志器
	/// @return 日志器
	std::shared_ptr<Logger> get_logger() { return m_logger; }
	/// @brief 获取日志级别
	/// @return 日志级别
	LogLevel::Level get_level() { return m_level; }
	/// @brief 获取日志内容字符串流
	/// @return 日志内容字符串流
	std::stringstream& get_ss() { return m_ss; }
	/// @brief 格式化写入日志内容
	/// @param fmt 格式
	/// @param  其他
	void format(const char* fmt, ...);
	/// @brief 
	/// @param fmt 格式
	/// @param al 格式化写入日志内容
	void format(const char* fmt, va_list al);
private:
	/// @brief 日志器
	std::shared_ptr<Logger> m_logger;
	/// @brief 日志等级
	LogLevel::Level m_level;
	/// @brief 文件名你
	const char* m_file = nullptr;
	/// @brief 行号
	int32_t m_line = 0;
	/// @brief 程序启动开始到现在的毫秒数
	uint32_t m_elapse = 0;
	/// @brief 线程ID
	int32_t m_thread = 0;
	/// @brief 协程ID
	uint32_t m_fiberid = 0; 
	/// @brief 时间
	uint64_t m_time;
	/// @brief 日志内容流
	std::stringstream m_ss; 

};

/// @brief 日志事件包装器
class LogEventWrap {
public:
	/// @brief 构造函数
	/// @param event 日志事件
	LogEventWrap(LogEvent::ptr event);
	/// @brief 析构函数
	~LogEventWrap();
	/// @brief 获取日志事件
	/// @return 日志事件
	LogEvent::ptr get_event() const { return m_event; }
	/// @brief 获取日志内容流
	/// @return 日志内容流
	std::stringstream& get_ss();
private:
	/// @brief 日志事件
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
	/// @brief 获取格式化日志文本
	/// @param logger 日志器
	/// @param level 日志级别
	/// @param evevt 日志事件
	/// @return 格式化日志文本
	std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt);
	/// @brief 获取格式化日志内容流
	/// @param ofs 日志输出流
	/// @param logger 日志器
	/// @param level 日志级别
	/// @param evevt 日志事件
	/// @return 格式化日志流
	std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
	/// @brief 日志内容项格式化
	class FormatItem{
	public:
		typedef std::shared_ptr<FormatItem> ptr;
		virtual ~FormatItem(){}
		/// @brief 格式化日志到流
		/// @param os 日志输出流
		/// @param logger 日志器
		/// @param level 日志级别
		/// @param evevt 日志事件
		virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) = 0;
	};
	/// @brief 初始化,解析日志模板
	void init();
	bool is_error() const { return m_error; }
	const std::string get_pattern() const { return m_pattern; }
private:
	
	/// @brief 日志模板
	std::string m_pattern;
	/// @brief 日志格式解析后格式
	std::vector<FormatItem::ptr> m_items;
	bool m_error = false;
};

// @brief 日志格式化
class LogAppender {
	friend  class Logger;
public:
	typedef std::shared_ptr<LogAppender> ptr;
	typedef Spinlock MutexType;
	virtual ~LogAppender() {}
	/// @brief 写入日志
	/// @param logger 日志器
	/// @param level 日志级别
	/// @param evevt 日志事件
	virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, const LogEvent::ptr evevt) = 0;
	virtual  std::string to_yaml_string() = 0;
	/// @brief 更改日志格式器
	/// @param val 日志模板
	void set_formatter(LogFormatter::ptr val);
	/// @brief 获取日志格式器
	/// @return 日志模板
	LogFormatter::ptr get_formatter();
	/// @brief 获取日志等级
	/// @return 日志等级
	LogLevel::Level get_level() const { return m_level; }
	/// @brief 设置日志等级
	/// @param level 日志等级
	void set_level(LogLevel::Level level) { m_level = level; }
protected:
	/// @brief 日志级别
	LogLevel::Level m_level = LogLevel::DEBUG; // 日志级别
	/// @brief 日志格式器
	LogFormatter::ptr m_formatter; // 输出格式
	/// @brief 
	bool m_has_formatter = false;
	/// @brief 互斥量
	MutexType m_mutex;
};



// @brief 日志
class Logger : public std::enable_shared_from_this<Logger> {
	friend class LoggerManager;
public:
	typedef std::shared_ptr<Logger> ptr;
	typedef Spinlock MutexType;
	/// @brief 构造函数
	/// @param name 日志器名称
	Logger(const std::string name = "root");
	/// @brief 写入日志
	/// @param level 日志级别
	/// @param event 日志事件
	void log(LogLevel::Level level, const LogEvent::ptr event);
	/// @brief 写debug级别日志
	/// @param[in] event 日志事件
	void debug(LogEvent::ptr event);
	/// @brief 写info级别日志
	/// @param[in] event 日志事件
	void info(LogEvent::ptr eventt);
	/// @brief 写warn级别日志
	/// @param[in] event 日志事件
	void warn(LogEvent::ptr event);
	/// @brief 写error级别日志
	/// @param[in] event 日志事件
	void error(LogEvent::ptr event);
	/// @brief 写fatal级别日志
	/// @param[in] event 日志事件
	void fatal(LogEvent::ptr event);
	/// @brief 添加日志目标
	/// @param appender 日志目标
	void add_appender(LogAppender::ptr appender);
	/// @brief 删除日志目标
	/// @param appender 日志目标
	void del_appender(LogAppender::ptr appender);
	/// @brief 获取日志级别
	/// @return 日志级别
	LogLevel::Level get_level() const { return m_level; }
	/// @brief 清除appender
	void clear_appender();
	/// @brief 设置日志级别
	/// @param val 日志级别
	void set_level(LogLevel::Level val) { m_level = val; }
	/// @brief 获取日志名
	/// @return 日志名
	const std::string get_name() const { return m_name; }
	/// @brief 设置日志格式
	/// @param val 日志格式
	void set_formatter(LogFormatter::ptr val);
	/// @brief 设置日志格式
	/// @param val 日志格式
	void set_formatter(const std::string& val);
	/// @brief 获取日志格式
	/// @return 日志格式
	LogFormatter::ptr get_formatter();
	std::string to_yaml_string();
private:
	/// @brief 日志名
	std::string m_name; // 日志名
	/// @brief 日志级别
	LogLevel::Level m_level; // 日志级别
	/// @brief 日志目标
	std::list<LogAppender::ptr> m_appenders; // Appender集合
	/// @brief 日志格式器
	LogFormatter::ptr m_formatter;

	Logger::ptr m_root;
	/// @brief 互斥量
	MutexType m_mutex;
};

// @brief 输出到控制台的Appender
class StdoutLogAppender : public LogAppender{
public:
	typedef std::shared_ptr<StdoutLogAppender> ptr;
	/// @brief 控制台写入日志
	/// @param logger 日志器
	/// @param level 日志级别
	/// @param event 日志事件
	void log(std::shared_ptr<Logger> logger, LogLevel::Level level, const LogEvent::ptr event) override;
	std::string to_yaml_string() override;
};

// @brief 输出到文件的Appender
class FileLogAppender : public LogAppender{
public:
	typedef std::shared_ptr<FileLogAppender> ptr;
	/// @brief 构造函数
	/// @param file_name 日志名 
	FileLogAppender(const std::string file_name);
	/// @brief 文件写入日志
	/// @param logger 日志器
	/// @param level 日志级别
	/// @param event 日志事件
	void log(std::shared_ptr<Logger> logger, LogLevel::Level level, const LogEvent::ptr event) override;
	/// @brief 重新打开日志文件
	/// @return 是否打开成功
	bool reopen(); // 重新打开文件，文件打开成功返回true
	std::string to_yaml_string() override;
private:
	/// @brief 文件名
	std::string m_file_name;
	/// @brief 文件流
	std::ofstream m_file_stream;
	/// @brief 上次重新打开时间
	uint64_t m_last_time = 0;
};

/// @brief 日志器管理类
class LoggerManager {
public:
	typedef Spinlock MutexType;
	LoggerManager();
	/// @brief 获取日志器
	/// @param name 日志名称
	/// @return 日志器
	Logger::ptr get_logger(const std::string& name);
	/// @brief 初始化
	void init();
	/// @brief 获取主日志器
	/// @return 主日志器
	Logger::ptr get_root() const { return m_root; }
	std::string to_yaml_string();
private:
	/// @brief 日志器容器
	std::map<std::string, Logger::ptr> m_loggers;
	/// @brief 主日志器
	Logger::ptr m_root;
	/// @brief 互斥量
	MutexType m_mutex;
};

/// @brief 日志器管理类单例模式
typedef ygj_server::Singleton<LoggerManager> LoggerMgr;
}


#endif // __SYLAR_LOG_H__
