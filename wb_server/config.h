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

/// @brief ���������ݸ�ʽ����ת��
/// @tparam F ԭ����
/// @tparam T Ŀ������
template<class F, class T>
class LexicalCast {
public:
	T operator () (const F& v) {
		return boost::lexical_cast<T>(v);
	}
};

/// @brief ��string����תΪvector����
/// @tparam T vector���ݸ�ʽ
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

/// @brief ��vector����תΪstring����
/// @tparam T vector���ݸ�ʽ
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

/// @brief ��string����תΪlist����
/// @tparam T list���ݸ�ʽ
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

/// @brief ��list����תΪstring����
/// @tparam T list���ݸ�ʽ
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

/// @brief ��string����תΪset����
/// @tparam T set���ݸ�ʽ
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

/// @brief ��set����תΪstring����
/// @tparam T set���ݸ�ʽ
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

/// @brief ��string����תΪunordered_set����
/// @tparam T unordered_set���ݸ�ʽ
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

/// @brief ��unordered_set����תΪstring����
/// @tparam T unordered_set���ݸ�ʽ
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


/// @brief ��string����תΪmap����
/// @tparam T map���ݸ�ʽ
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

/// @brief ��map����תΪstring����
/// @tparam T map���ݸ�ʽ
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


/// @brief ��string����תΪunordered_map����
/// @tparam T unordered_map���ݸ�ʽ
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

/// @brief ��unordered_map����תΪstring����
/// @tparam T unordered_map���ݸ�ʽ
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


/// @brief ���ò���ģ������, �����Ӧ���͵Ĳ���ֵ
/// @details T �����ľ�������
/// FromStr ��std::stringת����T���͵ķº���
/// ToStr ��Tת����std::string�ķº���
/// std::string ΪYAML��ʽ���ַ���
template<class T,
		 class FromStr = LexicalCast<std::string, T>,
		 class ToStr = LexicalCast<T, std::string> >
class ConfigVar : public ConfigVarBase {
public:
	typedef std::shared_ptr<ConfigVar> ptr;

	typedef std::function<void(const T& old_value, const T& new_value)> on_change_cb;
	/// @brief ͨ��������, ����ֵ, ��������ConfigVar
	/// @param[in] name ����������Ч�ַ�Ϊ[0 - 9a - z_.]
	/// @param[in] default_value ������Ĭ��ֵ
	/// @param[in] description ����������
	ConfigVar(const std::string name, 
		const T& default_value, 
		const std::string description)
		: ConfigVarBase(name, description), m_val(default_value) {
	}
	/// @brief ������ֵת����YAML String
	/// @exception ��ת��ʧ���׳��쳣
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
	/// @brief ��YAML String ת�ɲ�����ֵ
	/// @exception ��ת��ʧ���׳��쳣
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
	/// @brief ��ȡ��ǰ������ֵ
	/// @return ��ǰ������ֵ
	const T get_value() const { return m_val; }
	/// @brief ���õ�ǰ������ֵ
	/// @param val ��ǰ������ֵ
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
	/// @brief ����ص�������
	///	@details uint64_t key,Ҫ��Ψһ��һ�������hash
	std::map<uint64_t, on_change_cb> m_cbs;
};


/// @brief ������
class Config {
public:
	typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigValMap;
	/// @brief ��ȡ/������Ӧ�����������ò���
	/// @param[in] name ���ò�������
	/// @param[in] default_value ����Ĭ��ֵ
	/// @param[in] description ��������
	/// @details ��ȡ������Ϊname�����ò���,�������ֱ�ӷ���
	///          ���������,�����������ò���default_value��ֵ
	/// @return ���ض�Ӧ�����ò���,������������ڵ������Ͳ�ƥ���򷵻�nullptr
	/// @exception ��������������Ƿ��ַ�[^0-9a-z_.] �׳��쳣 std::invalid_argument
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

		if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0.123456789") // ��ʽУ��
				!= std::string::npos) {
			YGJ_LOG_ERROR(YGJ_LOG_ROOT()) << "lookup name invalid" << name;
			throw std::invalid_argument(name);
		}

		typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
		get_datas()[name] = v;
		return v;

	}
	/// @brief �������ò���
	/// @param[in] name ���ò�������
	/// @return �������ò�����Ϊname�����ò���
	template<class T>
	static typename ConfigVar<T>::ptr lookup(const std::string& name) {
		auto it = get_datas().find(name);
		if(it == get_datas().end()) {
			return nullptr;
		}
		return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
	}
	/// @brief ʹ��YAML::Node��ʼ������ģ��
	/// @param root YAML::Node
	static void load_from_yaml(const YAML::Node& root);
	/// @brief �������ò���
	/// @param name ���ò�����
	/// @return ���ò����Ļ���
	static ConfigVarBase::ptr lookup_base(const std::string& name);
private:
	/// @brief ���е�������
	static ConfigValMap& get_datas() {
		static ConfigValMap s_datas;
		return s_datas;
	}

};

}
#endif  // _YGJ_CONFIG_H_
