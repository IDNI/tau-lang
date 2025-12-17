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
inline size_t tau_type_id() {
	static size_t id = ba_types<node>::id(tau_type<node>());
	return id;
}

template<NodeType node>
bool is_tau_type(tref t) {
	using tau = tree<node>;

	return tau::get(t)[0].get_string() == "tau";
}

template <NodeType node>
bool is_tau_type(size_t t) {
	return is_tau_type<node>(ba_types<node>::type_tree(t));
}

template<NodeType node>
tref nat_type() {
	using tau = tree<node>;

	tref type = tau::get(tau::type, "nat");
	return tau::get(tau::typed, type);
}

template<NodeType node>
inline size_t nat_type_id() {
	static size_t id = ba_types<node>::id(nat_type<node>());
	return id;
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
inline size_t untyped_type_id() {
	static size_t id = ba_types<node>::id(untyped_type<node>());
	return id;
}

template<NodeType node>
bool is_untyped(tref t) {
	using tau = tree<node>;

	return tau::get(t)[0].get_string() == "untyped";
}

template <NodeType node>
bool is_untyped(size_t t) {
	return is_untyped<node>(ba_types<node>::type_tree(t));
}

// sbf type definitions
// TODO: They must go into sbf_ba.impl.h -> currently causes compilation error
template<NodeType node>
tref sbf_type() {
	using tau = tree<node>;

	tref type = tau::get(tau::type, "sbf");
	return tau::get(tau::typed, type);
}

template<NodeType node>
inline size_t sbf_type_id() {
	static size_t id = ba_types<node>::id(sbf_type<node>());
	return id;
}

template<NodeType node>
bool is_sbf_type(tref t) {
	using tau = tree<node>;

	return tau::get(t)[0].get_string() == "sbf";
}

template <NodeType node>
bool is_sbf_type(size_t t) {
	return is_sbf_type<node>(ba_types<node>::type_tree(t));
}


// bool type definitions
template<NodeType node>
tref bool_type() {
	using tau = tree<node>;

	tref type = tau::get(tau::type, "bool");
	return tau::get(tau::typed, type);
}

template<NodeType node>
inline size_t bool_type_id() {
	static size_t id = ba_types<node>::id(bool_type<node>());
	return id;
}

template<NodeType node>
bool is_bool_type(tref t) {
	using tau = tree<node>;

	return tau::get(t)[0].get_string() == "bool";
}

template <NodeType node>
bool is_bool_type(size_t t) {
	return is_bool_type<node>(ba_types<node>::type_tree(t));
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
inline size_t bv_type_id() {
	static size_t id = ba_types<node>::id(bv_type<node>());
	return id;
}

template<NodeType node>
size_t bv_type_id(unsigned short bitwidth) {
	return ba_types<node>::id(bv_type<node>(bitwidth));
}

template<NodeType node>
tref bv_base_type() {
	using tau = tree<node>;

	tref type = tau::get(tau::type, "bv");
	return tau::get(tau::typed, type);
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

template<NodeType node>
bool is_bv_base_type(tref n) {
	using tau = tree<node>;

	auto t = tau::get(n);
	return t[0].get_string() == "bv" && t.children_size() == 1;
}

template<NodeType node>
bool is_bv_base_type(size_t ba_type_id) {
	return is_bv_base_type<node>(ba_types<node>::type_tree(ba_type_id));
}

template <NodeType node>
size_t get_bv_width(tref t) {
	using tau = tree<node>;
	using tt = tau::traverser;

	DBG(assert(is_bv_type_family<node>(t)));
	if (size_t num = tt(t) | tau::subtype | tau::num | tt::num; num)
		return num;
	return default_bv_size;
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
	static std::vector<tref> t { 
		untyped_type<node>(), 
		tau_type<node>(),
		bv_type<node>(), 
		sbf_type<node>() };
	return t;
}

// type_sid -> ba_type id
template <NodeType node>
subtree_map<node, size_t>& ba_types<node>::type_tree_to_idx() {
	static subtree_map<node, size_t> t{ 
		{ untyped_type<node>(), 0 },
		{ tau_type<node>(), 1 }, 
		{ bv_type<node>(), 2 },
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
tref unify(tref t1, tref t2) {
	using tau = tree<node>;

	// If either is nat, return nat
	if (is_nat_type<node>(t1) || is_nat_type<node>(t2)) return nullptr;
	// If one is untyped return the other
	if (is_untyped<node>(t1)) return t2;
	if (is_untyped<node>(t2)) return t1;
	// If they are the same type return either
	if (is_same_ba_type<node>(t1, t2)) return t1;
	// If t1 and t2 have same type name
	if (tau::subtree_equals(tau::trim(t1), tau::trim(t2))) {
		// If t1 or t2 does not have a type parameter
		if (tau::get(t1).children_size() == 1)
			return t2;
		if (tau::get(t2).children_size() == 1)
			return t1;
	}
	return nullptr;
}

template <NodeType node>
tref unify(const trefs& ns, tref default_type) {
	tref result = default_type;
	for (size_t i = 0; i < ns.size(); ++i) {
		result = unify<node>(result, ns[i]);
		if (result == nullptr) return nullptr;
	}
	return result;
}

template <NodeType node>
tref unify(const trefs& ns1, const trefs& ns2, tref default_type) {
	if (ns1.size() != ns2.size()) return nullptr;
	auto result = default_type;
	for (size_t i = 0; i < ns1.size(); ++i) {
		auto result = unify<node>(result, ns1[i]);
		if (result == nullptr) return nullptr;
		result = unify<node>(result, ns2[i]);
		if (result == nullptr) return nullptr;
	}
	return result;
}

template <NodeType node>
std::optional<size_t> unify(size_t tid1, size_t tid2) {
	auto t1 = ba_types<node>::type_tree(tid1);
	auto t2 = ba_types<node>::type_tree(tid2);
	auto result = unify<node>(t1, t2);
	return result ? std::optional<size_t>{ ba_types<node>::id(result) } : std::nullopt;
}	

template <NodeType node>
std::optional<size_t> unify(const std::vector<size_t>& nids, size_t default_type) {
	std::optional<size_t> result = default_type;
	for (size_t i = 0; i < nids.size(); ++i) {
		result = unify<node>(result.value(), nids[i]);
		if (!result) return std::nullopt;
	}
	return result;
}

template <NodeType node>
std::optional<size_t> unify(const std::vector<size_t>& nids1, const std::vector<size_t>& nids2, size_t default_type) {
	if (nids1.size() != nids2.size()) return std::nullopt;
	std::optional<size_t> result = default_type;
	for (size_t i = 0; i < nids1.size(); ++i) {
		result = unify<node>(result.value(), nids1[i]);
		if (!result) return std::nullopt;
		result = unify<node>(result.value(), nids2[i]);
		if (!result) return std::nullopt;
	}
	return result;
}

template<NodeType node>
bool is_typeable(tref t) {
	using tau = tree<node>;

	return is<node, tau::variable>(t)
		|| is<node, tau::ba_constant>(t)
		|| is<node, tau::bf_t>(t)
		|| is<node, tau::bf_f>(t);
		//|| is<node, tau::ref>(t);
}

template <NodeType node>
bool is_typed(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;

	return (tt(n) | tau::typed | tt::ref) != nullptr;
}	

template <NodeType node>
bool are_typed(trefs ns) {
	for (auto n : ns)
		if (!is_typed<node>(n)) return false;
	return true;	
}	

template <NodeType node>
tref get_typed(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;

	return (tt(n) | tau::typed | tt::ref);
}	

template <NodeType node>
trefs get_typeds(trefs n) {
	trefs typed_nodes;
	for (auto ni : n) typed_nodes.push_back(get_typed<node>(ni));
	return typed_nodes;
}	

template <NodeType node>
size_t get_typed_id(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;

	if (auto check = tt(n) | tau::typed | tt::ref ; check)
		return ba_types<node>::id(check);
	return untyped_type_id<node>();	
}	

template <NodeType node>
std::vector<size_t> get_typed_ids(trefs n) {
	std::vector<size_t> type_ids;
	for (auto ni : n) type_ids.push_back(get_typed_id<node>(ni));
	return type_ids;
}


template<NodeType node>
tref untyped(tref n) {
	using tau = tree<node>;

	auto nt = tau::get(n).get_type();
	switch (nt) {
	case tau::bf_t:
	case tau::bf_f:
		return tau::get_typed(nt, tau::get(n).get_ba_type());
	case tau::variable:
	return tau::get_typed(tau::variable, {tau::get(n).first()}, tau::get(n).get_ba_type());
	case tau::ref: {
		auto children = tau::get(n).get_children();
		return tau::get_typed(tau::ref, children, tau::get(n).get_ba_type());
	}
	default:
		return n;
	}
}

template <NodeType node>
bool has_type(tref n) {
	using tau = tree<node>;

	return tau::get(n).get_ba_type() != untyped_type_id<node>();
}

template <NodeType node>
bool have_types(trefs t) {
	for (auto ti : t)
		if (!has_type<node>(ti)) return false;
	return true;
}

template <NodeType node>
tref get_type(tref t) {
	auto type_id = get_type_id<node>(t);
	return ba_types<node>::type_tree(type_id);	
}

template <NodeType node>
trefs get_types(trefs t) {
	trefs types;
	for (auto ti : t) types.push_back(get_type<node>(ti));
	return types;
}

template <NodeType node>
size_t get_type_id(tref t) {
	using tau = tree<node>;

	return tau::get(t).get_ba_type();
}

template <NodeType node>
std::vector<size_t> get_type_ids(trefs t) {
	std::vector<size_t> type_ids;
	for (auto ti : t) type_ids.push_back(get_type_id<node>(ti));
	return type_ids;
}

template<NodeType node>
tref untype(tref t) {
	using tau = tree<node>;
	using tt = tau::traverser;

	auto nt = tau::get(t).get_type();
	switch (nt) {
	case tau::bf_t:
		return tau::get(tau::bf_t);
	case tau::bf_f:
		return tau::get(tau::bf_f);
	case tau::ba_constant: {
		if (tau::get(t).children_size() > 0)
			return t; // tau::get(tau::ba_constant, tau::get(t).first());
		else
			return tau::get(tau::get(t).value.ba_retype(untyped_type_id<node>()));
	}
	case tau::variable:
	case tau::ref:
		if (auto check = tt(t) | tau::typed; check) {
			// copy all the children but the typed
			auto children = tau::get(t).get_children();
			trefs n_children;
			for (auto ch: tau::get(t).get_children())
				if (!is<node, tau::typed>(ch)) n_children.push_back(ch);
			return tau::get(nt, n_children);
		}
		return tau::get(nt, tau::get(t).get_children());
	default:
		return t;
	}
}

template <NodeType node>
size_t find_ba_type (tref term) {
	using tau = tree<node>;

	size_t type = tau::get(term).get_ba_type();
	if (type != 0) return type;
	auto f = [&type](const tref n) {
		const auto& t = tau::get(n);
		if (t.is(tau::bf)) type = t.get_ba_type();
		if (type > 0) return false;
		return true;
	};
	pre_order<node>(term).search_unique(f);
	return type;
}

template <NodeType node>
tref find_ba_type_tree (tref term) {
	const size_t t = find_ba_type<node>(term);
	return get_ba_type_tree<node>(t);
}

template <NodeType node>
std::ostream& print_ba_type(std::ostream& os, size_t ba_type_id) {
	return ba_types<node>::print(os, ba_type_id);
}

} // namespace idni::tau_lang
