// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file tau_options.h
 * @brief The core option set of the tau library: one spec per engine limit
 * and flag that an api setter owns, the codecs of the fields whose type is
 * no option kind, the hook that runs after a write by name, and the
 * binding of each option to its field.
 *
 * api.tmpl.h includes this file after every limit is declared, and
 * `tau_init()` declares the set and binds it. A program then loads the
 * environment with `options().load_env("TAU_")`.
 */

#ifndef __IDNI__TAU__TAU_OPTIONS_H__
#define __IDNI__TAU__TAU_OPTIONS_H__

#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <limits>
#include <string>
#include <string_view>
#include <system_error>

#include "utility/options.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

/// A `double` field as a text option.
struct real_codec {
	std::string_view name;
	option_value to_value(double d) const {
		char buf[64];
		auto [end, ec] = std::to_chars(buf, buf + sizeof buf, d);
		return std::string(buf, ec == std::errc{} ? end : buf);
	}
	result<double> from_value(const option_value& v) const {
		result<double> r;
		const std::string* text =
			idni::detail::option_text_of(v, r.report());
		if (!text) return r;
		double d = 0;
		const char* first = text->data();
		const char* last = first + text->size();
		auto [end, ec] = std::from_chars(first, last, d);
		if (ec != std::errc{} || end != last || !std::isfinite(d))
			return r.with_error(code::invalid_argument,
				parser_strings::messages::option_bad_value,
				{ { label::name, name },
				  { label::value, *text } });
		return r.with_value(d);
	}
};

/// The algorithm word of `ltl-alg`, checked against the words
/// @ref ltl_algorithm_name accepts and kept as given.
struct ltl_algorithm_codec {
	option_value to_value(const std::string& alg) const { return alg; }
	result<std::string> from_value(const option_value& v) const {
		result<std::string> r;
		const std::string* text =
			idni::detail::option_text_of(v, r.report());
		if (!text) return r;
		if (!ltl_algorithm_name(*text))
			return r.with_error(code::invalid_argument,
				parser_strings::messages::option_bad_value,
				{ { label::name, "ltl-alg" },
				  { label::value, *text } });
		return r.with_value(*text);
	}
};

/// A budget whose loops count down from SIZE_MAX: the option reads and
/// writes 0 for it, as its api setter does.
struct zero_is_unlimited_codec {
	option_value to_value(std::size_t n) const {
		return n == std::numeric_limits<std::size_t>::max()
			? std::size_t{ 0 } : n;
	}
	result<std::size_t> from_value(const option_value& v) const {
		result<std::size_t> r;
		const auto* n = std::get_if<std::size_t>(&v);
		if (!n) return r.with_error(code::type_error,
			parser_strings::messages::option_value_kind);
		return r.with_value(*n ? *n
			: std::numeric_limits<std::size_t>::max());
	}
};

