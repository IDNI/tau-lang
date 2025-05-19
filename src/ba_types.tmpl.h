// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "ba_types.h"

namespace idni::tau_lang {

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ba_types"

// -----------------------------------------------------------------------------
// BA types

template <NodeType node>
size_t ba_types<node>::type_id(size_t type_sid) {
	if (types.empty()) types.push_back(string_id("untyped")),
			type_names_map.emplace(types.back(), 0);
	if (auto it = type_names_map.find(type_sid);
		it != type_names_map.end()) return it->second;
	return type_names_map.emplace(type_sid, types.size()),
		types.push_back(type_sid), types.size() - 1;
}

template <NodeType node>
size_t ba_types<node>::type_id(const std::string& name) {
	// TODO properly initialize types with "untyped" as first element
	return type_id(dict(name));
}

template <NodeType node>
const std::string& ba_types<node>::type_name(size_t tid) {
	if (tid >= types.size()) tid = 0;
	return dict(types[tid]);
}

template <NodeType node>
std::ostream& ba_types<node>::print_type(std::ostream& os, size_t tid) {
	return os << type_name(tid);
}

template <NodeType node>
size_t get_ba_type_id(size_t ba_type_sid) {
	return  ba_types<node>::type_id(ba_type_sid);
}

template <NodeType node>
size_t get_ba_type_id(const std::string& ba_type_name) {
	return ba_types<node>::type_id(ba_type_name);
}

template <NodeType node>
const std::string& get_ba_type_name(size_t ba_type_id) {
	return ba_types<node>::type_name(ba_type_id);
}

template <NodeType node>
std::ostream& print_ba_type(std::ostream& os, size_t ba_type_id) {
	return ba_types<node>::print_type(os, ba_type_id);
}

} // namespace idni::tau_lang
