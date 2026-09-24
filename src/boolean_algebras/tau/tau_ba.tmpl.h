// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/tau/tau_ba.h"

#include <mutex>
#include <unordered_map>

#include "tau_spec.h"
#include "tau_diagnostics.h"
#include "reset_hooks.h"

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <deque>
#include <iterator>

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "tau_ba"

namespace idni::tau_lang {

namespace detail {

// Process-lifetime memo for nested Tau-SAT decisions on immutable tau
// constants: is_zero/is_one/normalize_tau each run a full nested Tau-SAT
// decision on the constant's `nso_rr`, and trees are hash-consed so caching
// by rr<node> identity turns a repeat decision into a lookup. Tau formulas
// are immutable, so no invalidation is needed; guarded by a mutex since
// concurrent solve/interpreter use isn't ruled out.
template <typename node>
struct tau_decision_cache {
	static std::mutex& mtx() {
		static std::mutex m;
		return m;
	}
	static std::unordered_map<rr<node>, bool>& is_zero_memo() {
		static std::unordered_map<rr<node>, bool> m;
		return m;
	}
	static std::unordered_map<rr<node>, bool>& is_one_memo() {
		static std::unordered_map<rr<node>, bool> m;
		return m;
	}
	static std::unordered_map<rr<node>, rr<node>>& normalize_memo() {
		static std::unordered_map<rr<node>, rr<node>> m;
		return m;
	}
	static std::unordered_map<rr<node>, tref>& splitter_normalize_memo() {
		static std::unordered_map<rr<node>, tref> m;
		return m;
	}
};

} // namespace detail

// Main formula of `fm` with its temporal quantifiers normalized;
// normalize_scopes=false leaves the formulas below the temporal
// quantifiers as they are. Used by ~, &, |, ^ below so newly combined
// mains stay in a comparable form; the rec relations are left untouched.
template <typename... BAs>
requires BAsPack<BAs...>
static result<tref> normalized_tau_ba_main(const tau_ba<BAs...>& fm) {
	using node = typename tau_ba<BAs...>::node;
	// Memoised per main tree: every Boolean operation on constants
	// (~, &, |, +) normalises the temporal layer of its operands, and the
	// same constants are operands over and over. Same key discipline as
	// cached_tau_ba_predicate: the main tree identifies the element only
	// when it carries no recurrence relations.
	result<tref> r;
	if (!fm.nso_rr.rec_relations.empty()) {
		tref main = fm.nso_rr.main->get();
		TAU_TRY(tref normalized,
			(normalize_temporal_quantifiers<node, false>(main)));
		return r.with_value(normalized);
	}
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	tref key = fm.nso_rr.main->get();
	if (auto it = cache.find(key); it != cache.end())
		return r.with_value(it->second);
	// compute before emplace: normalisation can create new trees, and a
	// rehash of `cache` must not happen with a half-built entry in it.
	TAU_TRY(tref res, (normalize_temporal_quantifiers<node, false>(key)));
	return r.with_value(cache.insert_or_assign(key, res).first->second);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba(const rewriter::rules& rec_relations, htref main)
		: nso_rr({ rec_relations, main }) {}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba(const rewriter::rules& rec_relations, tref main)
		: nso_rr({ rec_relations, tau::geth(main) }) {}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba(htref main) : nso_rr({ main }) {}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba(tref main) : nso_rr({ tau::geth(main) }) {}

template<typename ... BAs> requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba() : nso_rr() {}

template <typename... BAs>
requires BAsPack<BAs...>
auto tau_ba<BAs...>::operator<=>(const tau_ba<BAs...>&) const = default;

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator~() const {
	// Push the negation in at the end in order to keep normalized forms
	// after double negation of formulas
	tref main = nso_rr.main->get();
	// The Boolean operator contract is total: a failed normalization
	// falls back to the unnormalized main by definition, not as a
	// dropped report.
	auto nmain = tau::geth(to_nnf<node>(tau::build_wff_neg(
		normalized_tau_ba_main(*this).value_or(main))));
	auto nrec_relations = nso_rr.rec_relations;
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator&(const tau_ba<BAs...>& other) const {
	// The Boolean operator contract is total: a failed normalization
	// falls back to the unnormalized main by definition, not as a
	// dropped report.
	auto lhs = normalized_tau_ba_main(*this).value_or(nso_rr.main->get());
	auto rhs = normalized_tau_ba_main(other).value_or(other.nso_rr.main->get());
	auto nmain = tau::geth(tau::build_wff_and(lhs, rhs));
	auto nrec_relations =
		rewriter::merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator|(const tau_ba<BAs...>& other) const {
	// The Boolean operator contract is total: a failed normalization
	// falls back to the unnormalized main by definition, not as a
	// dropped report.
	auto lhs = normalized_tau_ba_main(*this).value_or(nso_rr.main->get());
	auto rhs = normalized_tau_ba_main(other).value_or(other.nso_rr.main->get());
	auto nmain = tau::geth(tau::build_wff_or(lhs, rhs));
	auto nrec_relations = rewriter::merge(nso_rr.rec_relations,
					      other.nso_rr.rec_relations);
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator+(const tau_ba<BAs...>& other) const {
	// The Boolean operator contract is total: a failed normalization
	// falls back to the unnormalized main by definition, not as a
	// dropped report.
	auto lhs = normalized_tau_ba_main(*this).value_or(nso_rr.main->get());
	auto rhs = normalized_tau_ba_main(other).value_or(other.nso_rr.main->get());
	auto nmain = tau::geth(tau::build_wff_xor(lhs, rhs));
	rewriter::rules nrec_relations = rewriter::merge(nso_rr.rec_relations,
						other.nso_rr.rec_relations);
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator^(const tau_ba<BAs...>& other) const {
	return *this + other;
}

/**
 * @internal
 * @brief Memoise a Tau-BA constant/valid test over the element's main tree.
 *
 * `is_zero`/`is_one` are what every layer above probes a Tau-BA leaf with --
 * `nso_ba`'s `operator==(tree, bool)` routes through `node::ba::is_zero/is_one`,
 * so each BDD node reduction over Tau-BA content asks the question at least
 * once. Unlike every other BA in the pack, answering it here costs a full
 * temporal decision procedure (`normalizer` re-normalizes and re-applies the
 * recurrence relations, then `is_tau_formula_sat`/`is_tau_impl` unroll the spec
 * to its unbounded continuation). Quantifier elimination over Tau-BA content
 * asks it tens of thousands of times about a handful of distinct elements, so
 * without memoisation a single elimination step runs for hours. Measured on a
 * `run` over nested conditionals with `:tau` streams: 35843 `is_one` and 35892
 * `is_zero` calls in the first two minutes over *three* distinct elements,
 * driving 71735 `transform_to_execution` and 215590 `normalize` calls; the memo
 * takes those to 11 and 26.
 *
 * This is a memo over an unchanged predicate, so no test pins it directly --
 * the existing tau_ba suites cover the answers and the cost is not something a
 * suite can assert without becoming a timing test.
 *
 * Deliberately NOT under `#ifdef TAU_CACHE`: this keeps the algorithm out of a
 * pathological regime rather than shaving a constant factor, and Debug builds
 * (where `TAU_CACHE` is off) run the same specs and the same tests.
 *
 * Registered through `tree<node>`'s GC-aware cache registry, so entries whose
 * key does not survive a sweep are dropped. Keyed by the element's main tree,
 * which is only a complete identity when the element carries no recurrence
 * relations -- `rewriter::rules` is not a tref-shaped key, and `normalizer`
 * folds those rules into the answer. Elements that carry them are therefore
 * computed uncached (correct, just as slow as before).
 * @endinternal
 */
// Keeps the trees behind the most recent decided rows alive across the
// interpreter's per-step sweep, so a decision made for a constant at one
// step is found again at the next (GitHub #92). The caches themselves are
// registered with the GC and drop any row whose key does not survive; a
// pinned key survives. Bounded: the oldest pin is released first once
// `ba_decision_pins` handles are held, and 0 disables the pinning.
template <typename node>
static void pin_decided_key(tref key) {
	static std::deque<htref> pins;
	if (ba_decision_pins == 0) return;
	static const bool reset_registered =
		(on_reset([] { pins.clear(); }), true);
	(void)reset_registered;
	pins.push_back(tree<node>::geth(key));
	while (pins.size() > ba_decision_pins) pins.pop_front();
}

// Mode of `ba_normalized_memo` (tau_ba.h): the environment variable
// TAU_BA_NORMALIZED_MEMO, read once, overrides the flag.
inline int ba_normalized_memo_mode() {
	static const std::optional<int> env = []() -> std::optional<int> {
		const char* v = std::getenv("TAU_BA_NORMALIZED_MEMO");
		if (!v || !*v) return std::nullopt;
		return v[0] == '2' ? 2 : v[0] == '1' ? 1 : 0;
	}();
	// When selected through the environment variable, the shadow mode
	// reports its counts once, at exit, so a whole run can be checked for
	// mains the normalizer would have changed.
	static const bool report = env && *env == 2 && std::atexit([]() {
		std::fprintf(stderr, "tau_ba normalized memo shadow: hits %zu,"
			" mismatches %zu\n", tau_ba_normalized_memo_hits,
			tau_ba_normalized_memo_mismatches);
	}) == 0;
	(void) report;
	return env ? *env : ba_normalized_memo;
}

inline int ba_normalized_conjunction_mode() {
	static const std::optional<int> env = []() -> std::optional<int> {
		const char* v = std::getenv("TAU_BA_NORMALIZED_CONJUNCTION");
		if (!v || !*v) return std::nullopt;
		return v[0] == '2' ? 2 : v[0] == '1' ? 1 : 0;
	}();
	static const bool report = env && *env == 2 && std::atexit([]() {
		std::fprintf(stderr, "tau_ba normalized conjunction shadow:"
			" hits %zu (shaped %zu), mismatches %zu\n",
			tau_ba_normalized_conjunction_hits,
			tau_ba_normalized_conjunction_shaped,
			tau_ba_normalized_conjunction_mismatches);
	}) == 0;
	(void) report;
	return env ? *env : ba_normalized_conjunction;
}

// Mains that `normalize_tau` returned. Registered with the GC, so a main
// that does not survive a sweep is forgotten with its tree. External
// linkage on purpose: `normalize_tau` and the decision can be instantiated
// in different translation units, and both must see the one cache.
template <typename node>
subtree_unordered_map<node, bool>& normalized_mains() {
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	return cache;
}

template <typename node>
bool is_normalized_main(tref main) {
	return ba_normalized_memo_mode() > 0
		&& normalized_mains<node>().find(main)
			!= normalized_mains<node>().end();
}

template <typename... BAs>
requires BAsPack<BAs...>
static result<bool> cached_tau_ba_predicate(const tau_ba<BAs...>& fm,
	subtree_unordered_map<typename tau_ba<BAs...>::node, bool>& cache,
	auto&& compute)
{
	using node = typename tau_ba<BAs...>::node;
	result<bool> r;
	if (!fm.nso_rr.rec_relations.empty()) {
		auto normalized = r.merge_take(normalizer<node>(fm.nso_rr));
		if (!normalized) return r;
		auto res = r.merge_take(compute(*normalized));
		if (!res) return r;
		return r.with_value(*res);
	}
	tref key = fm.nso_rr.main->get();
	if (auto it = cache.find(key); it != cache.end())
		return r.with_value(it->second);
	std::optional<tref> normalized;
	if (is_normalized_main<node>(key)) {
		// The main is a normal form `normalize_tau` returned, which the
		// normalizer maps to itself; deciding it directly saves the
		// renormalization of the whole constant. Correct regardless of
		// tree identity: `compute` decides any well-formed formula and
		// the main is equivalent to its renormalization; the identity is
		// what the shadow mode checks.
		if (ba_normalized_memo_mode() == 2) {
			normalized = r.merge_take(normalizer<node>(fm.nso_rr));
			if (!normalized) return r;
			if (*normalized != key) ++tau_ba_normalized_memo_mismatches;
		} else normalized = key;
		++tau_ba_normalized_memo_hits;
	} else {
		normalized = r.merge_take(normalizer<node>(fm.nso_rr));
		if (!normalized) return r;
	}
	// compute() before emplace: it can create new trees, and a rehash of
	// `cache` must not happen with a half-built entry in it.
	++tau_ba_predicate_misses;
	auto res = r.merge_take(compute(*normalized));
	if (!res) return r;
	pin_decided_key<node>(key);
	return r.with_value(cache.insert_or_assign(key, *res).first->second);
}

/**
 * @internal
 * @brief Per-support-component sat/valid decision for a `:tau` constant.
 *
 * `is_zero`/`is_one` decide a constant by running the full temporal decision
 * procedure over its formula. When that formula is a conjunction of units
 * with pairwise disjoint free supports -- the shape a constant takes when
 * independent clauses accumulate into it -- every question pays for all
 * units, although the answer factors: a model of each unit assigns only its
 * own variables, so models over disjoint supports compose, and validity
 * distributes over conjunction. `factored_tau_sat`/`factored_tau_valid`
 * decide per unit group and cache the verdicts per group (the `create_cache`
 * discipline of `cached_tau_ba_predicate`, compute before emplace), so a
 * constant that grows by one clause pays for that clause.
 *
 * Supports are compared by variable NAME, not by variable node: `o1[t]`,
 * `o1[t-1]` and `o1[0]` are one stream and must land in one group (a
 * node-identity grouping such as `group_by_shared_vars` would keep them
 * apart); the time offset variable is not part of the support
 * (`get_free_vars` does not descend into io variables). Bound-variable names
 * are included, which can only merge groups, never split them.
 *
 * Conservative gates, each falling back to the monolithic path: any embedded
 * BA constant inside a unit (its support is invisible from the outside), any
 * free variable without a printable name, fewer than two groups. A single
 * `always` hull is split into per-unit hulls first (`always` distributes
 * over conjunction). Both decisions are taken at start time 0, which is the
 * only start time the callers use.
 * @endinternal
 */
template <typename node>
static int factored_tau_units(tref fm, trefs& units) {
	using tau = tree<node>;
	trefs clauses = get_cnf_wff_clauses<node>(fm);
	for (size_t i = 0; i < clauses.size(); ++i) {
		if (tau::get(clauses[i]).child_is(tau::wff_always)) {
			trefs aw = get_cnf_wff_clauses<node>(
				tau::trim2(clauses[i]));
			clauses[i] = tau::build_wff_always(aw[0]);
			for (size_t j = 1; j < aw.size(); ++j)
				clauses.push_back(
					tau::build_wff_always(aw[j]));
		}
	}
	if (clauses.size() < 2) return -1;
	units = std::move(clauses);
	return 0;
}

// Whether component factoring of is_zero/is_one is on: true if the
// `ba_component_factoring` API flag (tau_ba.h) is set, or the environment
// variable TAU_BA_COMPONENT_FACTORING is set to a non-empty value other
// than exactly "0". The environment is read once and latched for the
// lifetime of the process; the API flag is re-read on every call.
inline bool ba_component_factoring_enabled() {
	static const std::optional<bool> env = []() -> std::optional<bool> {
		const char* v = std::getenv("TAU_BA_COMPONENT_FACTORING");
		if (!v || !*v) return std::nullopt;
		return !(v[0] == '0' && v[1] == '\0');
	}();
	return env ? *env : ba_component_factoring;
}

template <typename node>
static int factored_tau_valid(tref fm);

/**
 * @brief The dual of a `sometimes` formula: `G(!D)` for `F(D)`, in NNF.
 *
 * A complemented `:tau` constant `{K}'` whose body is an always-conjunction
 * normalizes to a `sometimes` over the DNF of `!K`. Neither factored path
 * can split that shape (the units are always-clauses); `F(D) == !G(!D)`
 * maps both questions back to the always/CNF units of `K`, which the
 * per-unit caches already hold: `sat(F(D)) == !valid(G(!D))` and
 * `valid(F(D)) == !sat(G(!D))`. Both identities are the engine's own
 * definitions of the two predicates (`valid(X)` is decided as `!sat(!X)`),
 * so the dual adds no assumption beyond the per-unit factoring.
 *
 * Returns nullptr when @p fm is not a `sometimes` formula, or when its body
 * holds a temporal operator of its own: the dual of a nested temporal body
 * is not an always-conjunction the unit split could take apart, and pushing
 * the negation through the full-LTL operators is the LTL pipeline's job.
 * The callers pass the normalized main, where the complement of an
 * always-conjunction is one `sometimes` over a DNF; a disjunction of
 * several `sometimes`, as `to_nnf` alone produces, is not taken apart here
 * and goes to the units path.
 */
template <typename node>
static tref sometimes_dual(tref fm) {
	using tau = tree<node>;
	const tau& t = tau::get(fm);
	if (!t.has_child() || !t.child_is(tau::wff_sometimes)) return nullptr;
	const tref body = tau::trim2(fm);
	if (tau::get(body).find_top(is_temporal_quantifier<node>)) return nullptr;
	return to_nnf<node>(tau::build_wff_always(tau::build_wff_neg(body)));
}

// Component-wise satisfiability; -1 = not applicable (fall back), 0 = unsat,
// 1 = sat.
template <typename node>
static int factored_tau_sat(tref fm) {
	using tau = tree<node>;
	// sat(F(D)) == !valid(G(!D)); see sometimes_dual
	if (tref dual = sometimes_dual<node>(fm); dual)
		if (int r = factored_tau_valid<node>(dual); r >= 0)
			return r == 1 ? 0 : 1;
	trefs units;
	if (factored_tau_units<node>(fm, units) < 0) return -1;
	for (tref u : units)
		if (tau::get(u).find_top([](tref t) {
			return tree<node>::get(t).is_ba_constant(); }))
			return -1;
	std::vector<std::vector<std::string>> supp(units.size());
	for (size_t i = 0; i < units.size(); ++i)
		for (tref v : tau::get(units[i]).get_free_vars()) {
			const std::string& nm = get_var_name<node>(v);
			if (nm.empty()) return -1;
			supp[i].push_back(nm);
		}
	std::vector<std::vector<std::string>> cn;
	std::vector<trefs> cc;
	auto shares = [](const std::vector<std::string>& a,
			 const std::vector<std::string>& b) {
		for (const auto& x : a) for (const auto& y : b)
			if (x == y) return true;
		return false;
	};
	for (size_t i = 0; i < units.size(); ++i) {
		std::vector<size_t> hit;
		for (size_t c = 0; c < cn.size(); ++c)
			if (shares(cn[c], supp[i])) hit.push_back(c);
		if (hit.empty()) {
			cn.push_back(supp[i]);
			cc.push_back(trefs{ units[i] });
			continue;
		}
		size_t base = hit[0];
		cn[base].insert(cn[base].end(),
			supp[i].begin(), supp[i].end());
		cc[base].push_back(units[i]);
		for (size_t k = hit.size(); k-- > 1; ) {
			size_t c = hit[k];
			cn[base].insert(cn[base].end(),
				cn[c].begin(), cn[c].end());
			cc[base].insert(cc[base].end(),
				cc[c].begin(), cc[c].end());
			cn.erase(cn.begin() + c);
			cc.erase(cc.begin() + c);
		}
	}
	if (cc.size() < 2) return -1;
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	bool all_sat = true;
	for (size_t c = 0; c < cc.size() && all_sat; ++c) {
		tref f = cc[c][0];
		for (size_t j = 1; j < cc[c].size(); ++j)
			f = tau::build_wff_and(f, cc[c][j]);
		if (auto it = cache.find(f); it != cache.end()) {
			all_sat = it->second;
			continue;
		}
		// compute() before emplace: it can create new trees, and a
		// rehash of `cache` must not happen with a half-built entry.
		auto sat = is_tau_formula_sat<node>(f);
		bool sres = sat.has_value() && sat.value();
		pin_decided_key<node>(f);
		cache.insert_or_assign(f, sres);
		all_sat = sres;
	}
	return all_sat ? 1 : 0;
}

// Unit-wise validity (distributes over conjunction unconditionally);
// -1 = not applicable, 0 = not valid, 1 = valid.
template <typename node>
static int factored_tau_valid(tref fm) {
	using tau = tree<node>;
	// valid(F(D)) == !sat(G(!D)); see sometimes_dual
	if (tref dual = sometimes_dual<node>(fm); dual)
		if (int r = factored_tau_sat<node>(dual); r >= 0)
			return r == 1 ? 0 : 1;
	trefs units;
	if (factored_tau_units<node>(fm, units) < 0) return -1;
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	bool all = true;
	for (size_t i = 0; i < units.size() && all; ++i) {
		if (auto it = cache.find(units[i]); it != cache.end()) {
			all = it->second;
			continue;
		}
		auto imp = is_tau_impl<node>(tau::_T(), units[i]);
		bool vres = imp.has_value() && imp.value();
		pin_decided_key<node>(units[i]);
		cache.insert_or_assign(units[i], vres);
		all = vres;
	}
	return all ? 1 : 0;
}


// The solver's bad splitter of a Tau constant is a fresh uninterpreted
// constant `<:splitN> != 0` (tau_splitter_one calls tau_bad_splitter on
// `T`, so the element is that bare formula), and the properness checks of
// the step solver probe the element with is_zero and is_one -- the second
// being the zero test of its complement -- before they commit a witness.
// Both answers follow from the shape alone: an uninterpreted constant
// `!= 0` is satisfiable (c := 1) and not valid (c := 0), and so is `= 0`.
// Answering them here keeps the solver's checks and saves a full temporal
// decision per probe -- two per minted constant, and the step solver mints
// a fresh one on every step.
template <typename... BAs>
requires BAsPack<BAs...>
static bool is_uconst_zero_test(const tau_ba<BAs...>& fm) {
	using node = typename tau_ba<BAs...>::node;
	using tau = tree<node>;
	if (!fm.nso_rr.rec_relations.empty() || !fm.nso_rr.main) return false;
	const tau& w = tau::get(fm.nso_rr.main->get());
	if (!(w.child_is(tau::bf_neq) || w.child_is(tau::bf_eq))) return false;
	tref l = w[0].first(), r = w[0].second();
	if (!l || !r || !tau::get(r).equals_0()) return false;
	// l must be exactly bf(variable(uconst_name)), the shape
	// build_bf_uconst makes: one uninterpreted constant and nothing else,
	// checked positively so that no operator, stream or constant around it
	// passes.
	const tau& tl = tau::get(l);
	return tl.is(tau::bf) && tl.child_is(tau::variable)
		&& tl[0].child_is(tau::uconst_name);
}

template <typename... BAs>
requires BAsPack<BAs...>
result<bool> tau_ba<BAs...>::is_zero() const {
	if (is_uconst_zero_test(*this)) return result<bool>{false};
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	return cached_tau_ba_predicate(*this, cache,
		[](tref normalized) -> result<bool> {
			if (ba_component_factoring_enabled())
				if (int r = factored_tau_sat<node>(normalized);
						r >= 0)
					return result<bool>{r == 0};
			return is_tau_formula_sat<node>(normalized)
				.transform([](bool sat) { return !sat; });
		});
}

template <typename... BAs>
requires BAsPack<BAs...>
result<bool> tau_ba<BAs...>::is_one() const {
	if (is_uconst_zero_test(*this)) return result<bool>{false};
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	return cached_tau_ba_predicate(*this, cache,
		[](tref normalized) -> result<bool> {
			if (ba_component_factoring_enabled())
				if (int r = factored_tau_valid<node>(normalized);
						r >= 0)
					return result<bool>{r == 1};
			return is_tau_impl<node>(tau::_T(), normalized);
		});
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const tau_ba<BAs...>& other, const bool& b) {
	// An undecidable is_one()/is_zero() falls to the side that never
	// misreports a witness: not one, and (unless proven otherwise) zero.
	return b ? other.is_one().value_or(false)
		 : other.is_zero().value_or(true);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const bool& b, const tau_ba<BAs...>& other) {
	return other == b;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const tau_ba<BAs...>& lhs, const tau_ba<BAs...>& rhs) {
	return lhs.nso_rr.main == rhs.nso_rr.main &&
		lhs.nso_rr.rec_relations == rhs.nso_rr.rec_relations;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const tau_ba<BAs...>& other, const bool& b) {
	return !(other == b);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const bool& b, const tau_ba<BAs...>& other) {
	return !(other == b);
}

// The Boole normal form of a conjunction of clauses is a conjunction of two
// left-nested chains: the unit clauses (an equality or inequality each),
// ordered by the comparator the syntactic path simplification sorts its
// assumptions with, applied to their positive atoms (an inequality reaches
// that sort as the negation of its equality), and the clauses that are
// disjunctions, in traversal order of the input. The shape comes from syntactic_formula_simplification,
// the first and the last step of term_boole_normal_form:
// simplify_using_equality flattens the conjunction, stable-sorts the
// equalities to its front (simplify_using_equality_sort_atms; a disjunction
// keeps its place among the disjunctions) and left-folds it, and
// syntactic_path_simplification collects the unit clauses of a conjunction
// as assumptions (disjunctions are skipped), sorts them with
// syntactic_path_simplification_wff_comp (equalities first, then
// subtree_less), left-folds them and conjoins them in front of the
// remainder, whose replaced atoms the hook rules `T && $X ::= $X` and
// `$X && T ::= $X` remove; the steps in between leave that arrangement in
// place, and the last pass fixes it. A new clause conjoined in
// front of such a normal form therefore lands at the inner end of the
// clause chain (the traversal lists it first, and a left fold nests the
// first element innermost), one conjoined behind it at the outer end, and
// a new unit at its place in the unit chain, while every other node stays
// as it is. `shaped_conjunction` builds that result from the two bodies
// without running the normalization: `big` is the body of the normal form,
// `one` the body of the single clause, `front` says whether the clause is
// the first operand. The result is a conjunction of the same clauses in
// another arrangement, so it is equivalent to the input by construction;
// that it is the same tree the normalization returns is measured (the
// shadow mode compares the two). nullptr when `big` has another shape,
// when `one` is neither a unit nor a disjunction, when a body mentions a
// variable that is not a stream, or when `one` mentions a stream
// occurrence `big` already mentions (the syntactic passes simplify clauses
// on a shared occurrence against each other, so the shape alone does not
// give the result there).
//
// Forward declaration: heuristics/syntactic_path_simplification.tmpl.h,
// which defines the comparator, is included after this header.
template <NodeType node>
bool syntactic_path_simplification_wff_comp(tref l, tref r);

template <typename node>
static tref shaped_conjunction(tref big, tref one, bool front) {
	using tau = tree<node>;
	auto is_and = [](tref n) {
		const tau& t = tau::get(n);
		return t.has_child() && t.child_is(tau::wff_and); };
	auto is_or = [](tref n) {
		const tau& t = tau::get(n);
		return t.has_child() && t.child_is(tau::wff_or); };
	auto is_unit = [](tref n) {
		const tau& t = tau::get(n);
		return t.has_child()
			&& (t.child_is(tau::bf_eq) || t.child_is(tau::bf_neq)); };
	// elements of a left-nested chain ((a && b) && c) -> [a, b, c]
	auto spine = [&](tref n) {
		trefs out;
		while (is_and(n)) {
			out.push_back(tau::get(n)[0].second());
			n = tau::get(n)[0].first();
		}
		out.push_back(n);
		std::reverse(out.begin(), out.end());
		return out;
	};
	auto chain = [](const trefs& v) {
		tref acc = v[0];
		for (size_t i = 1; i < v.size(); ++i)
			acc = tau::build_wff_and(acc, v[i]);
		return acc;
	};
	// A body with both kinds is AND(unit chain, clause chain), whose left
	// spine ends in the clause chain as one element (a unit is never a
	// conjunction); a body with one kind is that chain itself.
	trefs units, clauses, el = spine(big);
	if (el.size() >= 2 && is_and(el.back())) {
		clauses = spine(el.back());
		el.pop_back();
		units = el;
	} else {
		size_t i = 0;
		while (i < el.size() && is_unit(el[i])) units.push_back(el[i++]);
		while (i < el.size()) clauses.push_back(el[i++]);
	}
	for (tref u : units) if (!is_unit(u)) return nullptr;
	for (tref c : clauses) if (!is_or(c)) return nullptr;
	// The stream occurrences a body mentions (`name[index]`), by printed
	// text without the type: the same stream can occur as differently
	// typed variable nodes, which the hash-consing keeps apart, and a
	// different time offset is a different atom to the syntactic passes.
	// A variable that is neither a stream occurrence nor the time index
	// of one (a free non-stream variable, which simplify_using_equality
	// can relate to others) leaves the body outside the shape, so it is
	// reported as such. Kept per body in a GC-registered cache, so the body a step
	// returns answers the next step's lookup, and the merge is one pass
	// over two sorted lists.
	using streams_t = std::vector<std::string>;
	using streams_cache_t = subtree_unordered_map<node, streams_t>;
	static streams_cache_t& streams_of =
		tree<node>::template create_cache<streams_cache_t>();
	bool other_variable = false;
	auto stream_names = [&other_variable](tref f) {
		streams_t names;
		subtree_set<node> in_stream;
		for (tref v : tau::get(f).select_all(is<node, tau::variable>))
			if (is_io_var<node>(v))
				for (tref w : tau::get(v).select_all(is<node, tau::variable>))
					in_stream.insert(w);
		for (tref v : tau::get(f).select_all(is<node, tau::variable>)) {
			if (is_io_var<node>(v)) {
				std::string s = tau::get(v).to_str();
				names.push_back(s.substr(0, s.find(':')));
			} else if (!in_stream.contains(v)) other_variable = true;
		}
		std::sort(names.begin(), names.end());
		names.erase(std::unique(names.begin(), names.end()), names.end());
		return names;
	};
	auto it = streams_of.find(big);
	if (it == streams_of.end())
		it = streams_of.emplace(big, stream_names(big)).first;
	const streams_t& have = it->second;
	streams_t add = stream_names(one);
	if (other_variable) return nullptr;
	// A clause on a stream the body already mentions is not assembled:
	// the normalization simplifies such clauses against each other (a
	// unit against a clause or another unit on the same stream, and
	// resolvable clauses), so the shape alone does not give the result.
	for (const auto& s : add)
		if (std::binary_search(have.begin(), have.end(), s)) return nullptr;
	if (is_unit(one)) {
		// The path simplification keys an inequality by its positive
		// atom (`l != r` reaches it as `!(l = r)` and the sort compares
		// the trimmed keys), so a unit's place is that of the equality
		// over the same terms; with every key an equality the comparator
		// reduces to subtree_less over the keys. The position a stable
		// sort by the comparator gives: ties under it are equal keys,
		// which the stream check has already excluded.
		auto key = [](tref u) {
			const tau& t = tau::get(u);
			if (!t.child_is(tau::bf_neq)) return u;
			return tau::build_bf_eq(t[0].first(), t[0].second());
		};
		const tref k = key(one);
		auto pos = std::lower_bound(units.begin(), units.end(), k,
			[&key](tref a, tref kb) {
				return syntactic_path_simplification_wff_comp<node>(
					key(a), kb); });
		units.insert(pos, one);
	} else if (is_or(one)) {
		if (front) clauses.insert(clauses.begin(), one);
		else clauses.push_back(one);
	} else return nullptr;
	streams_t merged;
	merged.reserve(have.size() + add.size());
	std::merge(have.begin(), have.end(), add.begin(), add.end(),
		std::back_inserter(merged));
	tref out = units.empty() ? chain(clauses)
		: clauses.empty() ? chain(units)
		: tau::build_wff_and(chain(units), chain(clauses));
	streams_of.insert_or_assign(out, std::move(merged));
	++tau_ba_normalized_conjunction_shaped;
	return out;
}

// The normal form of `a && b` built from the normal forms of its sides
// (ba_normalized_conjunction): the always-hull over the Boole normal form
// of the two bodies conjoined, in operand order. That is the tree the
// pipeline returns for such a main: normalize_temporal_quantifiers
// squeezes the always-hulls into one and applies term_boole_normal_form
// to its scope, and the passes before it (quantifier and arithmetic
// elimination, widening) map a body that is already their output to
// itself. A side is a normal form
// `normalize_tau` returned or a single clause (an always-hull), which is
// normalized on its own first; a side that is itself a conjunction is not
// normalized here, so a constant that arrives as one large conjunction
// keeps the pipeline.
// nullptr when the main or a side has another shape, or when a body holds
// a quantifier or a temporal operator (those the pipeline eliminates or
// scopes).
template <typename... BAs>
requires BAsPack<BAs...>
static tref normalized_conjunction(tref main) {
	using node = typename tau_ba<BAs...>::node;
	using tau = tree<node>;
	const tau& m = tau::get(main);
	if (!m.has_child() || !m.child_is(tau::wff_and)) return nullptr;
	auto is_hull = [](tref f) {
		const tau& t = tau::get(f);
		return t.has_child() && t.child_is(tau::wff_always);
	};
	auto normal_form = [&](tref side) -> tref {
		if (is_normalized_main<node>(side)) return side;
		if (!is_hull(side)) return nullptr;
		return normalize_tau(tau_ba<BAs...>(side)).nso_rr.main->get();
	};
	auto body = [&](tref nf) -> tref {
		if (!nf || !is_hull(nf)) return nullptr;
		tref b = tau::trim2(nf);
		if (tau::get(b).find_top([](tref n) {
			return is_quantifier<node>(n)
				|| is_child_temporal_quantifier<node>(n); }))
			return nullptr;
		return b;
	};
	tref l = body(normal_form(m[0].first()));
	if (!l) return nullptr;
	tref r = body(normal_form(m[0].second()));
	if (!r) return nullptr;
	// one side a single clause (no conjunction at its top): the shape of
	// the other gives the result
	auto single = [](tref f) {
		const tau& t = tau::get(f);
		return !t.has_child() || !t.child_is(tau::wff_and); };
	tref shaped = single(l) ? shaped_conjunction<node>(r, l, true)
		: single(r) ? shaped_conjunction<node>(l, r, false) : nullptr;
	if (shaped) {
		++tau_ba_normalized_conjunction_hits;
		return tau::build_wff_always(shaped);
	}
	auto bnf = term_boole_normal_form<node>(tau::build_wff_and(l, r));
	if (!bnf.has_value()) return nullptr;
	++tau_ba_normalized_conjunction_hits;
	return tau::build_wff_always(bnf.value());
}

// Normalizes a tau_ba constant: applies its rec relations to the main
// formula (nso_rr_apply) and simplifies unsat/valid subformulas. The
// result carries the normalized main only — the rec relations, already
// applied, are not copied into the returned tau_ba.
template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> normalize_tau(const tau_ba<BAs...>& fm) {
	using node = typename tau_ba<BAs...>::node;
	using cache = detail::tau_decision_cache<node>;
	{
		std::lock_guard<std::mutex> lock(cache::mtx());
		auto& memo = cache::normalize_memo();
		if (auto it = memo.find(fm.nso_rr); it != memo.end()) {
			normalized_mains<node>().insert_or_assign(
				it->second.main->get(), true);
			return tau_ba<BAs...>(it->second.rec_relations, it->second.main);
		}
	}
	if (fm.nso_rr.rec_relations.empty()
		&& is_normalized_main<node>(fm.nso_rr.main->get()))
	{
		// A normal form this function returned earlier: normalizing it
		// again yields the same main.
		++tau_ba_normalized_memo_hits;
		if (ba_normalized_memo_mode() != 2) return fm;
		auto applied = nso_rr_apply<node>(fm.nso_rr);
		if (!applied.has_value()) return fm;
		auto simplified = simp_tau_unsat_valid<node>(applied.value());
		if (!simplified.has_value()) return fm;
		if (simplified.value() != fm.nso_rr.main->get())
			++tau_ba_normalized_memo_mismatches;
		return fm;
	}
	// No safe normalized form exists on failure; return the element
	// unchanged, matching splitter()'s fallback below.
	auto applied = nso_rr_apply<node>(fm.nso_rr);
	if (!applied.has_value()) return fm;
	const int conjunction = fm.nso_rr.rec_relations.empty()
		? ba_normalized_conjunction_mode() : 0;
	tref built = conjunction > 0
		? normalized_conjunction<BAs...>(applied.value()) : nullptr;
	auto simplified = simp_tau_unsat_valid<node>(applied.value(), 0, false,
		conjunction == 1 ? built : nullptr);
	if (!simplified.has_value()) return fm;
	if (built && conjunction == 2) {
		// the shadow runs the unsat/valid simplification a second time,
		// on the built form, so it pays the validity check twice
		auto direct = simp_tau_unsat_valid<node>(applied.value(), 0,
			false, built);
		if (!direct.has_value() || direct.value() != simplified.value())
			++tau_ba_normalized_conjunction_mismatches;
	}
	tau_ba<BAs...> out(tree<node>::geth(simplified.value()));
	std::lock_guard<std::mutex> lock(cache::mtx());
	// A normal form computed while the bdd node table was exhausted may
	// be incomplete: neither memoized nor recorded as a fixed point.
	if (!bdd_node_table_exhausted) {
		normalized_mains<node>().insert_or_assign(simplified.value(), true);
		cache::normalize_memo().emplace(fm.nso_rr, out.nso_rr);
	}
	return out;
}

// Memoized normalizer<node>(nso_rr) for the splitter's normalized-formula
// precondition (ba_descriptor<tau_ba<...>>::splitter), called once per
// candidate inside atomless_choose_value's splitter ladder. Deliberately a
// separate cache from normalize_memo: normalizer() only normalizes, unlike
// normalize_tau's own simp_tau_unsat_valid pass, so the two aren't
// interchangeable.
template <typename node>
tref normalize_for_splitter(const rr<node>& nso_rr) {
	using cache = detail::tau_decision_cache<node>;
	{
		std::lock_guard<std::mutex> lock(cache::mtx());
		auto& memo = cache::splitter_normalize_memo();
		if (auto it = memo.find(nso_rr); it != memo.end())
			return it->second;
	}
	auto normalized = normalizer<node>(nso_rr);
	tref result = normalized.has_value() ? normalized.value() : nullptr;
	std::lock_guard<std::mutex> lock(cache::mtx());
	if (!bdd_node_table_exhausted)
		cache::splitter_normalize_memo().emplace(nso_rr, result);
	return result;
}

// Purely syntactic check: the main formula is literally T. No rec
// relations are applied and no satisfiability check runs — a semantically
// valid but non-literal main returns false (use is_one() for that).
template <typename... BAs>
requires BAsPack<BAs...>
bool is_tau_syntactic_one(const tau_ba<BAs...>& fm) {
	return tree<node<tau_ba<BAs...>, BAs...>>::get(fm.nso_rr.main).equals_T();
}

// Purely syntactic check: the main formula is literally F. No rec
// relations are applied and no satisfiability check runs — a semantically
// unsat but non-literal main returns false (use is_zero() for that).
template <typename... BAs>
requires BAsPack<BAs...>
bool is_tau_syntactic_zero(const tau_ba<BAs...>& fm) {
	return tree<node<tau_ba<BAs...>, BAs...>>::get(fm.nso_rr.main).equals_F();
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> splitter(const tau_ba<BAs...>& fm, splitter_type st) {
	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	auto normalized = normalizer<node>(fm.nso_rr);
	if (!normalized.has_value()) return fm;
	tref s = tau_splitter<tau_ba<BAs...>, BAs...>(normalized.value(), st);
	return tau_ba<BAs...>(tree<node>::geth(s));
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_splitter_one() {
	return tau_ba<BAs...>(tau_bad_splitter<tau_ba<BAs...>, BAs...>());
}

template <typename... BAs>
requires BAsPack<BAs...>
result<bool> is_tau_closed(const tau_ba<BAs...>& fm) {
	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	using tau = tree<node>;
	result<bool> r;
	auto applied = r.merge_take(nso_rr_apply<node>(fm.nso_rr));
	if (!applied) return r;
	tref simp_fm = apply_defs_to_spec<node>(*applied);
	// apply_defs_to_spec carries no report of its own (normalizer.tmpl.h);
	// its only failure signal is the null tree.
	if (!simp_fm)
		return r.with_error(code::internal_error,
			"Failed to apply definitions to spec");
	if (tau::get(simp_fm).find_top(is<node, tau::ref>))
		return r.with_value(false);
	const trefs& vars = get_free_vars<node>(simp_fm);
	for (tref v : vars) {
		const tau& t = tau::get(v);
		if (!(t.child_is(tau::io_var)
			|| t.child_is(tau::uconst_name)))
				return r.with_value(false);
	}
	return r.with_value(true);
}

template <typename... BAs>
requires BAsPack<BAs...>
result<typename node<tau_ba<BAs...>, BAs...>::constant_with_type>
	parse_tau(const std::string& src)
{
	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	result<typename node::constant_with_type> r;
	// parse source
	tau_spec<node> s;
	std::optional<rr<node>> maybe_nso_rr;
	if (!s.parse(src) || !(maybe_nso_rr = s.get_nso_rr())) {
		for (const auto& error : s.errors())
			r.error(code::parse_error, error);
		if (!r.has_error())
			r.error(code::parse_error, "Failed to parse tau constant");
		return r;
	}
	// compute final result
	return r.with_value(typename node::constant_with_type{
		std::variant<tau_ba<BAs...>, BAs...>(
			tau_ba<BAs...>(maybe_nso_rr.value().rec_relations,
				       maybe_nso_rr.value().main)),
		tau_type<node>() });
}

template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& operator<<(std::ostream& os, const tau_ba<BAs...>& rs) {
	return print<node<tau_ba<BAs...>, BAs...>>(os, rs.nso_rr);
}

} // namespace idni::tau_lang