/// The engine limits and flags of the library. The defaults of the
/// interpreter fields are written out: those fields exist per node type.
inline const option_set tau_core_option_set{ {
	// solver
	{ "block-max-splits", "solver",
		zero_is_unlimited_codec{}.to_value(block_boole_max_splits),
		"cap per-block Boole-decomposition splits in anti-prenexing "
		"(0 = unlimited)" },
	{ "block-max-rounds", "solver",
		zero_is_unlimited_codec{}.to_value(block_max_rounds),
		"cap anti-prenexing quantifier-block driver rounds "
		"(0 = unlimited)" },
	{ "cqe-max-clauses", "solver",
		zero_is_unlimited_codec{}.to_value(cqe_max_clauses),
		"cap the DNF clauses complete quantifier elimination may "
		"distribute one scope into (0 = unlimited)" },
	{ "lgrs-max-vars", "solver",
		zero_is_unlimited_codec{}.to_value(lgrs_max_vars),
		"hand a pure-equality bitvector system with more distinct "
		"variables than this to the solver instead of solving it "
		"algebraically per width (0 = unlimited)" },
	{ "max-fixpoint-steps", "solver", std::size_t{ max_fixpoint_steps },
		"cap temporal-normalization fixpoint steps (0 = unlimited)" },
	{ "max-flag-search-steps", "solver",
		std::size_t{ max_flag_search_steps },
		"cap the eventual-flag search past the flag boundary; a "
		"give-up reports an error, not a verdict (0 = unlimited)" },
	{ "block-squeeze-cap", "solver", std::size_t{ block_squeeze_cap },
		"skip block squeezing above this operand-set size "
		"(0 = unlimited)" },
	{ "max-simplify-rounds", "solver", std::size_t{ max_simplify_rounds },
		"cap bitvector simplification rewrite rounds (0 = unlimited)" },
	{ "max-def-passes", "solver", std::size_t{ max_def_passes },
		"cap definition-expansion passes (0 = unlimited)" },
	{ "max-probe-steps", "solver", std::size_t{ max_probe_steps },
		"cap the untyped saturation probe over a residual recurrence "
		"reference (0 = unlimited)" },
	{ "max-enum-steps", "solver", std::size_t{ max_enum_steps },
		"cap recurrence-relation enumeration steps (0 = unlimited)" },
	{ "max-rewrite-rounds", "solver", std::size_t{ max_rewrite_rounds },
		"cap rewrite-to-fixpoint rounds (0 = unlimited)" },
	{ "max-constant-size", "solver", std::size_t{ max_constant_size },
		"largest region of fresh values, in tree nodes, a run keeps "
		"across steps (0 = unlimited)" },
	// ltl
	{ "max-consistency-subsets", "ltl",
		std::size_t{ 4096 },
		"cap k-ary consistency subset checks per atom group "
		"(0 = unlimited)" },
	{ "max-cover-products", "ltl", std::size_t{ 256 },
		"cap the ABA oracle's mixed-type coverage expansion "
		"(0 = unlimited)" },
	{ "ltl-timeout", "ltl", std::size_t{ 60 },
		"wall-clock cap in seconds on each ltlsynt call "
		"(0 = no watchdog)" },
	{ "ltl-alg", "ltl", std::string("auto"),
		"omcat synthesis algorithm: A, B, D or auto" },
	{ "ltl-qe-max-vars", "ltl", std::size_t{ 2 },
		"free-variable cap of the omcat QE fast path; above 2 is not "
		"sound (0 = off)" },
	{ "ltl-hoa-max-states", "ltl", std::size_t{ 1 } << 22,
		"largest state count accepted from an ltlsynt HOA strategy "
		"(0 = unlimited)" },
	{ "ltl-guard-max-cubes", "ltl",
		std::size_t{ 512 },
		"cap the DNF cubes a HOA guard may expand into in the "
		"Algorithm D game (0 = unlimited)" },
	{ "ltl-refinement-rounds", "ltl",
		std::size_t{ 64 },
		"cap the ABA-oracle refinement rounds of a realizability "
		"check; the cap answers UNKNOWN (0 = unlimited)" },
	{ "ltl-window-max-paths", "ltl",
		std::size_t{ 4096 },
		"cap the strategy paths the multi-step window oracle examines "
		"per check (0 = unlimited)" },
	{ "ltl-closed-regions-timeout", "ltl",
		std::size_t{ 20 },
		"cap in seconds the data game's attempt on regions that keep "
		"their quantifiers, all its questions together, each at most a "
		"quarter of it (0 = no such attempt)" },
	{ "ltl-data-game-max-nodes", "ltl",
		std::size_t{ 1 } << 23,
		"cap the live nodes of the BDD of a data game over codes; a "
		"full table leaves the game undecided (0 = unlimited)" },
	{ "ltl-data-game-max-memo", "ltl",
		std::size_t{ 1 } << 25,
		"cap the operation memo entries of the BDD of a data game over "
		"codes; a full memo is emptied (0 = unlimited)" },
	{ "ltl-data-game-max-combinations", "ltl",
		std::size_t{ 4096 },
		"cap the value combinations the data game tabulates for one "
		"comparison its circuits do not encode (0 = unlimited)" },
	{ "ltl-max-observations", "ltl",
		std::size_t{ 8 },
		"cap the observation props whose impossible joint values the "
		"synthesis skeleton assumes away (at most 30, 0 = 30)" },
	{ "ltl-mealy-max-states", "ltl",
		std::size_t{ 4096 },
		"most states of the Mealy view a data-game strategy is played "
		"through (0 = no view)" },
	{ "ltl-mealy-max-edges", "ltl", std::size_t{ 1 } << 16,
		"most edges of the Mealy view a data-game strategy is played "
		"through (0 = no view)" },
	// gc
	{ "gc-min-size", "gc", std::size_t{ 256 },
		"tree-node count floor before gc may trigger" },
	{ "gc-growth-factor", "gc", std::string("1.5"),
		"gc triggers when node count grows by this factor since last "
		"sweep (<= 0 disables gc)" },
	// run
	{ "preprocessing", "run", bool{ preprocessing },
		"BA preprocessing, e.g. bv predicate blasting" },
	{ "pwr-semantic", "run", bool{ pwr_semantic_fallback },
		"enable the semantic (winning-region) fallback of the temporal "
		"pointwise revision" },
	{ "step-definitional-propagation", "run", true,
		"propagate the constants a step formula determines before its "
		"paths are enumerated" },
	{ "spec-size-warn", "run", std::size_t{ 0 },
		"warn when an updated specification exceeds this many "
		"characters (0 = off)" },
	{ "max-revision-alts", "run", std::size_t{ 0 },
		"cap the revision alternatives kept per specification part, "
		"dropping middle preference tiers (0 = unlimited)" },
	{ "cache-bound", "run", std::size_t{ cache_bound },
		"bound the string-keyed synthesis caches, FIFO eviction "
		"(0 = unbounded)" },
	{ "tref-budget", "run", std::size_t{ tref_budget_param },
		"cap the live interned tree nodes; an api call that starts "
		"with the store at or above the cap fails instead of running "
		"(0 = unlimited)" },
	{ "tref-budget-soft", "run",
		std::size_t{ tref_budget_soft_param },
		"percentage of --tref-budget at which a sweep is forced "
		"regardless of the gc growth trigger" },
	{ "compile-max-table-edges", "run",
		std::size_t{ 400 },
		"most edges of a Mealy view gen/compile carries as a table "
		"instead of solving as the program runs (0 = none)" },
	{ "compile-build-timeout", "run", std::size_t{ 3600 },
		"seconds the cmake build of compile may take before it is "
		"stopped (0 = no timeout)" },
	{ "bf-dependence-max-nodes", "run",
		std::size_t{ bf_dependence_max_nodes },
		"cap the BDD nodes built to tell whether a Boolean function "
		"depends on a variable (0 = unlimited)" },
} };

