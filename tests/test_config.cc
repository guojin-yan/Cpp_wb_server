#include <iostream>
#include <thread>
#include <yaml-cpp/yaml.h>
#ifdef _WIN32
#include "../wb_server/log.h"
#include "../wb_server/util.h"
#include "../wb_server/config.h"
#else
#include "../wb_server/log.h"
#include "../wb_server/util.h"
#include "../wb_server/config.h"
#endif



void print_yaml(const YAML::Node& node, int level) {
	if (node.IsScalar()) {
		YGJ_LOG_INFO(YGJ_LOG_ROOT()) << std::string(level * 4,' ')
			<< node.Scalar() << " - " << node.Type() << " - " << level;
	} else if (node.IsNull()) {
		YGJ_LOG_INFO(YGJ_LOG_ROOT()) << std::string(level * 4, ' ')
			<< "NULL - " << node.Type() << " - " << level;
	} else if (node.IsMap()) {
		for (auto it = node.begin(); it != node.end(); ++it) {
			YGJ_LOG_INFO(YGJ_LOG_ROOT()) << std::string(level * 4, ' ')
				<< it->first << " - " << it->second.Type() << " - " << level;
			print_yaml(it->second, level + 1);
		}
	} else if (node.IsSequence()) {
		for (size_t i = 0; i < node.size(); ++i) {
			YGJ_LOG_INFO(YGJ_LOG_ROOT()) << std::string(level * 4, ' ')
				<< i << " - " << node[i].Type() << " - " << level;
			print_yaml(node[i], level + 1);
		}
	}
}

void test_yaml() {
#ifdef _WIN32
	YAML::Node root = YAML::LoadFile("E:\\VMwareShare\\C++高性能服务器框架\\bin\\conf\\log.yml");
#else
	YAML::Node root = YAML::LoadFile("/mnt/hgfs/cpp_wb_server/bin/conf/log.yml");
#endif

	print_yaml(root, 0);
	/*YGJ_LOG_INFO(YGJ_LOG_ROOT()) << root;*/

}

void test_config() {

	ygj_server::ConfigVar<int>::ptr g_int_value_config =
		ygj_server::Config::lookup("system.port", (int)8080, "system port");
	ygj_server::ConfigVar<int>::ptr g_int_valuex_config =
		ygj_server::Config::lookup("system.port", (int)8080, "system port");

	ygj_server::ConfigVar<float>::ptr g_float_value_config =
		ygj_server::Config::lookup("system.value", (float)10.2, "system value");

	ygj_server::ConfigVar<std::vector<int>>::ptr g_vector_value_config =
		ygj_server::Config::lookup("system.int_vector", std::vector<int>{1, 2, 3}, "system int_vector");

	ygj_server::ConfigVar<std::list<int>>::ptr g_list_value_config =
		ygj_server::Config::lookup("system.int_list", std::list<int>{1, 2, 3}, "system int_list");
	ygj_server::ConfigVar<std::set<int>>::ptr g_set_value_config =
		ygj_server::Config::lookup("system.int_set", std::set<int>{1, 2, 3}, "system int_set");
	ygj_server::ConfigVar<std::unordered_set<int>>::ptr g_uset_value_config =
		ygj_server::Config::lookup("system.int_uset", std::unordered_set<int>{1, 2, 3}, "system int_uset");

	ygj_server::ConfigVar<std::map<std::string, int>>::ptr g_map_value_config =
		ygj_server::Config::lookup("system.int_map", std::map<std::string, int>{{"k",2}}, "system int_map");
	ygj_server::ConfigVar<std::unordered_map<std::string, int>>::ptr g_umap_value_config =
		ygj_server::Config::lookup("system.int_umap", std::unordered_map<std::string, int>{ {"k", 2}}, "system int_umap");



	YGJ_LOG_INFO(YGJ_LOG_ROOT()) << "before: " << g_int_value_config->get_value();
	YGJ_LOG_INFO(YGJ_LOG_ROOT()) << "before: " << g_float_value_config->to_string();
#define XX(g_val, name, prefix) \
	{auto v = g_val->get_value();\
	for(auto& i : v) {\
		YGJ_LOG_INFO(YGJ_LOG_ROOT()) << #prefix "  " #name  " : " << i;\
	}\
		YGJ_LOG_INFO(YGJ_LOG_ROOT()) << #prefix "  " #name  " yaml: " << g_val->to_string();\
	}

#define XX_M(g_val, name, prefix) \
	{auto v = g_val->get_value();\
	for(auto& i : v) {\
		YGJ_LOG_INFO(YGJ_LOG_ROOT()) << #prefix "  " #name  " : {" << i.first << " - " << i.second << "}";\
	}\
		YGJ_LOG_INFO(YGJ_LOG_ROOT()) << #prefix "  " #name  " yaml: " << g_val->to_string();\
	}

	XX(g_vector_value_config, int_vec, before);
	XX(g_list_value_config, int_list, before);
	XX(g_set_value_config, int_set, before);
	XX(g_uset_value_config, int_uset, before);
	XX_M(g_map_value_config, int_map, before);
	XX_M(g_umap_value_config, int_umap, before);

#ifdef _WIN32
	YAML::Node root = YAML::LoadFile("E:\\VMwareShare\\C++高性能服务器框架\\bin\\conf\\log.yml");
#else
	YAML::Node root = YAML::LoadFile("/mnt/hgfs/cpp_wb_server/bin/conf/log.yml");
#endif
	ygj_server::Config::load_from_yaml(root);
	YGJ_LOG_INFO(YGJ_LOG_ROOT()) << "after: " << g_int_value_config->get_value();
	YGJ_LOG_INFO(YGJ_LOG_ROOT()) << "after: " << g_float_value_config->to_string();


	XX(g_vector_value_config, int_vec, after);
	XX(g_list_value_config, int_list, after);
	XX(g_set_value_config, int_set, after);
	XX(g_uset_value_config, int_uset, before);
	XX_M(g_map_value_config, int_map, before);
	XX_M(g_umap_value_config, int_umap, before);
}


