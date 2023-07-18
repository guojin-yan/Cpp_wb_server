/// @file log.h
/// @brief ��־ģ���װ
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

/// @brief ʹ����ʽ��ʽ����־����level����־д�뵽logger
#define YGJ_LOG_LEVEL(logger, level)\
	if(logger->get_level() <= level)\
		ygj_server::LogEventWrap(ygj_server::LogEvent::ptr( new ygj_server::LogEvent(logger, level,\
		__FILE__, __LINE__, 0, ygj_server::get_thread_id(), ygj_server::get_fiber_id(),\
		time(0)))).get_ss()
/// @brief ʹ����ʽ��ʽ����־����debug����־д�뵽logger
#define YGJ_LOG_DEBUG(logger) YGJ_LOG_LEVEL(logger, ygj_server::LogLevel::DEBUG)
/// @brief ʹ����ʽ��ʽ����־����INFO����־д�뵽logger
#define YGJ_LOG_INFO(logger) YGJ_LOG_LEVEL(logger, ygj_server::LogLevel::INFO)
/// @brief ʹ����ʽ��ʽ����־����WARN����־д�뵽logger
#define YGJ_LOG_WARN(logger) YGJ_LOG_LEVEL(logger, ygj_server::LogLevel::WARN)
/// @brief ʹ����ʽ��ʽ����־����ERROR����־д�뵽logger
#define YGJ_LOG_ERROR(logger) YGJ_LOG_LEVEL(logger, ygj_server::LogLevel::ERROR)
/// @brief ʹ����ʽ��ʽ����־����FATAL����־д�뵽logger
#define YGJ_LOG_FATAL(logger) YGJ_LOG_LEVEL(logger, ygj_server::LogLevel::FATAL)

/// @brief ʹ�ø�ʽ����ʽ����־����level����־д�뵽logger
#define YGJ_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->get_level() <= level) \
        ygj_server::LogEventWrap(ygj_server::LogEvent::ptr(new ygj_server::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, ygj_server::get_thread_id(),\
                ygj_server::get_fiber_id(), time(0)))).get_event()->format(fmt, __VA_ARGS__)
/// @brief ʹ�ø�ʽ����ʽ����־����debug����־д�뵽logger
#define YGJ_LOG_FMT_DEBUG(logger, fmt, ...) YGJ_LOG_FMT_LEVEL(logger, ygj_server::LogLevel::DEBUG, fmt, __VA_ARGS__)
/// @brief ʹ�ø�ʽ����ʽ����־����INFO����־д�뵽logger
#define YGJ_LOG_FMT_INFO(logger, fmt, ...)  YGJ_LOG_FMT_LEVEL(logger, ygj_server::LogLevel::INFO, fmt, __VA_ARGS__)
/// @brief ʹ�ø�ʽ����ʽ����־����WARN����־д�뵽logger
#define YGJ_LOG_FMT_WARN(logger, fmt, ...)  YGJ_LOG_FMT_LEVEL(logger, ygj_server::LogLevel::WARN, fmt, __VA_ARGS__)
/// @brief ʹ�ø�ʽ����ʽ����־����ERROR����־д�뵽logger
#define YGJ_LOG_FMT_ERROR(logger, fmt, ...) YGJ_LOG_FMT_LEVEL(logger, ygj_server::LogLevel::ERROR, fmt, __VA_ARGS__)
/// @brief ʹ�ø�ʽ����ʽ����־����FATAL����־д�뵽logger
#define YGJ_LOG_FMT_FATAL(logger, fmt, ...) YGJ_LOG_FMT_LEVEL(logger, ygj_server::LogLevel::FATAL, fmt, __VA_ARGS__)

/// @brief ��ȡ����־��
#define YGJ_LOG_ROOT() ygj_server::LoggerMgr::get_instance()->get_root()

/// @brief ��ȡname����־��
#define YGJ_LOG_NAME(name) ygj_server::LoggerMgr::get_instance()->get_logger(name)