namespace api_detail {
template <NodeType node> size_t semantic_options_fingerprint();
}

/// The semantic fingerprint after the last option write. A hook runs after
/// its write only, so this is what it compares against.
template <NodeType node>
inline size_t semantic_options_seen = 0;

/// The hook of every core option: what `option_change_guard` does around
/// an api setter, for a write by name.
template <NodeType node>
void clear_caches_on_semantic_change() {
	const size_t now = api_detail::semantic_options_fingerprint<node>();
	if (now != semantic_options_seen<node>) tree<node>::clear_caches();
	semantic_options_seen<node> = now;
}

/// Binds the options whose field is a plain global to that field. An option
/// of a limit that still reads its own variable stays unbound.
template <NodeType node>
result<void> bind_core_options(options_repository& repo) {
	result<void> r;
	using interp = interpreter<node>;
	const option_hook hook = clear_caches_on_semantic_change<node>;
	semantic_options_seen<node> =
		api_detail::semantic_options_fingerprint<node>();
	TAU_TRY_VOID(repo.bind("block-max-splits", block_boole_max_splits,
		zero_is_unlimited_codec{}, hook));
	TAU_TRY_VOID(repo.bind("block-max-rounds", block_max_rounds,
		zero_is_unlimited_codec{}, hook));
	TAU_TRY_VOID(repo.bind("cqe-max-clauses", cqe_max_clauses,
		zero_is_unlimited_codec{}, hook));
	TAU_TRY_VOID(repo.bind("lgrs-max-vars", lgrs_max_vars,
		zero_is_unlimited_codec{}, hook));
	TAU_TRY_VOID(repo.bind("max-fixpoint-steps", max_fixpoint_steps, hook));
	TAU_TRY_VOID(repo.bind("max-flag-search-steps", max_flag_search_steps,
		hook));
	TAU_TRY_VOID(repo.bind("block-squeeze-cap", block_squeeze_cap, hook));
	TAU_TRY_VOID(repo.bind("max-simplify-rounds", max_simplify_rounds,
		hook));
	TAU_TRY_VOID(repo.bind("max-def-passes", max_def_passes, hook));
	TAU_TRY_VOID(repo.bind("max-probe-steps", max_probe_steps, hook));
	TAU_TRY_VOID(repo.bind("max-enum-steps", max_enum_steps, hook));
	TAU_TRY_VOID(repo.bind("max-rewrite-rounds", max_rewrite_rounds, hook));
	TAU_TRY_VOID(repo.bind("max-constant-size", max_constant_size, hook));
	TAU_TRY_VOID(repo.bind("gc-min-size", interp::gc_min_size, hook));
	TAU_TRY_VOID(repo.bind("gc-growth-factor", interp::gc_growth_factor,
		real_codec{ "gc-growth-factor" }, hook));
	TAU_TRY_VOID(repo.bind("preprocessing", preprocessing, hook));
	TAU_TRY_VOID(repo.bind("pwr-semantic", pwr_semantic_fallback, hook));
	TAU_TRY_VOID(repo.bind("step-definitional-propagation",
		interp::definitional_propagation, hook));
	TAU_TRY_VOID(repo.bind("spec-size-warn", interp::spec_size_warn_threshold,
		hook));
	TAU_TRY_VOID(repo.bind("max-revision-alts", interp::max_revision_alts,
		hook));
	TAU_TRY_VOID(repo.bind("cache-bound", cache_bound, hook));
	TAU_TRY_VOID(repo.bind("tref-budget", tref_budget_param, hook));
	TAU_TRY_VOID(repo.bind("tref-budget-soft", tref_budget_soft_param, hook));
	TAU_TRY_VOID(repo.bind("bf-dependence-max-nodes",
		bf_dependence_max_nodes, hook));
	return r;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TAU_OPTIONS_H__
