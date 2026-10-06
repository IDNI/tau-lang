// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/tau/tau_ba.h"

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

// Memos for the normalize passes on immutable tau constants: trees are
// hash-consed, so keying by the constant's main tref turns a repeat pass
// into a lookup, and a row lives as long as its key tree.
template <typename node>
struct tau_decision_cache {
	/// Memo of `normalize_tau`: main tree -> normalized main.
	static auto& normalize_memo() {
		using cache_t = subtree_unordered_map<node, tref>;
		static cache_t& m = tree<node>::template create_cache<cache_t>();
		return m;
	}
	/// Memo of `normalize_for_splitter`: main tree -> normalized formula.
	static auto& splitter_normalize_memo() {
		using cache_t = subtree_unordered_map<node, tref>;
		static cache_t& m = tree<node>::template create_cache<cache_t>();
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
	// TODO (HIGH) dropped error: the operand's normalization report -- operator~ returns a plain tau_ba, which cannot carry it.
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
	// TODO (HIGH) dropped error: the left operand's normalization report -- operator& returns a plain tau_ba, which cannot carry it.
	auto lhs = normalized_tau_ba_main(*this).value_or(nso_rr.main->get());
	// TODO (HIGH) dropped error: the right operand's normalization report -- operator& returns a plain tau_ba, which cannot carry it.
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
	// TODO (HIGH) dropped error: the left operand's normalization report -- operator| returns a plain tau_ba, which cannot carry it.
	auto lhs = normalized_tau_ba_main(*this).value_or(nso_rr.main->get());
	// TODO (HIGH) dropped error: the right operand's normalization report -- operator| returns a plain tau_ba, which cannot carry it.
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
	// TODO (HIGH) dropped error: the left operand's normalization report -- operator+ returns a plain tau_ba, which cannot carry it.
	auto lhs = normalized_tau_ba_main(*this).value_or(nso_rr.main->get());
	// TODO (HIGH) dropped error: the right operand's normalization report -- operator+ returns a plain tau_ba, which cannot carry it.
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

/*
 * Memoise a Tau-BA constant/valid test over the element's main tree.
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
 * computed uncached.
 */
/// Keeps the trees behind the most recent decided rows alive across the
/// interpreter's per-step sweep, so a decision made for a constant at one
/// step is found again at the next (GitHub #92). The caches themselves are
/// registered with the GC and drop any row whose key does not survive; a
/// pinned key survives. Bounded: the oldest pin is released first once
/// `ba_decision_pins` handles are held, and 0 disables the pinning.
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

/**
 * @internal
 * @brief Memoise a Tau-BA constant/valid test over the element's main tree
 * (the note above `pin_decided_key` gives the reasons and the key
 * discipline).
 * @param fm The element to decide.
 * @param cache The memo of this predicate, keyed by main tree; used only
 * when @p fm has no recurrence relations.
 * @param compute Decides the normalized formula: `result<bool>(tref)`.
 * @return The verdict, or the report of a failed normalization or decision
 * (a failure is not memoized).
 * @endinternal
 */
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

// Splits @p fm into its CNF clauses, an `always` clause further into one
// `always` per clause of its body. Returns 0 with the clauses in @p units,
// or -1 (leaving @p units untouched) when there are fewer than two.
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

/// Whether component factoring of is_zero/is_one is on. A non-empty
/// environment variable TAU_BA_COMPONENT_FACTORING decides alone (off for
/// exactly "0", on otherwise); unset or empty, the `ba_component_factoring`
/// flag (tau_ba.h) decides. The environment is read once and latched for the
/// lifetime of the process; the flag is re-read on every call.
inline bool ba_component_factoring_enabled() {
	static const std::optional<bool> env = []() -> std::optional<bool> {
		const char* v = std::getenv("TAU_BA_COMPONENT_FACTORING");
		if (!v || !*v) return std::nullopt;
		return !(v[0] == '0' && v[1] == '\0');
	}();
	return env ? *env : ba_component_factoring;
}

// In the body of an `always` unit: a constraint on the time point, a stream
// read at a fixed time point, or a temporal operator of its own.
template <typename node>
static bool at_absolute_time(tref t) {
	using tau = tree<node>;
	const auto& n = tau::get(t);
	return n.is(tau::constraint)
		|| (n.child_is(tau::io_var) && is_io_initial<node>(t))
		|| is_temporal_quantifier<node>(t);
}

/**
 * @brief Whether a unit refers to absolute time.
 *
 * The decision of a formula takes quantities from the formula as a whole,
 * among them the step from which its always part is enforced; a unit or a
 * group decided on its own takes them from itself. An `always` unit whose
 * streams are read at the current step and at steps back from it says the
 * same from every step, so its verdict does not depend on that step. A
 * `sometimes` clause, a stream read at a fixed time point and a constraint
 * on the time point refer to absolute time: their verdict can depend on
 * that step. Two kinds of unit are left to the decision of the whole
 * formula with them: a unit that is no `always` unit, and an `always` unit
 * whose body holds a temporal operator of its own, for which the decision
 * of the whole formula may take another procedure than the decision of a
 * group.
 */
template <typename node>
static bool refers_to_absolute_time(tref unit) {
	using tau = tree<node>;
	return !tau::get(unit).child_is(tau::wff_always)
		|| tau::get(tau::trim2(unit)).find_top(at_absolute_time<node>);
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
 * own variables, so models over disjoint supports compose where no unit
 * refers to absolute time (`refers_to_absolute_time`), and validity
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
 * free variable without a printable name, fewer than two groups, and any
 * unit that refers to absolute time (`refers_to_absolute_time`). A single
 * `always` hull is split into per-unit hulls first (`always` distributes
 * over conjunction). Both decisions are taken at start time 0, which is the
 * only start time the callers use.
 * @endinternal
 */
// Component-wise satisfiability; -1 = not applicable (fall back), 0 = unsat,
// 1 = sat. A group whose decision fails gives no value and its report; the
// failure is not cached.
template <typename node>
static result<int> factored_tau_sat(tref fm) {
	using tau = tree<node>;
	result<int> r;
	trefs units;
	if (factored_tau_units<node>(fm, units) < 0) return r.with_value(-1);
	// one pass over the body of a unit finds an embedded constant and a
	// reference to absolute time
	for (tref u : units)
		if (!tau::get(u).child_is(tau::wff_always)
			|| tau::get(tau::trim2(u)).find_top([](tref t) {
				return tree<node>::get(t).is_ba_constant()
					|| at_absolute_time<node>(t); }))
			return r.with_value(-1);
	std::vector<std::vector<std::string>> supp(units.size());
	for (size_t i = 0; i < units.size(); ++i)
		for (tref v : tau::get(units[i]).get_free_vars()) {
			const std::string& nm = get_var_name<node>(v);
			if (nm.empty()) return r.with_value(-1);
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
			// c is a bounded container index.
			erase_at(cn, c);
			erase_at(cc, c);
		}
	}
	if (cc.size() < 2) return r.with_value(-1);
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
		auto sat = r.merge_take(is_tau_formula_sat<node>(f));
		if (!sat) return r;
		pin_decided_key<node>(f);
		cache.insert_or_assign(f, *sat);
		all_sat = *sat;
	}
	return r.with_value(all_sat ? 1 : 0);
}

// Unit-wise validity (distributes over conjunction unconditionally);
// -1 = not applicable, 0 = not valid, 1 = valid. A unit whose decision fails
// gives no value and its report; the failure is not cached.
template <typename node>
static result<int> factored_tau_valid(tref fm) {
	using tau = tree<node>;
	result<int> r;
	trefs units;
	if (factored_tau_units<node>(fm, units) < 0) return r.with_value(-1);
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	bool all = true;
	for (size_t i = 0; i < units.size() && all; ++i) {
		if (auto it = cache.find(units[i]); it != cache.end()) {
			all = it->second;
			continue;
		}
		// the units before this one are valid. A unit that is not valid
		// ends the loop, and the units after it are not read: it does
		// not refer to absolute time, so it is not valid from any step,
		// and neither is the conjunction.
		if (refers_to_absolute_time<node>(units[i]))
			return r.with_value(-1);
		auto imp = r.merge_take(is_tau_impl<node>(tau::_T(), units[i]));
		if (!imp) return r;
		pin_decided_key<node>(units[i]);
		cache.insert_or_assign(units[i], *imp);
		all = *imp;
	}
	return r.with_value(all ? 1 : 0);
}

template <typename... BAs>
requires BAsPack<BAs...>
result<bool> tau_ba<BAs...>::is_zero() const {
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	return cached_tau_ba_predicate(*this, cache,
		[](tref normalized) -> result<bool> {
			result<bool> r;
			if (ba_component_factoring_enabled()) {
				auto f = r.merge_take(
					factored_tau_sat<node>(normalized));
				if (!f) return r;
				if (*f >= 0) return r.with_value(*f == 0);
			}
			auto sat = r.merge_take(
				is_tau_formula_sat<node>(normalized));
			if (!sat) return r;
			return r.with_value(!*sat);
		});
}

template <typename... BAs>
requires BAsPack<BAs...>
result<bool> tau_ba<BAs...>::is_one() const {
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	return cached_tau_ba_predicate(*this, cache,
		[](tref normalized) -> result<bool> {
			result<bool> r;
			if (ba_component_factoring_enabled()) {
				auto f = r.merge_take(
					factored_tau_valid<node>(normalized));
				if (!f) return r;
				if (*f >= 0) return r.with_value(*f == 1);
			}
			auto valid = r.merge_take(
				is_tau_impl<node>(tau::_T(), normalized));
			if (!valid) return r;
			return r.with_value(*valid);
		});
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const tau_ba<BAs...>& other, const bool& b) {
	// An undecidable is_one()/is_zero() falls to the side that never
	// misreports a witness: not one, and (unless proven otherwise) zero.
	// TODO (HIGH) dropped error: is_one's decision report -- operator== returns bool, which cannot carry it.
	// TODO (HIGH) dropped error: is_zero's decision report -- operator== returns bool, which cannot carry it.
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

/// Normalizes a tau_ba constant: applies its rec relations to the main
/// formula (nso_rr_apply) and simplifies unsat/valid subformulas. The
/// result carries the normalized main only — the rec relations, already
/// applied, are not copied into the returned tau_ba — or the report of a
/// failed step. Memoized by main tree when @p fm has no rec relations and
/// the BDD node table is not exhausted.
template <typename... BAs>
requires BAsPack<BAs...>
result<tau_ba<BAs...>> normalize_tau(const tau_ba<BAs...>& fm) {
	using node = typename tau_ba<BAs...>::node;
	using cache = detail::tau_decision_cache<node>;
	// The main tref identifies the constant only when it carries no rec
	// relations, so only that case may use the memo.
	bool memoize = fm.nso_rr.rec_relations.empty();
	auto& memo = cache::normalize_memo();
	tref key = fm.nso_rr.main->get();
	if (memoize) {
		if (auto it = memo.find(key); it != memo.end())
			return result<tau_ba<BAs...>>{
				tau_ba<BAs...>(tree<node>::geth(it->second))};
	}
	result<tau_ba<BAs...>> r;
	TAU_TRY(tref applied, nso_rr_apply<node>(fm.nso_rr));
	TAU_TRY(tref simplified, simp_tau_unsat_valid<node>(applied));
	tau_ba<BAs...> out(tree<node>::geth(simplified));
	if (memoize && !bdd_node_table_exhausted)
		memo.insert_or_assign(key, out.nso_rr.main->get());
	return r.with_value(std::move(out));
}

/// Memoized normalizer<node>(nso_rr) for the splitter's normalized-formula
/// precondition (ba_descriptor<tau_ba<...>>::splitter), called once per
/// candidate inside atomless_choose_value's splitter ladder. Deliberately a
/// separate cache from normalize_memo: normalizer() only normalizes, unlike
/// normalize_tau's own simp_tau_unsat_valid pass, so the two aren't
/// interchangeable. Returns the normalized formula (never null) or the
/// normalizer's report.
template <typename node>
result<tref> normalize_for_splitter(const rr<node>& nso_rr) {
	using cache = detail::tau_decision_cache<node>;
	// The main tref identifies the rr only when it carries no rec
	// relations, so only that case may use the memo.
	bool memoize = nso_rr.rec_relations.empty();
	auto& memo = cache::splitter_normalize_memo();
	tref key = nso_rr.main->get();
	if (memoize) {
		if (auto it = memo.find(key); it != memo.end())
			return result<tref>{it->second};
	}
	result<tref> r;
	TAU_TRY(tref normalized, normalizer<node>(nso_rr));
	// The normalizer never succeeds with a null formula; a null here is
	// the normalizer breaking its contract.
	if (!normalized)
		return r.with_error(code::runtime_error,
			"normalizer returned a null formula");
	if (memoize && !bdd_node_table_exhausted)
		memo.insert_or_assign(key, normalized);
	return r.with_value(normalized);
}

/// Purely syntactic check: the main formula is literally T. No rec
/// relations are applied and no satisfiability check runs — a semantically
/// valid but non-literal main returns false (use is_one() for that).
template <typename... BAs>
requires BAsPack<BAs...>
bool is_tau_syntactic_one(const tau_ba<BAs...>& fm) {
	return tree<node<tau_ba<BAs...>, BAs...>>::get(fm.nso_rr.main).equals_T();
}

/// Purely syntactic check: the main formula is literally F. No rec
/// relations are applied and no satisfiability check runs — a semantically
/// unsat but non-literal main returns false (use is_zero() for that).
template <typename... BAs>
requires BAsPack<BAs...>
bool is_tau_syntactic_zero(const tau_ba<BAs...>& fm) {
	return tree<node<tau_ba<BAs...>, BAs...>>::get(fm.nso_rr.main).equals_F();
}

template <typename... BAs>
requires BAsPack<BAs...>
result<tau_ba<BAs...>> splitter(const tau_ba<BAs...>& fm, splitter_type st) {
	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	result<tau_ba<BAs...>> r;
	TAU_TRY(tref n, normalize_for_splitter<node>(fm.nso_rr));
	TAU_TRY(tref s, (tau_splitter<tau_ba<BAs...>, BAs...>(n, st)));
	return r.with_value(tau_ba<BAs...>(tree<node>::geth(s)));
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
	TAU_TRY(tref simp_fm, apply_defs_to_spec<node>(*applied));
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
	if (!s.parse(src)) {
		for (const auto& error : s.errors())
			r.error(code::parse_error, error);
		if (!r.has_error())
			r.error(code::parse_error, "Failed to parse tau constant");
		return r;
	}
	TAU_TRY(auto nso_rr, s.get_nso_rr());
	// compute final result
	return r.with_value(typename node::constant_with_type{
		std::variant<tau_ba<BAs...>, BAs...>(
			tau_ba<BAs...>(nso_rr.rec_relations,
				       nso_rr.main)),
		tau_type<node>() });
}

template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& operator<<(std::ostream& os, const tau_ba<BAs...>& rs) {
	return print<node<tau_ba<BAs...>, BAs...>>(os, rs.nso_rr);
}

} // namespace idni::tau_lang