class Person {
public:
	std::string m_name;
	int m_age;
	bool m_sex = 0;

	std::string to_string() const{
		std::stringstream ss;
		ss << "[Person name = " << m_name
			<< ", age = " << m_age
			<< ", sex = " << m_sex
			<< "]";
		return ss.str();
	}

	bool operator==(const Person& other) const {
		return m_age == other.m_age &&
			m_sex == other.m_sex &&
			m_name == other.m_name;
	}
};

namespace ygj_server {


template<>
class LexicalCast<std::string, Person> {
public:
	Person operator () (const std::string& v) {
		YAML::Node node = YAML::Load(v);
		Person p;
		p.m_name = node["name"].as<std::string>();
		p.m_age = node["age"].as<int>();
		p.m_sex = node["sex"].as<bool>();
		return p;
	}
};


template<>
class LexicalCast<Person, std::string> {
public:
	std::string operator () (const Person& p) {
		YAML::Node node;
		node["name"] = p.m_name;
		node["age"] = p.m_age;
		node["sex"] = p.m_sex;

		std::stringstream ss;
		ss << node;
		return ss.str();
	}
};

}

void test_class() {
	ygj_server::ConfigVar<Person>::ptr g_person =
		ygj_server::Config::lookup("class.person", Person(), "class person");
	ygj_server::ConfigVar<std::map<std::string, Person>>::ptr g_person_map =
		ygj_server::Config::lookup("class.map", std::map<std::string, Person>(), "class person");
	YGJ_LOG_INFO(YGJ_LOG_ROOT()) << "before: " << g_person->get_value().to_string() << " - " << g_person->to_string();

#define XX_PM(g_var, prefix) \
	{ \
		auto m = g_person_map->get_value(); \
		for (auto& i : m ) { \
			YGJ_LOG_INFO(YGJ_LOG_ROOT()) << prefix  << " : " << i.first << " - " << i.second.to_string(); \
		} \
		YGJ_LOG_INFO(YGJ_LOG_ROOT()) << prefix  << " : size = " << m.size(); \
	}

	g_person->add_listener(10, [](const Person& old_value, const Person& new_value) {
		YGJ_LOG_INFO(YGJ_LOG_ROOT()) << "old value=" << old_value.to_string() << " new_value=" <<
			new_value.to_string();
	});
	XX_PM(g_person_map, "class.map before");

#ifdef _WIN32
	YAML::Node root = YAML::LoadFile("E:\\VMwareShare\\C++高性能服务器框架\\bin\\conf\\log.yml");
#else
	YAML::Node root = YAML::LoadFile("/mnt/hgfs/cpp_wb_server/bin/conf/log.yml");
#endif
	ygj_server::Config::load_from_yaml(root);
	YGJ_LOG_INFO(YGJ_LOG_ROOT()) << "after: " << g_person->get_value().to_string() << " - " << g_person->to_string();
	XX_PM(g_person_map, "class.map after");
}

void test_log() {
	static ygj_server::Logger::ptr system_log = YGJ_LOG_NAME("system");

	YGJ_LOG_INFO(system_log) << "hello system" << std::endl;

	std::cout << ygj_server::LoggerMgr::get_instance()->to_yaml_string() << std::endl;
#ifdef _WIN32
	YAML::Node root = YAML::LoadFile("E:\\VMwareShare\\C++高性能服务器框架\\config\\log.yml");
#else
	YAML::Node root = YAML::LoadFile("/mnt/hgfs/cpp_wb_server/config/log.yml");
#endif

	ygj_server::Config::load_from_yaml(root);

	std::cout << "================================" << std::endl;
	std::cout << ygj_server::LoggerMgr::get_instance()->to_yaml_string() << std::endl;
	std::cout << "================================" << std::endl;
	std::cout << root << std::endl;

	YGJ_LOG_INFO(system_log) << "hello system" << std::endl;

	system_log->set_formatter("%d - %m%n");

	YGJ_LOG_INFO(system_log) << "hello system" << std::endl;
}

int main2(int argc, char** argv) {
	//YGJ_LOG_INFO(YGJ_LOG_ROOT()) << g_int_value_config->get_value();
	//YGJ_LOG_INFO(YGJ_LOG_ROOT()) << g_int_value_config->to_string();
	//YGJ_LOG_INFO(YGJ_LOG_ROOT()) << g_float_value_config->get_value();
	//YGJ_LOG_INFO(YGJ_LOG_ROOT()) << g_float_value_config->to_string();
	//test_config();

	//test_class();

	test_log();
	return  0;
}
