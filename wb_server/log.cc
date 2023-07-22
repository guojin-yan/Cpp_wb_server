#include "log.h"

#include <functional>
#include <iostream>
#include <fstream>
#include <map>
#include <cstdarg>
#include <tuple>

#include <time.h>
#include <stdarg.h>
#include <stdio.h>



#include "config.h"

#include "util.h"


namespace ygj_server {
const char* LogLevel::to_string(LogLevel::Level level) {
	switch (level) {
#define XX(name) \
	case LogLevel::name: \
		return #name; \
		break;

		XX(DEBUG);
		XX(INFO);
		XX(WARN);
		XX(ERROR);
		XX(FATAL);
#undef XX
	default:
		return "UNKNOW";
	}
	return "UNKNOW";
}

LogLevel::Level LogLevel::from_string(const std::string& str) {
#define XX(level, v) \
    if(str == #v) { \
        return LogLevel::level; \
    }
	XX(DEBUG, debug);
	XX(INFO, info);
	XX(WARN, warn);
	XX(ERROR, error);
	XX(FATAL, fatal);

	XX(DEBUG, DEBUG);
	XX(INFO, INFO);
	XX(WARN, WARN);
	XX(ERROR, ERROR);
	XX(FATAL, FATAL);
	return LogLevel::UNKNOW;
#undef XX
}

class MessageFormatItem : public LogFormatter::FormatItem {
public:
	MessageFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) override {
		os << evevt->get_content();
	}
};

class LevelFormatItem : public LogFormatter::FormatItem {
public:
	LevelFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) override {
		os << LogLevel::to_string(level);
	}
};
class ElapseFormatItem : public LogFormatter::FormatItem {
public:
	ElapseFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) override {
		os << evevt->get_elapse();
	}
};

class NameFormatItem : public LogFormatter::FormatItem {
public:
	NameFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) override {
		os << evevt->get_logger()->get_name();
	}
};

class ThreadIdFormatItem : public LogFormatter::FormatItem {
public:
	ThreadIdFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) override {
		os << evevt->get_thread();
	}
};

class FiberIdFormatItem : public LogFormatter::FormatItem {
public:
	FiberIdFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) override {
		os << evevt->get_fiberi();
	}
};

class ThreadNameFormatItem : public LogFormatter::FormatItem {
public:
	ThreadNameFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) override {
		os << evevt->get_thread_name();
	}
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
	DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S") :m_format(format) {
		if (m_format.empty()) {
			m_format = "%Y-%m-%d %H:%M:%S";
		}
	}
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) override {
		struct tm tm;
		time_t time = evevt->get_time();
#ifdef _WIN32
		localtime_s(&tm, &time);
#else
		localtime_r(&time, &tm);
#endif
		char buf[64];
		strftime(buf, sizeof(buf), m_format.c_str(), &tm);
		os << buf;
	}
private:
	std::string m_format;
};

class FilenameFormatItem : public LogFormatter::FormatItem {
public:
	FilenameFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) override {
		os << evevt->get_file_name();
	}
};

class LineFormatItem : public LogFormatter::FormatItem {
public:
	LineFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) override {
		os << evevt->get_line();
	}
};
class NewLineFormatItem : public LogFormatter::FormatItem {
public:
	NewLineFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) override {
		os << std::endl;
	}
};
class StringFormatItem : public LogFormatter::FormatItem {
public:
	StringFormatItem(const std::string str) : m_string(str){}
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) override {
		os << m_string;
	}
private:
	std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem {
public:
	TabFormatItem(const std::string str) : m_string(str) {}
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) override {
		os << "\t";
	}
private:
	std::string m_string;
};

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse, 
	uint32_t thread_id, uint32_t fiber_id, uint64_t time, const std::string& thread_name)
	:m_logger(logger),
	m_level(level),
	m_file(file),
	m_line(line),
	m_elapse(elapse),
	m_thread(thread_id),
	m_fiberid(fiber_id),
	m_time(time) ,
	m_thread_name(thread_name){
	
}

void LogEvent::format(const char* fmt, ...) {
	va_list al;
	va_start(al, fmt);
	format(fmt, al);
	va_end(al);
}

