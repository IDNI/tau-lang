// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <algorithm>
#include <array>
#include <optional>
#include <vector>

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
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(t); it != cache.end()) return it->second;
#endif // TAU_CACHE
	bool result = tau::get(t)[0].get_string() == "tau";
#ifdef TAU_CACHE
	cache.emplace(t, result);
#endif // TAU_CACHE
	return result;
}

template <NodeType node>
bool is_tau_type(size_t t) {
	auto tt = ba_types<node>::type_tree(t);
	// Advisory drop: bool contract, no channel for a report here.
	return tt.has_value() && is_tau_type<node>(tt.value());
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
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(t); it != cache.end()) return it->second;
#endif // TAU_CACHE
	bool result = tau::get(t)[0].get_string() == "nat";
#ifdef TAU_CACHE
	cache.emplace(t, result);
#endif // TAU_CACHE
	return result;
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
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(t); it != cache.end()) return it->second;
#endif // TAU_CACHE
	bool result = tau::get(t)[0].get_string() == "untyped";
#ifdef TAU_CACHE
	cache.emplace(t, result);
#endif // TAU_CACHE
	return result;
}

template <NodeType node>
bool is_untyped(size_t t) {
	auto tt = ba_types<node>::type_tree(t);
	// Advisory drop: bool contract, no channel for a report here.
	return tt.has_value() && is_untyped<node>(tt.value());
}

template<NodeType node>
bool is_bool_type(tref t) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(t); it != cache.end()) return it->second;
#endif // TAU_CACHE
	bool result = tau::get(t)[0].get_string() == "bool";
#ifdef TAU_CACHE
	cache.emplace(t, result);
#endif // TAU_CACHE
	return result;
}

template <NodeType node>
bool is_bool_type(size_t t) {
	auto tt = ba_types<node>::type_tree(t);
	// Advisory drop: bool contract, no channel for a report here.
	return tt.has_value() && is_bool_type<node>(tt.value());
}

namespace ba_types_detail {

template <NodeType node>
tref make_syntactic_type_tree(const char* name) {
	using tau = tree<node>;

	return tau::get(tau::typed, tau::get(tau::type, name));
}

template <typename BA, NodeType node>
bool type_tree_name_is(tref t, const char* name) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(t); it != cache.end()) return it->second;
#endif // TAU_CACHE
	bool result = tau::get(t)[0].get_string() == name;
#ifdef TAU_CACHE
	cache.emplace(t, result);
#endif // TAU_CACHE
	return result;
}

template <typename BA, NodeType node>
bool type_tree_name_is(size_t ba_type_id, const char* name) {
	auto t = ba_types<node>::type_tree(ba_type_id);
	// Advisory drop: bool contract, no channel for a report here.
	return t.has_value() && type_tree_name_is<BA, node>(t.value(), name);
}

} // namespace ba_types_detail

// -----------------------------------------------------------------------------
// BA types

template <NodeType node>
size_t ba_types<node>::id(tref ba_type) {
	using tau = tree<node>;
	if (auto it = type_tree_to_idx().find(ba_type);
		it != type_tree_to_idx().end()) return it->second;
	// node::ba_type stores this id, so it must not outgrow that field.
	DBG(assert(type_trees().size()
		< std::numeric_limits<decltype(node::ba_type)>::max());)
	return type_tree_to_idx().emplace(ba_type, type_trees().size()),
		type_trees().push_back(tau::geth(ba_type)), type_trees().size() - 1;
}

// A ba_type_id past the end of type_trees() means the id was corrupted
// somewhere upstream; silently clamping it to 0 (untyped) in release --
// while asserting in debug -- masked that corruption as a valid type,
// which unify(size_t,size_t) would then happily merge with whatever the
// other operand's type is. Fail loudly in both configurations instead.
template<NodeType node>
result<tref> ba_types<node>::type_tree(size_t ba_type_id) {
	result<tref> r;
	if (ba_type_id >= type_trees().size())
		return r.with_assert_check_error(code::out_of_range,
			messages::invalid_ba_type_id,
			{{label::actual, ba_type_id}, {label::limit, type_trees().size()}});
	return r.with_assert_check_value(type_trees()[ba_type_id]->get());
}

template <NodeType node>
size_t ba_types<node>::count() {
	return type_trees().size();
}

