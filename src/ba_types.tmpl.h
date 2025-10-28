// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "ba_types.h"

namespace idni::tau_lang {

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ba_types"

// -----------------------------------------------------------------------------
// BA types

template <NodeType node>
size_t ba_types<node>::id(size_t type_sid) {
	if (auto it = type_names_map.find(type_sid);
		it != type_names_map.end()) return it->second;
	return type_names_map.emplace(type_sid, types.size()),
		types.push_back(type_sid), types.size() - 1;
}

template <NodeType node>
size_t ba_types<node>::id(const std::string& name) {
	return id(dict(name));
}

template <NodeType node>
const std::string& ba_types<node>::name(size_t tid) {
	DBG(assert(tid < types.size()));
	if (tid >= types.size()) tid = 0;
	return dict(types[tid]);
}

template <NodeType node>
std::ostream& ba_types<node>::print(std::ostream& os, size_t tid) {
	return os << name(tid);
}

template <NodeType node>
std::ostream& ba_types<node>::dump(std::ostream& os) {
	LOG_TRACE << "BA types pool(" << types.size() << "):\n";
	os << "BA types pool(" << types.size() << "):\n";
	for (size_t i = 0; i < types.size(); ++i) {
		LOG_TRACE << "type: " << i+1;
		LOG_TRACE << "val:  " << dict(types[i]);
		os << LOG_INDENT << "type: "
			<< i+1 << " " << dict(types[i]) << "\n";
	}
	return os;
}

template <NodeType node>
std::string ba_types<node>::dump_to_str() {
	std::stringstream ss;
	return dump(ss), ss.str();
}

// -----------------------------------------------------------------------------
// functional API to ba_types

template <NodeType node>
size_t get_ba_type_id(size_t ba_type_sid) {
	return  ba_types<node>::id(ba_type_sid);
}

template <NodeType node>
size_t get_ba_type_id(const std::string& ba_type_name) {
	return ba_types<node>::id(ba_type_name);
}

template <NodeType node>
const std::string& get_ba_type_name(size_t ba_type_id) {
	return ba_types<node>::name(ba_type_id);
}

template <NodeType node>
std::ostream& print_ba_type(std::ostream& os, size_t ba_type_id) {
	return ba_types<node>::print(os, ba_type_id);
}

} // namespace idni::tau_lang
