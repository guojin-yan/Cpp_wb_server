#include "log.h"

#include <functional>
#include <iostream>
#include <map>
#include <tuple>

#include <string.h>
#include <time.h>


namespace ygj_server {

const char* LogLevel::tostring(LogLevel::Level level) {
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

class MessageFormatItem : public LogFormatter::FormatItem {
public:
	MessageFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvevt::ptr evevt) override {
		os << evevt->get_content();
	}
};

class LevelFormatItem : public LogFormatter::FormatItem {
public:
	LevelFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvevt::ptr evevt) override {
		os << LogLevel::tostring(level);
	}
};
class ElapseFormatItem : public LogFormatter::FormatItem {
public:
	ElapseFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvevt::ptr evevt) override {
		os << evevt->get_elapse();
	}
};

class NameFormatItem : public LogFormatter::FormatItem {
public:
	NameFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvevt::ptr evevt) override {
		os << logger->get_name();
	}
};

class ThreadIdFormatItem : public LogFormatter::FormatItem {
public:
	ThreadIdFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvevt::ptr evevt) override {
		os << evevt->get_thread();
	}
};

class FiberIdFormatItem : public LogFormatter::FormatItem {
public:
	FiberIdFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvevt::ptr evevt) override {
		os << evevt->get_fiberi();
	}
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
	DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S") :m_format(format) {
		if (m_format.empty()) {
			m_format = "%Y-%m-%d %H:%M:%S";
		}
	}
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvevt::ptr evevt) override {
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
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvevt::ptr evevt) override {
		os << evevt->get_file_name();
	}
};

class LineFormatItem : public LogFormatter::FormatItem {
public:
	LineFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvevt::ptr evevt) override {
		os << evevt->get_line();
	}
};
class NewLineFormatItem : public LogFormatter::FormatItem {
public:
	NewLineFormatItem(const std::string& fmt = "") {};
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvevt::ptr evevt) override {
		os << std::endl;
	}
};
class StringFormatItem : public LogFormatter::FormatItem {
public:
	StringFormatItem(const std::string str) : m_string(str){}
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvevt::ptr evevt) override {
		os << m_string;
	}
private:
	std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem {
public:
	TabFormatItem(const std::string str) : m_string(str) {}
	void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvevt::ptr evevt) override {
		os << "\t";
	}
private:
	std::string m_string;
};

LogEvevt::LogEvevt(const char* file, int32_t line, uint32_t elapse, uint32_t thread_id,
	uint32_t fiber_id, uint64_t time)
	:m_file(file),
	m_line(line),
	m_elapse(elapse),
	m_thread(thread_id),
	m_fiberid(fiber_id),
	m_time(time) {
	
}

Logger::Logger(const std::string name ): m_name(name),
m_level(LogLevel::DEBUG){
	m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T<%f:%l>%T%m %n"));
}

void Logger::add_appender(LogAppender::ptr appender) {
	if (!appender->get_format()) {
		appender->set_format(m_formatter);
	}
	m_appenders.push_back(appender);
}
void Logger::del_appender(LogAppender::ptr appender) {
	for (auto it = m_appenders.begin(); it != m_appenders.end(); it++)
	{
		if (*it == appender)
		{
			m_appenders.erase(it);
			break;
		}
	}
}
void Logger::log(LogLevel::Level level, const LogEvevt::ptr evevt) {
	if(level>=m_level)
	{
		auto self = shared_from_this();
		for(auto & i : m_appenders)
		{
			i->log(self, level, evevt);
		}
	}
}

void Logger::debug(LogEvevt::ptr evevt) {
	log(LogLevel::DEBUG, evevt);
}
void Logger::info(LogEvevt::ptr evevt) {
	log(LogLevel::INFO, evevt);
}
void Logger::warn(LogEvevt::ptr evevt) {
	log(LogLevel::WARN, evevt);
}
void Logger::error(LogEvevt::ptr evevt) {
	log(LogLevel::ERROR, evevt);
}
void Logger::fatal(LogEvevt::ptr evevt) {
	log(LogLevel::FATAL, evevt);
}


void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, const LogEvevt::ptr evevt) {
	if (level >= m_level) {
		std::cout << m_formatter->format(logger, level, evevt);
	}
}
FileLogAppender::FileLogAppender(const std::string file_name) : m_file_name(file_name) {
}
void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, const LogEvevt::ptr evevt) {
	if (level >= m_level) {
		m_file_stream << m_formatter->format(logger, level, evevt);
	}
}
bool FileLogAppender::reopen() {
	if (m_file_stream) {
		m_file_stream.close();
	}
	
	m_file_stream.open(m_file_name);
	return !!m_file_stream;
}

LogFormatter::LogFormatter(const std::string& pattern) :m_pattern(pattern) {
	init();
}
std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvevt::ptr evevt) {
	std::stringstream ss;
	for(auto&i :m_items) {
		i->format(ss, logger, level, evevt);
	}
	return ss.str();
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
		//XX(N, ThreadNameFormatItem),        //N:线程名称
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
			} else {
				m_items.push_back(it->second(std::get<1>(i)));
			}
		}
		std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
	}
	std::cout << m_items.size() << std::endl;
}



}
