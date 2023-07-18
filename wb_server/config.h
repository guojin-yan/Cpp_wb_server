#ifndef _YGJ_CONFIG_H_
#define _YGJ_CONFIG_H_

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <boost/lexical_cast.hpp>
#include "yaml-cpp/yaml.h"

#include "log.h"

namespace ygj_server {
class ConfigVarBase {
public:
	typedef std::shared_ptr<ConfigVarBase> ptr;
	ConfigVarBase(const std::string name, const std::string description)
		: m_name(name), m_description(description) {
		std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
	}
	virtual ~ConfigVarBase() {}

	const std::string& get_name() { return m_name; }
	const std::string& get_description() { return m_description; }

	virtual std::string to_string() = 0;
	virtual bool from_string(const std::string& val) = 0;
	virtual  std::string get_type_name() const = 0;
protected:
	std::string m_name;
	std::string m_description;
};

/// @brief 将两种数据格式进行转换
/// @tparam F 原数据
/// @tparam T 目标数据
template<class F, class T>
class LexicalCast {
public:
	T operator () (const F& v) {
		return boost::lexical_cast<T>(v);
	}
};

/// @brief 将string数据转为vector数据
/// @tparam T vector数据格式
template<class T>
class LexicalCast<std::string, std::vector<T>> {
public:
	std::vector<T> operator () (const std::string& v) {
		YAML::Node node = YAML::Load(v);
		typename std::vector<T> vec;
		std::stringstream ss;
		for(size_t i = 0; i < node.size(); ++i) {
			ss.str("");
			ss << node[i];
			vec.push_back(LexicalCast<std::string, T>()(ss.str()));
		}
		return vec;
	}
};

/// @brief 将vector数据转为string数据
/// @tparam T vector数据格式
template<class T>
class LexicalCast<std::vector<T>, std::string> {
public:
	std::string operator () (const std::vector<T>& v) {
		YAML::Node node ;
		for(auto& i:v) {
			node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
		}

		std::stringstream ss;
		ss << node;
		return ss.str();
	}
};

/// @brief 将string数据转为list数据
/// @tparam T list数据格式
template<class T>
class LexicalCast<std::string, std::list<T>> {
public:
	std::list<T> operator () (const std::string& v) {
		YAML::Node node = YAML::Load(v);
		typename std::list<T> vec;
		std::stringstream ss;
		for (size_t i = 0; i < node.size(); ++i) {
			ss.str("");
			ss << node[i];
			vec.push_back(LexicalCast<std::string, T>()(ss.str()));
		}
		return vec;
	}
};

/// @brief 将list数据转为string数据
/// @tparam T list数据格式
template<class T>
class LexicalCast<std::list<T>, std::string> {
public:
	std::string operator () (const std::list<T>& v) {
		YAML::Node node;
		for (auto& i : v) {
			node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
		}

		std::stringstream ss;
		ss << node;
		return ss.str();
	}
};

/// @brief 将string数据转为set数据
/// @tparam T set数据格式
template<class T>
class LexicalCast<std::string, std::set<T>> {
public:
	std::set<T> operator () (const std::string& v) {
		YAML::Node node = YAML::Load(v);
		typename std::set<T> vec;
		std::stringstream ss;
		for (size_t i = 0; i < node.size(); ++i) {
			ss.str("");
			ss << node[i];
			vec.insert(LexicalCast<std::string, T>()(ss.str()));
		}
		return vec;
	}
};

/// @brief 将set数据转为string数据
/// @tparam T set数据格式
template<class T>
class LexicalCast<std::set<T>, std::string> {
public:
	std::string operator () (const std::set<T>& v) {
		YAML::Node node;
		for (auto& i : v) {
			node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
		}

		std::stringstream ss;
		ss << node;
		return ss.str();
	}
};

/// @brief 将string数据转为unordered_set数据
/// @tparam T unordered_set数据格式
template<class T>
class LexicalCast<std::string, std::unordered_set<T>> {
public:
	std::unordered_set<T> operator () (const std::string& v) {
		YAML::Node node = YAML::Load(v);
		typename std::unordered_set<T> vec;
		std::stringstream ss;
		for (size_t i = 0; i < node.size(); ++i) {
			ss.str("");
			ss << node[i];
			vec.insert(LexicalCast<std::string, T>()(ss.str()));
		}
		return vec;
	}
};

/// @brief 将unordered_set数据转为string数据
/// @tparam T unordered_set数据格式
template<class T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
	std::string operator () (const std::unordered_set<T>& v) {
		YAML::Node node;
		for (auto& i : v) {
			node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
		}

		std::stringstream ss;
		ss << node;
		return ss.str();
	}
};


