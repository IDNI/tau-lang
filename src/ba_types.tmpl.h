// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "ba_types.h"

namespace idni::tau_lang {

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ba_types"


template<NodeType node>
tref tau_type() {
	using tau = tree<node>;
	tref type = tau::get(tau::type, "tau");
	return tau::get(tau::typed, type);
}

template<NodeType node>
bool is_tau_type(tref t) {
	using tau = tree<node>;
	return tau::get(t)[0].get_string() == "tau";
}

template<NodeType node>
tref nat_type() {
	using tau = tree<node>;
	tref type = tau::get(tau::type, "nat");
	return tau::get(tau::typed, type);
}

template<NodeType node>
bool is_nat_type(tref t) {
	using tau = tree<node>;
	return tau::get(t)[0].get_string() == "nat";
}

template<NodeType node>
tref untyped_type() {
	using tau = tree<node>;
	tref type = tau::get(tau::type, "untyped");
	return tau::get(tau::typed, type);
}

template<NodeType node>
bool is_untyped(tref t) {
	using tau = tree<node>;
	return tau::get(t)[0].get_string() == "untyped";
}

// bitvector type definitions
// TODO: They must go into bv_ba.impl.h -> currently causes compilation error
template<NodeType node>
tref bv_type(unsigned short bitwidth) {
	using tau = tree<node>;
	tref type = tau::get(tau::type, "bv");
	tref subtype = tau::get(tau::subtype, tau::get_num(bitwidth));
	return tau::get(tau::typed, type, subtype);
}

template<NodeType node>
bool is_bv_type_family(tref t) {
	using tau = tree<node>;
	return tau::get(t)[0].get_string() == "bv";
}

template<NodeType node>
bool is_bv_type_family(size_t ba_type_id) {
	return is_bv_type_family<node>(ba_types<node>::type_tree(ba_type_id));
}


// -----------------------------------------------------------------------------
// BA types

template <NodeType node>
size_t ba_types<node>::id(tref ba_type) {
	if (auto it = type_tree_to_idx().find(ba_type);
		it != type_tree_to_idx().end()) return it->second;
	return type_tree_to_idx().emplace(ba_type, type_trees().size()),
		type_trees().push_back(ba_type), type_trees().size() - 1;
}

template<NodeType node>
tref ba_types<node>::type_tree(size_t ba_type_id) {
	DBG(assert(ba_type_id < type_trees().size()));
	if (ba_type_id >= type_trees().size()) ba_type_id = 0;
	return type_trees()[ba_type_id];
}

template <NodeType node>
std::string ba_types<node>::name(size_t ba_type_id) {
	using tau = tree<node>;
	DBG(assert(ba_type_id < type_trees().size()));
	if (ba_type_id >= type_trees().size()) ba_type_id = 0;
	return tau::get(type_trees()[ba_type_id]).to_str();
}

template <NodeType node>
std::ostream& ba_types<node>::print(std::ostream& os, size_t tid) {
	return os << name(tid);
}

template <NodeType node>
std::ostream& ba_types<node>::dump(std::ostream& os) {
	LOG_TRACE << "BA types pool(" << type_trees().size() << "):\n";
	os << "BA type_trees pool(" << type_trees().size() << "):\n";
	for (size_t i = 0; i < type_trees().size(); ++i) {
		LOG_TRACE << "type: " << i+1;
		LOG_TRACE << "val:  " << name(i);
		os << LOG_INDENT << "type: "
			<< i+1 << " " << name(i) << "\n";
	}
	return os;
}

template <NodeType node>
std::string ba_types<node>::dump_to_str() {
	std::stringstream ss;
	return dump(ss), ss.str();
}

// type_sids (index = ba_type id)
template <NodeType node>
std::vector<tref>& ba_types<node>::type_trees() {
	static std::vector<tref> t { untyped_type<node>(), tau_type<node>(),
		bv_type<node>(), sbf_type<node>() };
	return t;
}

// type_sid -> ba_type id
template <NodeType node>
subtree_map<node, size_t>& ba_types<node>::type_tree_to_idx() {
	static subtree_map<node, size_t> t{ {untyped_type<node>(), 0 },
		{ tau_type<node>(), 1 }, { bv_type<node>(), 2 },
		{ sbf_type<node>(), 3 }
	};
	return t;
}

// -----------------------------------------------------------------------------
// functional API to ba_types

template <NodeType node>
size_t get_ba_type_id(tref ba_type) {
	return ba_types<node>::id(ba_type);
}

template<NodeType node>
tref get_ba_type_tree(size_t ba_type_id) {
	return ba_types<node>::type_tree(ba_type_id);
}

template <NodeType node>
std::string get_ba_type_name(size_t ba_type_id) {
	// This is needed in order to initialize the type trees
	if (ba_type_id == 0) return "untyped";
	return ba_types<node>::name(ba_type_id);
}

template<NodeType node>
bool is_same_ba_type(tref t1, tref t2) {
	using tau = tree<node>;
	return tau::get(t1) == tau::get(t2);
}

template <NodeType node>
std::ostream& print_ba_type(std::ostream& os, size_t ba_type_id) {
	return ba_types<node>::print(os, ba_type_id);
}

} // namespace idni::tau_lang
