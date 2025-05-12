// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "ba_types.h"

namespace idni::tau_lang {

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ba_types"

// -----------------------------------------------------------------------------
// BA types

template <typename... BAs>
requires BAsPack<BAs...>
size_t ba_types<BAs...>::type_id(size_t type_sid) {
	if (types.empty()) types.push_back(string_id("untyped")),
			type_names_map.emplace(types.back(), 0);
	if (auto it = type_names_map.find(type_sid);
		it != type_names_map.end()) return it->second;
	return type_names_map.emplace(type_sid, types.size()),
		types.push_back(type_sid), types.size() - 1;
}

template <typename... BAs>
requires BAsPack<BAs...>
size_t ba_types<BAs...>::type_id(const std::string& name) {
	// TODO properly initialize types with "untyped" as first element
	return type_id(string_id(name));
}

template <typename... BAs>
requires BAsPack<BAs...>
const std::string& ba_types<BAs...>::type_name(size_t tid) {
	if (tid >= types.size()) tid = 0;
	return string_from_id(types[tid]);
}

template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& ba_types<BAs...>::print_type(std::ostream& os, size_t tid) {
	return os << type_name(tid);
}

template <NodeType node>
size_t get_ba_type_id(size_t ba_type_sid) {
	return  node::ba_types::type_id(ba_type_sid);
}

template <NodeType node>
size_t get_ba_type_id(const std::string& ba_type_name) {
	return node::ba_types::type_id(ba_type_name);
}

template <NodeType node>
std::string get_ba_type_name(size_t ba_type_id) {
	return node::ba_types::type_name(ba_type_id);
}

template <NodeType node>
std::ostream& print_ba_type(std::ostream& os, size_t ba_type_id) {
	return node::ba_types::print_type(os, ba_type_id);
}

} // idni::tau_lang namespace