void LogEvent::format(const char* fmt, va_list al) {
	char buf[64];
	int len = vsprintf(buf, fmt, al);
	if (len != -1) {
		m_ss << std::string(buf, len);
		//free(buf);
	}
	
}

LogEventWrap::LogEventWrap(LogEvent::ptr event) : m_event(event){
}
LogEventWrap::~LogEventWrap() {
	m_event->get_logger()->log(m_event->get_level(), m_event);
}
std::stringstream& LogEventWrap::get_ss() {
	return m_event->get_ss();
}


void LogAppender::set_formatter(LogFormatter::ptr val) {
	MutexType::Lock lock(m_mutex);
	m_formatter = val;
	if(m_formatter) {
		m_has_formatter = true;
	} else {
		m_has_formatter = false;
	}
}

LogFormatter::ptr LogAppender::get_formatter() {
	MutexType::Lock lock(m_mutex);
	return m_formatter;
}

Logger::Logger(const std::string name ): m_name(name),
m_level(LogLevel::DEBUG){
	m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T<%f:%l>%T%m %n"));

}

void Logger::add_appender(LogAppender::ptr appender) {
	MutexType::Lock lock(m_mutex);
	if (!appender->get_formatter()) {
		MutexType::Lock ll(appender-> m_mutex);
		appender->m_formatter = m_formatter;
	}
	m_appenders.push_back(appender);
}
void Logger::del_appender(LogAppender::ptr appender) {
	MutexType::Lock lock(m_mutex);
	for (auto it = m_appenders.begin(); it != m_appenders.end(); it++)
	{
		if (*it == appender)
		{
			m_appenders.erase(it);
			break;
		}
	}
}
void Logger::clear_appender() {
	MutexType::Lock lock(m_mutex);
	m_appenders.clear();
}
void Logger::log(LogLevel::Level level, const LogEvent::ptr evevt) {
	if(level>=m_level)
	{
		auto self = shared_from_this();
		MutexType::Lock lock(m_mutex);
		if(!m_appenders.empty()) {
			for (auto& i : m_appenders)
			{
				i->log(self, level, evevt);
			}
		} else if (m_root){
			m_root->log(level, evevt);
		}

	}
}

void Logger::debug(LogEvent::ptr evevt) {
	log(LogLevel::DEBUG, evevt);
}
void Logger::info(LogEvent::ptr evevt) {
	log(LogLevel::INFO, evevt);
}
void Logger::warn(LogEvent::ptr evevt) {
	log(LogLevel::WARN, evevt);
}
void Logger::error(LogEvent::ptr evevt) {
	log(LogLevel::ERROR, evevt);
}
void Logger::fatal(LogEvent::ptr evevt) {
	log(LogLevel::FATAL, evevt);
}

void Logger::set_formatter(LogFormatter::ptr val) {
	MutexType::Lock lock(m_mutex);
	m_formatter = val;
	for(auto& i:m_appenders) {
		MutexType::Lock ll(i->m_mutex);
		if(!i->m_has_formatter) {
			i->m_formatter = m_formatter;
		}
	}
}

void Logger::set_formatter(const std::string& val) {
	
	ygj_server::LogFormatter::ptr new_val(new ygj_server::LogFormatter(val));
	if (new_val->is_error()) {
		std::cout << "Logger setFormatter name=" << m_name
			<< " value=" << val << " invalid formatter"
			<< std::endl;
		return;
	}
	set_formatter(new_val);
}

LogFormatter::ptr Logger::get_formatter() {
	MutexType::Lock lock(m_mutex);
	return m_formatter;
}

std::string Logger::to_yaml_string() {
	MutexType::Lock lock(m_mutex);
	YAML::Node node;
	node["name"] = m_name;
	if (m_level != LogLevel::UNKNOW) {
		node["level"] = LogLevel::to_string(m_level);
	}
	if (m_formatter) {
		node["formatter"] = m_formatter->get_pattern();
	}

	for (auto& i : m_appenders) {
		node["appenders"].push_back(YAML::Load(i->to_yaml_string()));
	}
	std::stringstream ss;
	ss << node;
	return ss.str();
}

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, const LogEvent::ptr evevt) {
	if (level >= m_level) {
		MutexType::Lock lock(m_mutex);
		std::cout << m_formatter->format(logger, level, evevt);
	}
}

