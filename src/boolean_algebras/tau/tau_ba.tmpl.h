// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/tau/tau_ba.h"

#include <mutex>
#include <unordered_map>

#include "tau_spec.h"
#include "tau_diagnostics.h"
#include "reset_hooks.h"

#include <cstdlib>
#include <deque>

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
	auto normalized = r.merge_take(normalizer<node>(fm.nso_rr));
	if (!normalized) return r;
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

// Component-wise satisfiability; -1 = not applicable (fall back), 0 = unsat,
// 1 = sat.
template <typename node>
static int factored_tau_sat(tref fm) {
	using tau = tree<node>;
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

template <typename... BAs>
requires BAsPack<BAs...>
result<bool> tau_ba<BAs...>::is_zero() const {
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
		if (auto it = memo.find(fm.nso_rr); it != memo.end())
			return tau_ba<BAs...>(it->second.rec_relations, it->second.main);
	}
	// No safe normalized form exists on failure; return the element
	// unchanged, matching splitter()'s fallback below.
	auto applied = nso_rr_apply<node>(fm.nso_rr);
	if (!applied.has_value()) return fm;
	auto simplified = simp_tau_unsat_valid<node>(applied.value());
	if (!simplified.has_value()) return fm;
	tau_ba<BAs...> out(tree<node>::geth(simplified.value()));
	std::lock_guard<std::mutex> lock(cache::mtx());
	if (!bdd_node_table_exhausted)
		cache::normalize_memo().emplace(fm.nso_rr, out.nso_rr);
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
