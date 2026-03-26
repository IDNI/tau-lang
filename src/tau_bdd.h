// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef TAU_TAU_BDD_H
#define TAU_TAU_BDD_H

#include "tau_tree.h"

namespace idni::tau_lang {

// BDD node holding a reference to the BDD storage alongside
// information if children are inverted (usage of input and output inverters)
template <NodeType node>
struct tau_bdd_node {
	tref v = nullptr;
	bool inv_v : 1 = false;
	bool inv_h : 1 = false;
	bool inv_l : 1 = false;

	explicit tau_bdd_node(tref _v) : v(_v) {}

	tau_bdd_node(tref _v, bool _inv_v, bool _inv_h, bool _inv_l)
		: v(_v), inv_v(_inv_v), inv_h(_inv_h), inv_l(_inv_l) {}

	bool operator==(const tau_bdd_node& other) const;
	bool operator!=(const tau_bdd_node& other) const;

	bool operator<(const tau_bdd_node& other) const;
};

// Reference to a BDD supporting output inverters
template <NodeType node>
struct tau_bdd_ref {
	tref b = nullptr;
	bool inv = false;

	tau_bdd_ref() {}
	tau_bdd_ref(tref _b, bool _inv) : b(_b), inv(_inv) {}

	bool operator==(const tau_bdd_ref& other) const;
	bool operator!=(const tau_bdd_ref& other) const;

	bool operator<(const tau_bdd_ref& other) const;
};

template <NodeType node>
struct tau_term_bdd : bintree<tau_bdd_node<node>> {
	// The unordered map represents the ordering of variables,
	// ie each variable is assigned a number.
	// A lower number means the variable is higher up in the BDD.
	using order = subtree_unordered_map<node, int_t>;
	using ref = tau_bdd_ref<node>;
	using refs = std::vector<ref>;
	using bdd_node = tau_bdd_node<node>;

	tau_term_bdd(bdd_node v, tref h, tref l) : bintree<bdd_node>(v, h, l) {}

	enum Quantifier {ex, all};
	static ref T, F;

	using quants = std::vector<std::pair<tref, Quantifier>>;

#ifdef TAU_CACHE
	// Caches
	using cache_and_t = std::unordered_map<std::array<ref, 2>, ref>;
	static cache_and_t& and_memo =
		bintree<bdd_node>::template create_cache<cache_and_t>();
	using cache_and_many_t = std::unordered_map<refs, ref>;
	static cache_and_many_t& and_many_memo =
		bintree<bdd_node>::template create_cache<cache_and_many_t>();
	using cache_ex_t = std::map<trefs, std::unordered_map<ref, ref>>;
	static cache_ex_t& ex_memo =
		bintree<bdd_node>::template create_cache<cache_ex_t>();
	using cache_quant_t = std::map<std::vector<std::pair<tref, Quantifier>>,
		std::unordered_map<ref, ref>>;
	static cache_quant_t& quant_memo =
		bintree<bdd_node>::template create_cache<cache_quant_t>();
#endif

	// Canonical order for ref's
	static void make_canonical(ref& x, ref& y);
	// Check if the variable x is less than y according to order o
	static bool less_then(tref x, tref y, const order& o);

	// Canonically add a tau_bdd to the universe
	static ref add(tref v, ref h, ref l);
	static ref add(tref leaf);
	// Converts a reference to a tau BDD
	static tau_term_bdd get_node(ref x);
	static tref get_var(ref x);
	static ref get_high(ref x);
	static ref get_low(ref x);
	static bool leaf(ref l);

	static ref build_bdd(tref f, const order& o);
	static ref from_bit(tref v);

	static ref bdd_and(ref x, tref y);
	static ref bdd_and(ref x, ref y, const order& o);
	static ref bdd_or(ref x, ref y, const order& o);
	static ref bdd_not(ref x);

	static ref bdd_ex(ref x, trefs& v, const order& o);
	static ref bdd_all(ref x, trefs v, const order& o);
	static ref bdd_quant(ref x, const quants& v, const order& o);

	static ref bdd_and_many(refs v, const order& o);
	static ref bdd_or_many(refs v, const order& o);

	static tref to_tau_term(ref x, size_t term_type);

private:
	static constexpr auto am_cmp = [](ref x, ref y) {
		bool s = x < y;
		x.inv = false, y.inv = false;
		return x < y ? true : x == y ? s : false;
	};
	static size_t bdd_and_many_iter(const refs& v, refs& h, refs& l, ref& res,
		tref& m, const order& o);
	static void am_sort(refs& b);
	static bool am_simplify(refs& v, const std::unordered_map<refs, ref>& memo);
	static bool subset(const refs& small, const refs& big);
	static ref abs(ref x);
#ifdef TAU_CACHE
	static ref bdd_ex(ref x, const trefs& v, size_t i, const order& o, auto& memo);
	static ref bdd_quant(ref x, const quants& v, size_t i, const order& o, auto& memo);
#else
	static ref bdd_ex(ref x, const trefs& v, size_t i, const order& o);
	static ref bdd_quant(ref x, const quants& v, size_t i, const order& o);
#endif

};

// Handle for a BDD supporting output inverters
template <NodeType node>
struct tau_term_bdd_handle {
	// The unordered map represents the ordering of variables,
	// ie each variable is assigned a number.
	// A lower number means the variable is higher up in the BDD.
	using order = subtree_unordered_map<node, int_t>;
	using ref = tau_bdd_ref<node>;
	using refs = std::vector<ref>;
	using term_handle = tau_term_bdd_handle;
	using term_handles = std::vector<term_handle>;
	using tbdd = tau_term_bdd<node>;
	using quants = std::vector<std::pair<tref, typename tbdd::Quantifier>>;

	// Create a BDD handle from a given ref
	explicit tau_term_bdd_handle(ref x);
	tau_term_bdd_handle(tref b, const bool _inv) : h(tbdd::geth(b)),
						       inv(_inv) {}
	tau_term_bdd_handle(htref _h, const bool _inv) : h(std::move(_h)),
							 inv(_inv) {}

	// Build a BDD handle from a Tau term
	static term_handle build(tref term, const order& o);
	// Convert the BDD handle to a Tau term
	tref to_tau_term(size_t term_type);

	term_handle bdd_and(term_handle other, const order& o);
	term_handle bdd_or(term_handle other, const order& o);
	term_handle bdd_not();
	static term_handle bdd_and_many(const term_handles& bdds, const order& o);
	static term_handle bdd_or_many(const term_handles& bdds, const order& o);
	term_handle bdd_ex(const trefs& v, const order& o);
	term_handle bdd_all(const trefs& v, const order& o);
	term_handle bdd_quant(const quants& q, const order& o);

	ref get();

	htref h;
	bool inv = false;
};

template<NodeType node>
using term_handle = tau_term_bdd_handle<node>;

} // namespace idni::tau_lang

template<typename T>
struct std::hash<idni::tau_lang::tau_bdd_node<T>> {
	size_t operator()(auto& n) const;
};

template<typename T>
struct std::hash<idni::tau_lang::tau_bdd_ref<T>> {
	size_t operator()(auto& r) const;
};

#include "tau_bdd.tmpl.h"

#endif //TAU_TAU_BDD_H