template <NodeType node>
result<std::string> ba_types<node>::name(size_t ba_type_id) {
	result<std::string> r;
	if (ba_type_id >= type_trees().size())
		return r.with_assert_check_error(code::out_of_range,
			messages::invalid_ba_type_id,
			{{label::actual, ba_type_id}, {label::limit, type_trees().size()}});
	// type_trees() entries are stable once registered (see id()), so the
	// name is cached here rather than re-stringified on every call: this
	// is on the hot path of node::hashit(), called for every tree node
	// constructed with a non-untyped ba_type. Cached via tree<node>'s
	// GC-aware cache registry (same pattern as get_free_vars in
	// tau_tree_extractors.tmpl.h), keyed by the type tree itself since
	// create_cache requires tref-shaped keys.
#ifdef TAU_CACHE
	using tau = tree<node>;
	using cache_t = subtree_unordered_map<node, std::string>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	tref type = type_trees()[ba_type_id]->get();
	if (auto it = cache.find(type); it != cache.end())
		return r.with_assert_check_value(it->second);
	return r.with_assert_check_value(
		cache.emplace(type, tau::get(type).to_str()).first->second);
#endif // TAU_CACHE
	return r.with_assert_check_value(
		tree<node>::get(type_trees()[ba_type_id]->get()).to_str());
}

template <NodeType node>
size_t ba_types<node>::name_hash(size_t ba_type_id) {
	// id 0 must short-circuit exactly like get_ba_type_name(0): name(0)
	// touches type_trees(), whose static initializer is itself building
	// the untyped type's nodes (and hashing them through here) the first
	// time -- going through name() recurses into that initialization
	// (__gnu_cxx::recursive_init_error).
	if (ba_type_id == 0) {
		static const size_t h0 = std::hash<std::string>{}(":untyped");
		return h0;
	}
	DBG(assert(ba_type_id < type_trees().size());)
	// index = ba_type id; type_trees() entries are stable once registered
	// (see id()), so a computed hash never changes. 0 marks "not yet
	// computed" -- no valid name hashes to 0 in practice, and a spurious
	// recompute would be harmless (same value again).
	static std::vector<size_t> cache;
	if (ba_type_id < cache.size() && cache[ba_type_id])
		return cache[ba_type_id];
	if (ba_type_id >= cache.size()) cache.resize(ba_type_id + 1, 0);
	auto nm = name(ba_type_id);
	// Advisory drop: node::hashit()'s noexcept contract has no channel for a report here.
	if (!nm.has_value()) {
		static const size_t h_invalid = std::hash<std::string>{}(":invalid");
		return h_invalid;
	}
	return cache[ba_type_id] = std::hash<std::string>{}(nm.value());
}

template <NodeType node>
std::ostream& ba_types<node>::print(std::ostream& os, size_t tid) {
	auto nm = name(tid);
	// Advisory drop: std::ostream& contract cannot abort the line.
	return os << (nm.has_value() ? nm.value() : std::string("INVALID"));
}

template <NodeType node>
std::ostream& ba_types<node>::dump(std::ostream& os) {
	LOG_TRACE << "BA types pool(" << type_trees().size() << "):\n";
	os << "BA type_trees pool(" << type_trees().size() << "):\n";
	for (size_t i = 0; i < type_trees().size(); ++i) {
		auto nm = name(i);
		// Advisory drop: std::ostream& contract cannot abort the line.
		std::string s = nm.has_value() ? nm.value() : std::string("INVALID");
		LOG_TRACE << "type: " << i;
		LOG_TRACE << "val:  " << s;
		os << LOG_INDENT << "type: "
			<< i << " " << s << "\n";
	}
	return os;
}

template <NodeType node>
std::string ba_types<node>::dump_to_str() {
	std::stringstream ss;
	return dump(ss), ss.str();
}

// type_trees (index = ba_type id): untyped at 0, then each BA of the pack in
// pack order, asking its descriptor for its own type. Pack order is the only
// order there is -- no id outside this file is a fixed number.
template <NodeType node>
std::vector<htref>& ba_types<node>::type_trees() {
	using tau = tree<node>;
	static std::vector<htref> t = [] {
		std::vector<htref> v { tau::geth(untyped_type<node>()) };
		pack_visit_all<node>([&]<typename BA>() {
			if constexpr (ba_has_descriptor_v<node, BA>)
				v.push_back(tau::geth(
					ba_descriptor<BA, node>::type_tree()));
		});
		return v;
	}();
	return t;
}

// type_sid -> ba_type id
template <NodeType node>
subtree_map<node, size_t>& ba_types<node>::type_tree_to_idx() {
	using tau = tree<node>;
	// type_trees() first, so its htrefs exist before t is initialized and the
	// pack's type-tree nodes survive any subsequent do_gc().
	static subtree_map<node, size_t> t = [] {
		subtree_map<node, size_t> m;
		auto& tt = type_trees();
		for (size_t i = 0; i < tt.size(); ++i)
			m.emplace(tt[i]->get(), i);
		return m;
	}();
	// Register GC callback to rebuild from surviving type_trees() htrefs.
	static bool gc_registered = false;
	if (!gc_registered) {
		gc_registered = true;
		tau::gc_callbacks.push_back(
			[](const std::unordered_set<tref>& kept) {
			auto& tt = type_trees();
			auto& idx = type_tree_to_idx();
			idx.clear();
			for (size_t i = 0; i < tt.size(); ++i) {
				tref tr = tt[i]->get();
				if (kept.count(tr)) idx.emplace(tr, i);
			}
		});
	}
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
	auto t = ba_types<node>::type_tree(ba_type_id);
	// Advisory drop: tref contract, nullptr is this function's existing invalid-id answer.
	return t.has_value() ? t.value() : nullptr;
}

