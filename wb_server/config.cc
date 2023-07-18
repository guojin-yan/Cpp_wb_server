#include "config.h"

#include <list>
#include <map>
#include <string>

namespace ygj_server {
//Config::ConfigValMap Config::get_datas();

ConfigVarBase::ptr Config::lookup_base(const std::string& name) {
	auto it = get_datas().find(name);
	return it == get_datas().end() ? nullptr : it->second;
}

static void list_all_menmber(const std::string prefix, const YAML::Node& node,
	std::list<std::pair<std::string, const YAML::Node>>& output) {
	if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0.123456789")
		!= std::string::npos) {
		YGJ_LOG_ERROR(YGJ_LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
	}
	output.push_back(std::make_pair(prefix, node));
	if (node.IsMap()) {
		for (auto it = node.begin(); it != node.end(); ++it) {
			list_all_menmber(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);
		}

	}
}
void Config::load_from_yaml(const YAML::Node& root) {
	std::list<std::pair<std::string, const YAML::Node>> all_nodes;
	list_all_menmber("", root, all_nodes);
	for (auto& i :all_nodes) { // ����ڵ����
		std::string key = i.first;
		if (key.empty()) {
			continue;
		}
		// ����д��ĸתСд
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		ConfigVarBase::ptr var = lookup_base(key);
		if (var) {
			if (i.second.IsScalar()) {
				var->from_string(i.second.Scalar());
			} else {
				std::stringstream ss;
				ss << i.second;
				var->from_string(ss.str());
			}
		}
	}
}

}