std::string StdoutLogAppender::to_yaml_string() {
	MutexType::Lock lock(m_mutex);
	YAML::Node node;
	node["type"] = "StdoutLogAppender";
	if (m_level != LogLevel::UNKNOW) {
		node["level"] = LogLevel::to_string(m_level);
	}
	if (m_has_formatter && m_formatter) {
		node["formatter"] = m_formatter->get_pattern();
	}
	std::stringstream ss;
	ss << node;
	return ss.str();
}

FileLogAppender::FileLogAppender(const std::string file_name) : m_file_name(file_name) {
	reopen();
}
void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, const LogEvent::ptr event) {
	//if (level >= m_level) {
	//	m_file_stream << m_formatter->format(logger, level, evevt);
	//}
	
	if (level >= m_level) {
		uint64_t now = time(0);
		if(now != m_last_time ) {
			reopen();
			m_last_time = now;
		}
		
		MutexType::Lock lock(m_mutex);
		if (!m_formatter->format(m_file_stream, logger, level, event)) {
			std::cout << "error" << std::endl;
		}
	}
}
bool FileLogAppender::reopen() {
	MutexType::Lock lock(m_mutex);
	if (m_file_stream) {
		m_file_stream.close();
	}
	m_file_stream.open(m_file_name.c_str(), std::ios::app);
	return m_file_stream.is_open();

}

std::string FileLogAppender::to_yaml_string() {
	MutexType::Lock lock(m_mutex);
	YAML::Node node;
	node["type"] = "FileLogAppender";
	node["file"] = m_file_name;
	if (m_level != LogLevel::UNKNOW) {
		node["level"] = LogLevel::to_string(m_level);
	}
	if (m_has_formatter && m_formatter) {
		node["formatter"] = m_formatter->get_pattern();
	}
	std::stringstream ss;
	ss << node;
	return ss.str();
}

LogFormatter::LogFormatter(const std::string& pattern) :m_pattern(pattern) {
	init();
}
std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr evevt) {
	std::stringstream ss;
	for(auto&i :m_items) {
		i->format(ss, logger, level, evevt);
	}
	return ss.str();
}
std::ostream& LogFormatter::format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
	for (auto& i : m_items) {
		i->format(ofs, logger, level, event);
	}
	return ofs;
}
void LogFormatter::init() {
	// str, format, type
	std::vector<std::tuple<std::string, std::string, int>> vec;
	std::string nstr;
	for (size_t i = 0; i < m_pattern.size(); ++i) {
		if (m_pattern[i] != '%') {
			nstr.append(1, m_pattern[i]);
			continue;
		}
		if ((i + 1) < m_pattern.size()) {
			if (m_pattern[i + 1] == '%') {
				nstr.append(1, '%');
				continue;
			}
		}
		size_t n = i + 1;
		int fmt_status = 0;
		size_t fmt_begin = 0;

		std::string str;
		std::string fmt;
		while ( n < m_pattern.size()) {
			if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
				&& m_pattern[n] != '}')) {
				str = m_pattern.substr(i + 1, n - i - 1);
				break;
			}
			if (fmt_status == 0) {
				if (m_pattern[n] == '{') {
					str = m_pattern.substr(i + 1, n - i - 1);
					fmt_status = 1; // 解析格式
					fmt_begin = n;
					++n;
					continue;
				}
			}
			if (fmt_status == 1) {
				if (m_pattern[n] == '}') {
					fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
					fmt_status = 2;
					++n;
					break;
				}
			}
			++n;

		}
		if (fmt_status == 0) {
			if (!nstr.empty()) {
				vec.push_back(std::make_tuple(nstr, std::string(), 0));
				nstr.clear();
			}
			str = m_pattern.substr(i + 1, n - i - 1);
			vec.push_back(std::make_tuple(str, fmt, 1));
			i = n - 1;
		} else if (fmt_status == 1) {
			std::cout << "pattern parseerror: " << m_pattern << " - " << m_pattern.substr(i)
				<< std::endl;
			m_error = true;
			vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
		} else if (fmt_status == 2) {
			if(nstr.empty()) {
				vec.push_back(std::make_tuple(nstr, std::string(), 0));
				nstr.clear();
			}
			vec.push_back(std::make_tuple(str, fmt, 1));
			i = n - 1;
		}
	}
	if (!nstr.empty()) {
		vec.push_back(std::make_tuple(nstr, "", 0));
	}

	static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

		XX(m, MessageFormatItem),           //m:消息
		XX(p, LevelFormatItem),             //p:日志级别
		XX(r, ElapseFormatItem),            //r:累计毫秒数
		XX(c, NameFormatItem),              //c:日志名称
		XX(t, ThreadIdFormatItem),          //t:线程id
		XX(n, NewLineFormatItem),           //n:换行
		XX(d, DateTimeFormatItem),          //d:时间
		XX(f, FilenameFormatItem),          //f:文件名
		XX(l, LineFormatItem),              //l:行号
		XX(T, TabFormatItem),               //T:Tab
		XX(F, FiberIdFormatItem),           //F:协程id
		XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
	};
	for(auto& i : vec) {
		if(std::get<2>(i) == 0) {
			m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
		} else {
			auto it = s_format_items.find(std::get<0>(i));
			if (it == s_format_items.end()) {
				m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" +
					std::get<0>(i) + ">>")));
				m_error = true;
			} else {
				m_items.push_back(it->second(std::get<1>(i)));
			}
		}
		// std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
	}
	// std::cout << m_items.size() << std::endl;
}