/// @brief 将string数据转为map数据
/// @tparam T map数据格式
template<class T>
class LexicalCast<std::string, std::map<std::string, T>> {
public:
	std::map<std::string, T> operator () (const std::string& v) {
		YAML::Node node = YAML::Load(v);
		typename std::map<std::string, T> vec;
		std::stringstream ss;
		for (auto it = node.begin(); it != node.end(); ++it) {
			ss.str("");
			ss << it->second;
			vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
		}
		return vec;
	}
};

/// @brief 将map数据转为string数据
/// @tparam T map数据格式
template<class T>
class LexicalCast<std::map<std::string, T>, std::string> {
public:
	std::string operator () (const std::map<std::string, T>& v) {
		YAML::Node node;
		for (auto& i : v) {
			node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
		}

		std::stringstream ss;
		ss << node;
		return ss.str();
	}
};


/// @brief 将string数据转为unordered_map数据
/// @tparam T unordered_map数据格式
template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T>> {
public:
	std::unordered_map<std::string, T> operator () (const std::string& v) {
		YAML::Node node = YAML::Load(v);
		typename std::unordered_map<std::string, T> vec;
		std::stringstream ss;
		for (auto it = node.begin(); it != node.end(); ++it) {
			ss.str("");
			ss << it->second;
			vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
		}
		return vec;
	}
};

/// @brief 将unordered_map数据转为string数据
/// @tparam T unordered_map数据格式
template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
public:
	std::string operator () (const std::unordered_map<std::string, T>& v) {
		YAML::Node node;
		for (auto& i : v) {
			node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
		}

		std::stringstream ss;
		ss << node;
		return ss.str();
	}
};


/// @brief 配置参数模板子类, 保存对应类型的参数值
/// @details T 参数的具体类型
/// FromStr 从std::string转换成T类型的仿函数
/// ToStr 从T转换成std::string的仿函数
/// std::string 为YAML格式的字符串
template<class T,
		 class FromStr = LexicalCast<std::string, T>,
		 class ToStr = LexicalCast<T, std::string> >
class ConfigVar : public ConfigVarBase {
public:
	typedef std::shared_ptr<ConfigVar> ptr;

	typedef std::function<void(const T& old_value, const T& new_value)> on_change_cb;
	/// @brief 通过参数名, 参数值, 描述构造ConfigVar
	/// @param[in] name 参数名称有效字符为[0 - 9a - z_.]
	/// @param[in] default_value 参数的默认值
	/// @param[in] description 参数的描述
	ConfigVar(const std::string name, 
		const T& default_value, 
		const std::string description)
		: ConfigVarBase(name, description), m_val(default_value) {
	}
	/// @brief 将参数值转换成YAML String
	/// @exception 当转换失败抛出异常
	std::string to_string() override {
		try {
			//return boost::lexical_cast<std::string>(m_val);
			return ToStr()(m_val);
		} catch (std::exception& e) {
			YGJ_LOG_ERROR(YGJ_LOG_ROOT()) << "ConfigVar to string exception"
			<< e.what() << " convert:" << typeid(m_val).name() << " to string;";
		}
		return "";
	}
	/// @brief 从YAML String 转成参数的值
	/// @exception 当转换失败抛出异常
	bool from_string(const std::string& val) override {
		try {
			//m_val = boost::lexical_cast<T>(val);
			set_value(FromStr()(val));
		}
		catch (std::exception& e) {
			YGJ_LOG_ERROR(YGJ_LOG_ROOT()) << "ConfigVar to string exception"
				<< e.what() << " convert: string to" << typeid(m_val).name() << " - " << val;
		}
		return false;
	}
	/// @brief 获取当前参数的值
	/// @return 当前参数的值
	const T get_value() const { return m_val; }
	/// @brief 设置当前参数的值
	/// @param val 当前参数的值
	void set_value(const T& val) {
		if (val == m_val) {
			return;
		}
		for(auto&i:m_cbs) {
			i.second(m_val, val);
		}
		m_val = val;
	}
	std::string get_type_name() const override{ return typeid(T).name(); }