template <NodeType node>
size_t get_ba_type_count() {
	return ba_types<node>::count();
}

template <NodeType node>
result<std::string> get_ba_type_name(size_t ba_type_id) {
	result<std::string> r;
	// This is needed in order to initialize the type trees
	if (ba_type_id == 0) return r.with_value(":untyped");
	TAU_TRY(auto name, ba_types<node>::name(ba_type_id));
	return r.with_value(std::move(name));
}

template<NodeType node>
bool is_same_ba_type(tref t1, tref t2) {
	using tau = tree<node>;

	return tau::get(t1) == tau::get(t2);
}

template <NodeType node>
tref unify(tref t1, tref t2) {
	using tau = tree<node>;

	// Natural type never unifies (barred from BA unification)
	if (is_nat_type<node>(t1) || is_nat_type<node>(t2)) return nullptr;
	// If one is untyped return the other
	if (is_untyped<node>(t1)) return t2;
	if (is_untyped<node>(t2)) return t1;
	// If they are the same type return either
	if (is_same_ba_type<node>(t1, t2)) return t1;
	// If t1 and t2 have the same family name, subtype aside: subtype is
	// now type's own child, so a whole-subtree compare would also compare
	// the width, and typed's children_size no longer reflects it either.
	tref f1 = tau::trim(t1), f2 = tau::trim(t2);
	if (tau::get(f1).get_string() == tau::get(f2).get_string()) {
		// If t1 or t2 does not have a type parameter
		if (tau::get(f1).children_size() == 0)
			return t2;
		if (tau::get(f2).children_size() == 0)
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
		result = unify<node>(result, ns1[i]);
		if (result == nullptr) return nullptr;
		result = unify<node>(result, ns2[i]);
		if (result == nullptr) return nullptr;
	}
	return result;
}

template <NodeType node>
result<size_t> unify(size_t tid1, size_t tid2) {
	result<size_t> r;
	TAU_TRY(tref t1, ba_types<node>::type_tree(tid1));
	TAU_TRY(tref t2, ba_types<node>::type_tree(tid2));
	auto unified = unify<node>(t1, t2);
	if (!unified) return r; // conflict: no value, no error
	return r.with_value(ba_types<node>::id(unified));
}

template <NodeType node>
std::optional<size_t> unify(const std::vector<size_t>& nids, size_t default_type) {
	std::optional<size_t> result = default_type;
	for (size_t i = 0; i < nids.size(); ++i) {
		auto u = unify<node>(result.value(), nids[i]);
		// Advisory drop: optional<size_t> conflict-only contract has no channel for a report here.
		if (!u.has_value()) return std::nullopt;
		result = u.value();
	}
	return result;
}

template <NodeType node>
std::optional<size_t> unify(const std::vector<size_t>& nids1, const std::vector<size_t>& nids2, size_t default_type) {
	if (nids1.size() != nids2.size()) return std::nullopt;
	std::optional<size_t> result = default_type;
	for (size_t i = 0; i < nids1.size(); ++i) {
		auto u1 = unify<node>(result.value(), nids1[i]);
		// Advisory drop: optional<size_t> conflict-only contract has no channel for a report here.
		if (!u1.has_value()) return std::nullopt;
		result = u1.value();
		auto u2 = unify<node>(result.value(), nids2[i]);
		// Advisory drop: optional<size_t> conflict-only contract has no channel for a report here.
		if (!u2.has_value()) return std::nullopt;
		result = u2.value();
	}
	return result;
}

template<NodeType node>
bool is_untyped_tref(tref t) {
	using tau = tree<node>;
	if (tau::get(t).get_ba_type() != 0) return false;
	for (auto c : tau::get(t).get_children())
		if (tau::get(c).is(tau::typed)) return false;
	return true;
}

template<NodeType node>
size_t get_effective_ba_type(tref t) {
	using tau = tree<node>;
	auto ba_type = tau::get(t).get_ba_type();
	if (ba_type != 0) return ba_type;
	for (auto c : tau::get(t).get_children())
		if (tau::get(c).is(tau::typed))
			return get_ba_type_id<node>(c);
	return 0;
}

template<NodeType node>
bool has_ba_type(tref term) {
	return !is_untyped_tref<node>(term);
}