LoggerManager::LoggerManager() {
	m_root.reset(new Logger);

	m_root->add_appender(LogAppender::ptr(new StdoutLogAppender));
	m_loggers[m_root->m_name] = m_root;
	init();
}
Logger::ptr LoggerManager::get_logger(const std::string& name) {
	MutexType::Lock lock(m_mutex);
	auto it = m_loggers.find(name);
	if (it != m_loggers.end()) {
		return it->second;
	}

	Logger::ptr logger(new Logger(name)); 
	logger->m_root = m_root;
	m_loggers[name] = logger;
	return logger;
}

struct LogAppenderDefine {
	int type = 0; //1 File, 2 Stdout
	LogLevel::Level level = LogLevel::UNKNOW;
	std::string formatter;
	std::string file;

	bool operator==(const LogAppenderDefine& oth) const {
		return type == oth.type
			&& level == oth.level
			&& formatter == oth.formatter
			&& file == oth.file;
	}
};

struct LogDefine {
	std::string name;
	LogLevel::Level level = LogLevel::UNKNOW;
	std::string formatter;
	std::vector<LogAppenderDefine> appenders;

	bool operator==(const LogDefine& oth) const {
		return name == oth.name
			&& level == oth.level
			&& formatter == oth.formatter
			&& appenders == appenders;
	}

	bool operator<(const LogDefine& oth) const {
		return name < oth.name;
	}

	bool is_valid() const {
		return !name.empty();
	}
};