	void add_listener(uint64_t key, on_change_cb cb) {
		m_cbs[key] = cb;
	}
	void del_lietener(uint64_t key) {
		m_cbs.erase(key);
	}
	on_change_cb get_listener(uint64_t key) {
		auto it = m_cbs.find(key);
		return it == m_cbs.end() ? nullptr : it->second;
	}

	void clear_listener() {
		m_cbs.clear();
	}
private:
	T m_val;
	/// @brief 变更回调函数组
	///	@details uint64_t key,要求唯一，一般可以用hash
	std::map<uint64_t, on_change_cb> m_cbs;
};


/// @brief 配置类
class Config {
public:
	typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigValMap;
	/// @brief 获取/创建对应参数名的配置参数
	/// @param[in] name 配置参数名称
	/// @param[in] default_value 参数默认值
	/// @param[in] description 参数描述
	/// @details 获取参数名为name的配置参数,如果存在直接返回
	///          如果不存在,创建参数配置并用default_value赋值
	/// @return 返回对应的配置参数,如果参数名存在但是类型不匹配则返回nullptr
	/// @exception 如果参数名包含非法字符[^0-9a-z_.] 抛出异常 std::invalid_argument
	template<class T>
	static  typename ConfigVar<T>::ptr lookup(const std::string& name,
		const T& default_value, const std::string description) {
		auto it = get_datas().find(name);
		if(it != get_datas().end()) {
			auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
			if(tmp) {
				YGJ_LOG_INFO(YGJ_LOG_ROOT()) << "lookup name = " <<
					name << " exists;";
				return tmp;
			} else {
				YGJ_LOG_ERROR(YGJ_LOG_ROOT()) << "lookup name = " << name << "exists but type not "
					<< typeid(T).name() << "real_type=" << it->second->get_type_name() << " " << it->second->to_string();
				return nullptr;
			}
		}
		//auto tmp = lookup<T>(name);
		//if (tmp) {
		//	YGJ_LOG_INFO(YGJ_LOG_ROOT()) << "lookup name = " <<
		//		name << "exists;";
		//	return tmp;
		//}

		if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0.123456789") // 格式校验
				!= std::string::npos) {
			YGJ_LOG_ERROR(YGJ_LOG_ROOT()) << "lookup name invalid" << name;
			throw std::invalid_argument(name);
		}

		typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
		get_datas()[name] = v;
		return v;

	}
	/// @brief 查找配置参数
	/// @param[in] name 配置参数名称
	/// @return 返回配置参数名为name的配置参数
	template<class T>
	static typename ConfigVar<T>::ptr lookup(const std::string& name) {
		auto it = get_datas().find(name);
		if(it == get_datas().end()) {
			return nullptr;
		}
		return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
	}
	/// @brief 使用YAML::Node初始化配置模块
	/// @param root YAML::Node
	static void load_from_yaml(const YAML::Node& root);
	/// @brief 查找配置参数
	/// @param name 配置参数名
	/// @return 配置参数的基类
	static ConfigVarBase::ptr lookup_base(const std::string& name);
private:
	/// @brief 所有的配置项
	static ConfigValMap& get_datas() {
		static ConfigValMap s_datas;
		return s_datas;
	}

};

}
#endif  // _YGJ_CONFIG_H_