namespace ygj_server {
class Logger;
class LogEvent;
class LogEventWrap;
class LoggerManager;

/// @brief ��־����
class LogLevel {
public:
	/// @brief ��־����ö��
	enum Level { // ��־����
		UNKNOW = 0,
		DEBUG = 1,
		INFO = 2,
		WARN = 3,
		ERROR = 4,
		FATAL = 5
	};
	/// @brief ����־����ת���ı����
	/// @param[in] level ��־����
	/// @return ��־�����ı�
	static const char* to_string(LogLevel::Level level);
	static LogLevel::Level from_string(const std::string& str);
};

/// @brief ��־�¼�
class LogEvent {
public:
	typedef std::shared_ptr<LogEvent> ptr;
	/// @brief ���캯��
	/// @param[in] logger ��־��
	/// @param[in] level ��־����
	/// @param[in] file �ļ���
	/// @param[in] line �ļ��к�
	/// @param[in] elapse �������������ĺ�ʱ(����)
	/// @param[in] thread_id �߳�id
	/// @param[in] fiber_id Э��id
	/// @param[in] time ��־�¼�(��)
	LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, 
		uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time);
	/// @brief ��ȡ�ļ���
	/// @return �ļ���
	const char* get_file_name() const { return m_file; }
	/// @brief ��ȡ�к�
	/// @return �к�
	int32_t get_line() { return m_line; }
	/// @brief ��ȡ��ʱ
	/// @return ��ʱ
	uint32_t get_elapse() { return m_elapse; }
	/// @brief ��ȡ�߳�ID
	/// @return �߳�ID
	int32_t get_thread() { return m_thread; }
	/// @brief ��ȡЭ��ID
	/// @return Э��ID
	uint32_t get_fiberi() { return m_fiberid; }
	/// @brief ��ȡʱ��
	/// @return ʱ��
	uint64_t get_time() { return m_time; }
	/// @brief ��ȡ��־����
	/// @return ��־����
	std::string get_content() const { return m_ss.str(); }
	/// @brief ��ȡ��־��
	/// @return ��־��
	std::shared_ptr<Logger> get_logger() { return m_logger; }
	/// @brief ��ȡ��־����
	/// @return ��־����
	LogLevel::Level get_level() { return m_level; }
	/// @brief ��ȡ��־�����ַ�����
	/// @return ��־�����ַ�����
	std::stringstream& get_ss() { return m_ss; }
	/// @brief ��ʽ��д����־����
	/// @param fmt ��ʽ
	/// @param  ����
	void format(const char* fmt, ...);
	/// @brief 
	/// @param fmt ��ʽ
	/// @param al ��ʽ��д����־����
	void format(const char* fmt, va_list al);
private:
	/// @brief ��־��
	std::shared_ptr<Logger> m_logger;
	/// @brief ��־�ȼ�
	LogLevel::Level m_level;
	/// @brief �ļ�����
	const char* m_file = nullptr;
	/// @brief �к�
	int32_t m_line = 0;
	/// @brief ����������ʼ�����ڵĺ�����
	uint32_t m_elapse = 0;
	/// @brief �߳�ID
	int32_t m_thread = 0;
	/// @brief Э��ID
	uint32_t m_fiberid = 0; 
	/// @brief ʱ��
	uint64_t m_time;
	/// @brief ��־������
	std::stringstream m_ss; 

};

/// @brief ��־�¼���װ��
class LogEventWrap {
public:
	/// @brief ���캯��
	/// @param event ��־�¼�
	LogEventWrap(LogEvent::ptr event);
	/// @brief ��������
	~LogEventWrap();
	/// @brief ��ȡ��־�¼�
	/// @return ��־�¼�
	LogEvent::ptr get_event() const { return m_event; }
	/// @brief ��ȡ��־������
	/// @return ��־������
	std::stringstream& get_ss();
private:
	/// @brief ��־�¼�
	LogEvent::ptr m_event;
};