template<>
class LexicalCast<std::string, std::set<LogDefine>> {
public:
	std::set<LogDefine> operator()(const std::string& v) {
		YAML::Node node = YAML::Load(v);
		std::set<LogDefine> vec;
		for(size_t i = 0; i < node.size(); ++i) {
			YAML::Node  n = node[i];
			
			if (!n["name"].IsDefined()) {
				std::cout << "log config error: name is null, " << n
					<< std::endl;
				continue;
			}
			LogDefine ld;
			ld.name = n["name"].as<std::string>();
			ld.level = LogLevel::from_string(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
			if (n["formatter"].IsDefined()) {
				ld.formatter = n["formatter"].as<std::string>();
			}

			if (n["appenders"].IsDefined()) {
				//std::cout << "==" << ld.name << " = " << n["appenders"].size() << std::endl;
				for (size_t x = 0; x < n["appenders"].size(); ++x) {
					YAML::Node a = n["appenders"][x];
					if (!a["type"].IsDefined()) {
						std::cout << "log config error: appender type is null, " << a
							<< std::endl;
						continue;
					}
					std::string type = a["type"].as<std::string>();
					LogAppenderDefine lad;
					if (type == "FileLogAppender") {
						lad.type = 1;
						if (!a["file"].IsDefined()) {
							std::cout << "log config error: fileappender file is null, " << a
								<< std::endl;
							continue;
						}
						lad.file = a["file"].as<std::string>();
						if (a["formatter"].IsDefined()) {
							lad.formatter = a["formatter"].as<std::string>();
						}
					}
					else if (type == "StdoutLogAppender") {
						lad.type = 2;
					}
					else {
						std::cout << "log config error: appender type is invalid, " << a
							<< std::endl;
						continue;
					}

					ld.appenders.push_back(lad);
				}
			}
			vec.insert(ld);
		}
		
		return vec;
	}
};

template<>
class LexicalCast<std::set<LogDefine>, std::string> {
public:
	std::string operator()(const std::set<LogDefine>& v) {
		YAML::Node node;
		for(auto& i : v) {
			YAML::Node n;
			n["name"] = i.name;
			if (i.level != LogLevel::UNKNOW) {
				n["level"] = LogLevel::to_string(i.level);
			}
			if (!i.formatter.empty()) {
				n["formatter"] = i.formatter;
			}

			for (auto& a : i.appenders) {
				YAML::Node na;
				if (a.type == 1) {
					na["type"] = "FileLogAppender";
					na["file"] = a.file;
				}
				else if (a.type == 2) {
					na["type"] = "StdoutLogAppender";
				}
				if (a.level != LogLevel::UNKNOW) {
					na["level"] = LogLevel::to_string(a.level);
				}

				if (!a.formatter.empty()) {
					na["formatter"] = a.formatter;
				}

				n["appenders"].push_back(na);
			}
			node.push_back(n);
		}

		std::stringstream ss;
		ss << node;
		return ss.str();
	}
};


ygj_server::ConfigVar<std::set<LogDefine> >::ptr g_log_defines =
	ygj_server::Config::lookup("logs", std::set<LogDefine>(), "logs config");


struct LogIniter {
	LogIniter() {
		g_log_defines->add_listener([](const std::set<LogDefine>& old_value, 
				const std::set<LogDefine>& new_value) {

			YGJ_LOG_INFO(YGJ_LOG_ROOT()) << "on_logger_conf_changed";

			for (auto& i : new_value) {
				auto it = old_value.find(i);
				ygj_server::Logger::ptr logger;
				if (it == old_value.end()) {
					//新增logger
					logger = YGJ_LOG_NAME(i.name);
				}
				else {
					if (!(i == *it)) {
						//修改的logger
						logger = YGJ_LOG_NAME(i.name);
	
					}
					else {
						continue;
					}
				}
				logger->set_level(i.level);
				if (!i.formatter.empty()) {
					logger->set_formatter(i.formatter);
				}
				logger->clear_appender();
				for (auto& a : i.appenders) {
					ygj_server::LogAppender::ptr ap;
					if (a.type == 1) {
						ap.reset(new FileLogAppender(a.file));
					}
					else if (a.type == 2) {
						ap.reset(new StdoutLogAppender);
					}
					ap->set_level(a.level);
					if(!a.formatter.empty()) {
						LogFormatter::ptr fmt(new LogFormatter(a.formatter));
						if(!fmt->is_error()) {
							ap->set_formatter(fmt);
						} else {
							std::cout << "log.nam=" << i.name <<  " appender type=" << a.type
							          << " formatter=" << a.formatter << "is invalid!" << std::endl;
						}
					}
					logger->add_appender(ap);
				}
			}

			for (auto& i : old_value) {
				auto it = new_value.find(i);
				if (it == new_value.end()) {
					//删除logger
					auto logger = YGJ_LOG_NAME(i.name);
					logger->set_level((LogLevel::Level)0);
					logger->clear_appender();
				}
			}
		});
	}
};

// 定义全局变量，保证在main函数前执行
static LogIniter __log_init;


std::string LoggerManager::to_yaml_string() {
	MutexType::Lock lock(m_mutex);
	YAML::Node node;
	for (auto& i : m_loggers) {
		node.push_back(YAML::Load(i.second->to_yaml_string()));
	}
	std::stringstream ss;
	ss << node;
	return ss.str();
}
void LoggerManager::init() {

}

}