// The types core builds for itself: the default type of an untyped element,
// nat and the untyped marker, none of which a BA owns.
template <NodeType node>
bool is_reserved_ba_type(size_t ba_type_id) {
	return ba_type_id == tau_type_id<node>()
		|| ba_type_id == nat_type_id<node>()
		|| ba_type_id == untyped_type_id<node>();
}

template <NodeType node>
bool is_reserved_ba_type_name(const std::string& name) {
	using tau = tree<node>;
	auto family = [](tref type_tree) {
		return tau::get(type_tree)[0].get_string();
	};
	static const std::array<std::string, 3> reserved = {
		family(tau_type<node>()), family(nat_type<node>()),
		family(untyped_type<node>()) };
	return std::ranges::find(reserved, name) != reserved.end();
}

template <NodeType node>
bool pack_owns_ba_type_name(const std::string& name) {
	if (is_reserved_ba_type_name<node>(name)) return true;
	for (const auto& n : node::ba::type_names())
		if (n == name) return true;
	return false;
}

// The type tree is 'typed(type[, subtype])'; only the family name in
// type's own data (e.g. "bv" for bv[8]) is compared against the pack.
template <NodeType node>
bool pack_owns_ba_type(size_t ba_type_id) {
	using tau = tree<node>;
	if (is_reserved_ba_type<node>(ba_type_id)) return true;
	auto tt = ba_types<node>::type_tree(ba_type_id);
	// Advisory drop: bool contract, no channel for a report here.
	if (!tt.has_value()) return false;
	return pack_owns_ba_type_name<node>(tau::get(tt.value())[0].get_string());
}

template <NodeType node>
result<size_t> pack_default_ba_type(size_t type_id) {
	result<size_t> r;
	TAU_TRY(tref type_tree, ba_types<node>::type_tree(type_id));
	return r.with_value(pack_owner_apply<node>(type_id, [&]<typename BA>()
		-> std::optional<size_t> {
			if constexpr (ba_has_type_tree_for<node, BA>)
				if (auto param = ba_descriptor<BA, node>::type_param(
						type_tree))
					return ba_descriptor<BA, node>::type_id_for(*param);
			return std::nullopt;
		}).value_or(type_id));
}

template <NodeType node>
size_t find_ba_type (tref term) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, size_t>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(term); it != cache.end()) return it->second;
#endif // TAU_CACHE
	size_t type = tau::get(term).get_ba_type();
	if (type != 0) {
#ifdef TAU_CACHE
		cache.emplace(term, type);
#endif // TAU_CACHE
		return type;
	}
	auto f = [&type](const tref n) {
		type = tau::get(n).get_ba_type();
		return type == 0;
	};
	pre_order<node>(term).search_unique(f);
#ifdef TAU_CACHE
	cache.emplace(term, type);
#endif // TAU_CACHE
	return type;
}

template <NodeType node>
tref find_ba_type_tree (tref term) {
	const size_t t = find_ba_type<node>(term);
	auto tt = ba_types<node>::type_tree(t);
	// Advisory drop: tref contract, nullptr is this function's existing not-found answer.
	return tt.has_value() ? tt.value() : nullptr;
}

template <NodeType node>
size_t find_ba_type_or_default (tref term) {
	size_t type = find_ba_type<node>(term);
	return type > 0 ? type : get_ba_type_id<node>(node::ba::default_type());
}

template <NodeType node>
std::ostream& print_ba_type(std::ostream& os, size_t ba_type_id) {
	return ba_types<node>::print(os, ba_type_id);
}

template <NodeType node>
bool is_buildable(size_t op, tref n, tref m) {
	using tau = tree<node>;

	auto n_type = tau::get(n).get_type();
	auto m_type = tau::get(m).get_type();
	if (n_type != m_type) return false;
	auto n_ba_type = tau::get(n).get_ba_type();
	auto m_ba_type = tau::get(m).get_ba_type();
	auto unified_r = unify<node>(n_ba_type, m_ba_type);
	// Advisory drop: bool contract, no channel for a report here.
	if (!unified_r.has_value()) return false;
	size_t unified = unified_r.value();
	switch (op) {
		case tau::bf_add: case tau::bf_sub: case tau::bf_mul:
		case tau::bf_div: case tau::bf_mod: case tau::bf_shr:
		case tau::bf_shl: case tau::bf_xnor: case tau::bf_nand:
		case tau::bf_nor: case tau::bf_min: case tau::bf_max: {
			return pack_type_has_arith_ops<node>(unified)
				|| is_untyped<node>(unified);
		}
		// BA2-15: bf_neg dropped from this switch -- it is unary and a
		// two-operand buildability answer for it was meaningless.
		case tau::bf_or: case tau::bf_xor: case tau::bf_and: {
			return true;
		}
		default:
			return false;
	}
}

} // namespace idni::tau_lang