/// @brief ��־��ʽ��
class LogFormatter {
public:
	typedef std::shared_ptr<LogFormatter> ptr;
	/// @brief ���캯��
	/// @param[in] pattern ��ʽģ��
	/// @details
	///  %m ��Ϣ %p ��־���� %r �ۼƺ����� %c ��־���� %t �߳�id %n ���� %d ʱ��
	///  %f �ļ��� %l �к� %T �Ʊ�� %F Э��id %N �߳�����
	///  Ĭ�ϸ�ʽ "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
	LogFormatter(const std::string& pattern);
	/// @brief ��ȡ��ʽ����־�ı�
	/// @param logger ��־��
	/// @param level ��־����
	/// @param evevt ��־�¼�
	/// @return ��ʽ����־�ı�
	std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt);
	/// @brief ��ȡ��ʽ����־������
	/// @param ofs ��־�����
	/// @param logger ��־��
	/// @param level ��־����
	/// @param evevt ��־�¼�
	/// @return ��ʽ����־��
	std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
	/// @brief ��־�������ʽ��
	class FormatItem{
	public:
		typedef std::shared_ptr<FormatItem> ptr;
		virtual ~FormatItem(){}
		/// @brief ��ʽ����־����
		/// @param os ��־�����
		/// @param logger ��־��
		/// @param level ��־����
		/// @param evevt ��־�¼�
		virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) = 0;
	};
	/// @brief ��ʼ��,������־ģ��
	void init();
	bool is_error() const { return m_error; }
	const std::string get_pattern() const { return m_pattern; }
private:
	
	/// @brief ��־ģ��
	std::string m_pattern;
	/// @brief ��־��ʽ�������ʽ
	std::vector<FormatItem::ptr> m_items;
	bool m_error = false;
};

// @brief ��־��ʽ��
class LogAppender {
	friend  class Logger;
public:
	typedef std::shared_ptr<LogAppender> ptr;
	typedef Spinlock MutexType;
	virtual ~LogAppender() {}
	/// @brief д����־
	/// @param logger ��־��
	/// @param level ��־����
	/// @param evevt ��־�¼�
	virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, const LogEvent::ptr evevt) = 0;
	virtual  std::string to_yaml_string() = 0;
	/// @brief ������־��ʽ��
	/// @param val ��־ģ��
	void set_formatter(LogFormatter::ptr val);
	/// @brief ��ȡ��־��ʽ��
	/// @return ��־ģ��
	LogFormatter::ptr get_formatter();
	/// @brief ��ȡ��־�ȼ�
	/// @return ��־�ȼ�
	LogLevel::Level get_level() const { return m_level; }
	/// @brief ������־�ȼ�
	/// @param level ��־�ȼ�
	void set_level(LogLevel::Level level) { m_level = level; }
protected:
	/// @brief ��־����
	LogLevel::Level m_level = LogLevel::DEBUG; // ��־����
	/// @brief ��־��ʽ��
	LogFormatter::ptr m_formatter; // �����ʽ
	/// @brief 
	bool m_has_formatter = false;
	/// @brief ������
	MutexType m_mutex;
};



