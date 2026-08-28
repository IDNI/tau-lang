// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/tau_ba.h"

#include "tau_spec.h"

#include <cstdlib>

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "tau_ba"

namespace idni::tau_lang {

template <typename... BAs>
requires BAsPack<BAs...>
static tref normalized_tau_ba_main(const tau_ba<BAs...>& fm) {
	using node = typename tau_ba<BAs...>::node;
	// Memoised per main tree: every Boolean operation on constants
	// (~, &, |, +) normalises the temporal layer of its operands, and the
	// same constants are operands over and over. Same key discipline as
	// cached_tau_ba_predicate: the main tree identifies the element only
	// when it carries no recurrence relations.
	if (!fm.nso_rr.rec_relations.empty())
		return normalize_temporal_quantifiers<node, false>(
			fm.nso_rr.main->get());
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	tref key = fm.nso_rr.main->get();
	if (auto it = cache.find(key); it != cache.end()) return it->second;
	// compute before emplace: normalisation can create new trees, and a
	// rehash of `cache` must not happen with a half-built entry in it.
	tref res = normalize_temporal_quantifiers<node, false>(key);
	return cache.insert_or_assign(key, res).first->second;
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
	auto nmain = tau::geth(
		to_nnf<node>(tau::build_wff_neg(normalized_tau_ba_main(*this))));
	auto nrec_relations = nso_rr.rec_relations;
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator&(const tau_ba<BAs...>& other) const {
	auto lhs = normalized_tau_ba_main(*this);
	auto rhs = normalized_tau_ba_main(other);
	auto nmain = tau::geth(tau::build_wff_and(lhs, rhs));
	auto nrec_relations =
		rewriter::merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator|(const tau_ba<BAs...>& other) const {
	auto lhs = normalized_tau_ba_main(*this);
	auto rhs = normalized_tau_ba_main(other);
	auto nmain = tau::geth(tau::build_wff_or(lhs, rhs));
	auto nrec_relations = rewriter::merge(nso_rr.rec_relations,
					      other.nso_rr.rec_relations);
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator+(const tau_ba<BAs...>& other) const {
	auto lhs = normalized_tau_ba_main(*this);
	auto rhs = normalized_tau_ba_main(other);
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
template <typename... BAs>
requires BAsPack<BAs...>
static bool cached_tau_ba_predicate(const tau_ba<BAs...>& fm,
	subtree_unordered_map<typename tau_ba<BAs...>::node, bool>& cache,
	auto&& compute)
{
	using node = typename tau_ba<BAs...>::node;
	if (!fm.nso_rr.rec_relations.empty())
		return compute(normalizer<node>(fm.nso_rr));
	tref key = fm.nso_rr.main->get();
	if (auto it = cache.find(key); it != cache.end()) return it->second;
	// compute() before emplace: it can create new trees, and a rehash of
	// `cache` must not happen with a half-built entry in it.
	bool res = compute(normalizer<node>(fm.nso_rr));
	return cache.insert_or_assign(key, res).first->second;
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

inline bool ba_component_factoring_enabled() {
	static const bool env = [] {
		const char* v = std::getenv("TAU_BA_COMPONENT_FACTORING");
		return v && *v && !(v[0] == '0' && v[1] == '\0');
	}();
	return ba_component_factoring || env;
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
		bool sres = is_tau_formula_sat<node>(f);
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
		bool vres = is_tau_impl<node>(tau::_T(), units[i]);
		cache.insert_or_assign(units[i], vres);
		all = vres;
	}
	return all ? 1 : 0;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool tau_ba<BAs...>::is_zero() const {
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	return cached_tau_ba_predicate(*this, cache, [](tref normalized) {
		if (ba_component_factoring_enabled())
			if (int r = factored_tau_sat<node>(normalized);
					r >= 0)
				return r == 0;
		return !is_tau_formula_sat<node>(normalized);
	});
}

template <typename... BAs>
requires BAsPack<BAs...>
bool tau_ba<BAs...>::is_one() const {
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	return cached_tau_ba_predicate(*this, cache, [](tref normalized) {
		if (ba_component_factoring_enabled())
			if (int r = factored_tau_valid<node>(normalized);
					r >= 0)
				return r == 1;
		return is_tau_impl<node>(tau::_T(), normalized);
	});
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const tau_ba<BAs...>& other, const bool& b) {
	return b ? other.is_one() : other.is_zero();
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

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> normalize_tau(const tau_ba<BAs...>& fm) {
	tref result =
		nso_rr_apply<node<tau_ba<BAs...>, BAs...>>(fm.nso_rr);
	result = simp_tau_unsat_valid<node<tau_ba<BAs...>, BAs...>>(result);
	return tau_ba<BAs...>(tree<node<tau_ba<BAs...>, BAs...>>::geth(result));
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_tau_syntactic_one(const tau_ba<BAs...>& fm) {
	return tree<node<tau_ba<BAs...>, BAs...>>::get(fm.nso_rr.main).equals_T();
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_tau_syntactic_zero(const tau_ba<BAs...>& fm) {
	return tree<node<tau_ba<BAs...>, BAs...>>::get(fm.nso_rr.main).equals_F();
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> splitter(const tau_ba<BAs...>& fm, splitter_type st) {
	tref s = tau_splitter<tau_ba<BAs...>, BAs...>(
		normalizer<node<tau_ba<BAs...>, BAs...>>(fm.nso_rr), st);
	return tau_ba<BAs...>(tree<node<tau_ba<BAs...>, BAs...>>::geth(s));
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_splitter_one() {
	return tau_ba<BAs...>(tau_bad_splitter<tau_ba<BAs...>, BAs...>());
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_tau_closed(const tau_ba<BAs...>& fm) {
	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	using tau = tree<node>;
	tref simp_fm = nso_rr_apply<node>(fm.nso_rr);
	if (!simp_fm) return false;
	simp_fm = apply_defs_to_spec<node>(simp_fm);
	if (!simp_fm) return false;
	if (tau::get(simp_fm).find_top(is<node, tau::ref>))
		return false;
	const trefs& vars = get_free_vars<node>(simp_fm);
	for (tref v : vars) {
		const tau& t = tau::get(v);
		if (!(t.child_is(tau::io_var)
			|| t.child_is(tau::uconst_name)))
				return false;
	}
	return true;
}

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<typename node<tau_ba<BAs...>, BAs...>::constant_with_type>
	parse_tau(const std::string& src)
{
	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	// parse source
	tau_spec<node> s;
	std::optional<rr<node>> maybe_nso_rr;
	if (!s.parse(src) || !(maybe_nso_rr = s.get_nso_rr())) {
		// TODO: pass these errors on so api users can handle them
		for (const auto& error : s.errors())
			TAU_LOG_ERROR << "[tau] " << error;
		return {};
	}
	// compute final result
	return typename node::constant_with_type{
		std::variant<tau_ba<BAs...>, BAs...>(
			tau_ba<BAs...>(maybe_nso_rr.value().rec_relations,
				       maybe_nso_rr.value().main)),
		tau_type<node>() };
}

template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& operator<<(std::ostream& os, const tau_ba<BAs...>& rs) {
	return print<node<tau_ba<BAs...>, BAs...>>(os, rs.nso_rr);
}

} // namespace idni::tau_lang