// @brief ��־
class Logger : public std::enable_shared_from_this<Logger> {
	friend class LoggerManager;
public:
	typedef std::shared_ptr<Logger> ptr;
	typedef Spinlock MutexType;
	/// @brief ���캯��
	/// @param name ��־������
	Logger(const std::string name = "root");
	/// @brief д����־
	/// @param level ��־����
	/// @param event ��־�¼�
	void log(LogLevel::Level level, const LogEvent::ptr event);
	/// @brief дdebug������־
	/// @param[in] event ��־�¼�
	void debug(LogEvent::ptr event);
	/// @brief дinfo������־
	/// @param[in] event ��־�¼�
	void info(LogEvent::ptr eventt);
	/// @brief дwarn������־
	/// @param[in] event ��־�¼�
	void warn(LogEvent::ptr event);
	/// @brief дerror������־
	/// @param[in] event ��־�¼�
	void error(LogEvent::ptr event);
	/// @brief дfatal������־
	/// @param[in] event ��־�¼�
	void fatal(LogEvent::ptr event);
	/// @brief �����־Ŀ��
	/// @param appender ��־Ŀ��
	void add_appender(LogAppender::ptr appender);
	/// @brief ɾ����־Ŀ��
	/// @param appender ��־Ŀ��
	void del_appender(LogAppender::ptr appender);
	/// @brief ��ȡ��־����
	/// @return ��־����
	LogLevel::Level get_level() const { return m_level; }
	/// @brief ���appender
	void clear_appender();
	/// @brief ������־����
	/// @param val ��־����
	void set_level(LogLevel::Level val) { m_level = val; }
	/// @brief ��ȡ��־��
	/// @return ��־��
	const std::string get_name() const { return m_name; }
	/// @brief ������־��ʽ
	/// @param val ��־��ʽ
	void set_formatter(LogFormatter::ptr val);
	/// @brief ������־��ʽ
	/// @param val ��־��ʽ
	void set_formatter(const std::string& val);
	/// @brief ��ȡ��־��ʽ
	/// @return ��־��ʽ
	LogFormatter::ptr get_formatter();
	std::string to_yaml_string();
private:
	/// @brief ��־��
	std::string m_name; // ��־��
	/// @brief ��־����
	LogLevel::Level m_level; // ��־����
	/// @brief ��־Ŀ��
	std::list<LogAppender::ptr> m_appenders; // Appender����
	/// @brief ��־��ʽ��
	LogFormatter::ptr m_formatter;

	Logger::ptr m_root;
	/// @brief ������
	MutexType m_mutex;
};

// @brief ���������̨��Appender
class StdoutLogAppender : public LogAppender{
public:
	typedef std::shared_ptr<StdoutLogAppender> ptr;
	/// @brief ����̨д����־
	/// @param logger ��־��
	/// @param level ��־����
	/// @param event ��־�¼�
	void log(std::shared_ptr<Logger> logger, LogLevel::Level level, const LogEvent::ptr event) override;
	std::string to_yaml_string() override;
};

// @brief ������ļ���Appender
class FileLogAppender : public LogAppender{
public:
	typedef std::shared_ptr<FileLogAppender> ptr;
	/// @brief ���캯��
	/// @param file_name ��־�� 
	FileLogAppender(const std::string file_name);
	/// @brief �ļ�д����־
	/// @param logger ��־��
	/// @param level ��־����
	/// @param event ��־�¼�
	void log(std::shared_ptr<Logger> logger, LogLevel::Level level, const LogEvent::ptr event) override;
	/// @brief ���´���־�ļ�
	/// @return �Ƿ�򿪳ɹ�
	bool reopen(); // ���´��ļ����ļ��򿪳ɹ�����true
	std::string to_yaml_string() override;
private:
	/// @brief �ļ���
	std::string m_file_name;
	/// @brief �ļ���
	std::ofstream m_file_stream;
	/// @brief �ϴ����´�ʱ��
	uint64_t m_last_time = 0;
};

/// @brief ��־��������
class LoggerManager {
public:
	typedef Spinlock MutexType;
	LoggerManager();
	/// @brief ��ȡ��־��
	/// @param name ��־����
	/// @return ��־��
	Logger::ptr get_logger(const std::string& name);
	/// @brief ��ʼ��
	void init();
	/// @brief ��ȡ����־��
	/// @return ����־��
	Logger::ptr get_root() const { return m_root; }
	std::string to_yaml_string();
private:
	/// @brief ��־������
	std::map<std::string, Logger::ptr> m_loggers;
	/// @brief ����־��
	Logger::ptr m_root;
	/// @brief ������
	MutexType m_mutex;
};

/// @brief ��־�������൥��ģʽ
typedef ygj_server::Singleton<LoggerManager> LoggerMgr;
}


#endif // __SYLAR_LOG_H__
