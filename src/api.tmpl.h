// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "api.h"

#include <cstdio>
#include <cstdlib>
#include <optional>

#include "tau_tree_builders.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "api"

namespace idni::tau_lang {

template <NodeType node>
void tau_init() {
	bdd_init<Bool>();
	// nat/untyped/bool are core-reserved, not a BA -- literals, and
	// building a tree here would need this function to already exist.
	static bool registered = false;
	if (registered) return;
	registered = true;
	std::vector<std::string> names{ "nat", "untyped", "bool" };
	for (auto n : node::ba::type_names()) names.emplace_back(n);
	auto& g = tau_parser::instance().get_grammar();
	g.add_dynamic("type_name", names);
	// lets a declared type name parse as type_name for the rest of the spec
	auto type_name_l = g.nt("type_name");
	tau_parser::instance().set_dynamic_grow(
		[type_name_l](tau_parser::input& in, size_t, size_t from,
			size_t to)
		{
			tau_parser::instance().get_grammar()
				.add_dynamic_production_from(type_name_l,
					in.get_terminals(from, to));
		},
		{ { tau_parser::type_def, tau_parser::new_type_name } });
}

// Helper functions
// ------------------------------------------------------------

// Extracts the spec update carried by the step's outputs: the tau
// constant assigned to the `u` output stream at the just-finished time
// point. Returns nullptr when the context types `u` as anything but tau
// or the assignment is absent or 0 (i.e. no update requested).
template <NodeType node>
tref get_update(interpreter<node>& i, const assignment<node>& output) {
	auto update_stream = build_out_var_at_n<node>(
		"u", i.time_point - 1, get_ba_type_id<node>(tau_type<node>()));
	// Update only if u is of type tau
	if (size_t t = i.ctx.type_of(update_stream);
		t != 0 && t == get_ba_type_id<node>(tau_type<node>()))
	{
		auto it = output.find(update_stream);
		if (it != output.end()
			&& !tree<node>::get(it->second).equals_0())
		{
			return unpack_tau_constant<node>(it->second);
		}
	}
	return nullptr;
}

// ------------------------------------------------------------
// common API settings
// ------------------------------------------------------------

template <NodeType node>
void api<node>::set_charvar(bool charvar) {
	std::set<std::string> guards{ charvar ? "charvar" : "var" };
	tau_parser::instance().get_grammar().set_enabled_productions(guards);
	pack_set_charvar<node>(charvar);
}

template <NodeType node>
void api<node>::set_preprocessing(bool enabled) {
	preprocessing = enabled;
}

template <NodeType node>
void api<node>::set_preprocess_placement(int site) {
	preprocess_placement = (site >= static_cast<int>(preprocess_site::per_leaf)
		&& site <= static_cast<int>(preprocess_site::per_formula))
			? static_cast<preprocess_site>(site) : preprocess_site::per_leaf;
}

template <NodeType node>
void api<node>::set_preprocess_method(int mode) {
	preprocess_method = (mode >= static_cast<int>(preprocess_mode::anti_prenex_result)
		&& mode <= static_cast<int>(preprocess_mode::defer))
			? static_cast<preprocess_mode>(mode)
			: preprocess_mode::anti_prenex_result;
}

template <NodeType node>
void api<node>::set_solver_placement(int site) {
	solver_placement = (site >= static_cast<int>(solver_site::eager)
		&& site <= static_cast<int>(solver_site::per_formula))
			? static_cast<solver_site>(site) : solver_site::eager;
}

template <NodeType node>
void api<node>::set_cvc5_options(int set) {
	cvc5_options = (set >= static_cast<int>(cvc5_option_set::baseline)
		&& set <= static_cast<int>(cvc5_option_set::combined_best))
			? static_cast<cvc5_option_set>(set)
			: cvc5_option_set::ext_rewrite_no_models;
}

// The two block budgets keep SIZE_MAX as their internal "unlimited" sentinel
// (their loops decrement them); the option surface says 0 = unlimited, so the
// setters translate.
template <NodeType node>
void api<node>::set_block_max_splits(size_t n) {
	block_boole_max_splits = n ? n : std::numeric_limits<size_t>::max();
}

template <NodeType node>
void api<node>::set_block_max_rounds(size_t n) {
	block_max_rounds = n ? n : std::numeric_limits<size_t>::max();
}

template <NodeType node>
void api<node>::set_cqe_max_clauses(size_t n) {
	cqe_max_clauses = n ? n : std::numeric_limits<size_t>::max();
}

template <NodeType node>
void api<node>::set_lgrs_max_vars(size_t n) {
	lgrs_max_vars = n ? n : std::numeric_limits<size_t>::max();
}

template <NodeType node>
void api<node>::set_max_blast_reentry_depth(size_t n) {
	max_blast_reentry_depth = n;
}

template <NodeType node>
void api<node>::set_block_squeeze_cap(size_t n) {
	block_squeeze_cap = n;
}

template <NodeType node>
void api<node>::set_max_fixpoint_steps(size_t n) {
	max_fixpoint_steps = n;
}

template <NodeType node>
void api<node>::set_max_flag_search_steps(size_t n) {
	max_flag_search_steps = n;
}

template <NodeType node>
void api<node>::set_max_def_passes(size_t n) {
	max_def_passes = n;
}

template <NodeType node>
void api<node>::set_max_enum_steps(size_t n) {
	max_enum_steps = n;
}

template <NodeType node>
void api<node>::set_max_probe_steps(size_t n) {
	max_probe_steps = n;
}

template <NodeType node>
void api<node>::set_max_rewrite_rounds(size_t n) {
	max_rewrite_rounds = n;
}

template <NodeType node>
void api<node>::set_max_simplify_rounds(size_t n) {
	max_simplify_rounds = n;
}

template <NodeType node>
void api<node>::set_gc_min_size(size_t n) {
	interpreter<node>::gc_min_size = n;
}

template <NodeType node>
void api<node>::set_gc_growth_factor(double f) {
	interpreter<node>::gc_growth_factor = f;
}

template <NodeType node>
void api<node>::set_tref_budget(size_t n) {
	tref_budget_param = (long) n;
}

template <NodeType node>
void api<node>::set_tref_budget_soft_percent(size_t pct) {
	tref_budget_soft_param = (long) pct;
}

template <NodeType node>
size_t api<node>::tref_count() {
	return tau_lang::tref_count<node>();
}

template <NodeType node>
void api<node>::set_spec_size_warn(size_t n) {
	interpreter<node>::spec_size_warn_threshold = n;
}

template <NodeType node>
void api<node>::set_max_revision_alts(size_t n) {
	interpreter<node>::max_revision_alts = n;
}

template <NodeType node>
void api<node>::set_max_consistency_subsets(size_t n) {
	max_consistency_subsets = n;
}

template <NodeType node>
void api<node>::set_cache_bound(size_t n) {
	cache_bound = n;
}

template <NodeType node>
void api<node>::set_max_cover_products(size_t n) {
	max_cover_products = n;
}

template <NodeType node>
void api<node>::set_ltl_timeout_sec(long seconds) {
	ltl_timeout_sec_param = seconds < 0 ? -1
		: std::min(seconds, ltl_timeout_sec_max);
}

template <NodeType node>
void api<node>::set_ltl_algorithm(const std::string& alg) {
	ltl_algorithm_param = alg;
}

template <NodeType node>
void api<node>::set_ltl_qe_max_vars(size_t n) {
	ltl_qe_max_vars_param = n;
}

template <NodeType node>
void api<node>::set_ltl_hoa_max_states(size_t n) {
	ltl_hoa_max_states_param = (long) n;
}

template <NodeType node>
void api<node>::set_ltl_guard_max_cubes(size_t n) {
	ltl_guard_max_cubes_param = (long) n;
}

template <NodeType node>
void api<node>::set_ltl_max_refinement_rounds(size_t n) {
	ltl_max_refinement_rounds_param = (long) n;
}

template <NodeType node>
void api<node>::set_ltl_window_max_paths(size_t n) {
	ltl_window_max_paths_param = (long) n;
}

template <NodeType node>
void api<node>::set_pwr_semantic_fallback(bool on) {
	pwr_semantic_fallback = on;
}

template <NodeType node>
void api<node>::set_step_definitional_propagation(bool on) {
	interpreter<node>::definitional_propagation = on;
}

template <NodeType node>
void api<node>::set_indenting(bool indenting) {
	pretty_printer_indenting = indenting;
}

template <NodeType node>
void api<node>::set_ba_component_factoring(bool state) {
	pack_set_ba_component_factoring<node>(state);
}

template <NodeType node>
void api<node>::set_ba_decision_pins(size_t n) {
	pack_set_ba_decision_pins<node>(n);
}

namespace api_detail {

template <NodeType node>
result<const ba_option*> find_ba_option(const std::string& name) {
	result<const ba_option*> r;
	const auto dash = name.find('-');
	if (dash == std::string::npos || dash == 0 || dash + 1 == name.size())
		return r.with_error(code::invalid_argument, "A BA option is "
			"named <family>-<option>", {{ label::value, name }});
	const auto found = pack_find_ba_option<node>(name.substr(0, dash),
		name.substr(dash + 1));
	switch (found.status) {
	case ba_option_lookup_status::found: return r.with_value(found.option);
	case ba_option_lookup_status::no_such_family:
		return r.with_error(code::not_found, "No BA of this family in "
			"the pack", {{ label::value, name }});
	case ba_option_lookup_status::no_such_option: break;
	}
	return r.with_error(code::not_found, "The BA declares no such option",
		{{ label::value, name }});
}

inline size_t ba_option_value(const ba_option& o) {
	return o.kind == ba_option_kind::flag ? (size_t) o.get_flag()
		: o.get_count();
}

} // namespace api_detail

template <NodeType node>
result<size_t> api<node>::set_ba_option(const std::string& name,
	size_t value)
{
	result<size_t> r;
	TAU_TRY(const ba_option* o, api_detail::find_ba_option<node>(name));
	if (o->kind == ba_option_kind::flag) o->set_flag(value != 0);
	else o->set_count(value);
	return r.with_value(api_detail::ba_option_value(*o));
}

template <NodeType node>
result<size_t> api<node>::get_ba_option(const std::string& name) {
	result<size_t> r;
	TAU_TRY(const ba_option* o, api_detail::find_ba_option<node>(name));
	return r.with_value(api_detail::ba_option_value(*o));
}

template <NodeType node>
std::vector<std::string> api<node>::ba_option_names() {
	std::vector<std::string> names;
	for (const auto& e : pack_ba_options<node>())
		names.push_back(e.family + "-" + e.option.name);
	return names;
}

template <NodeType node>
void api<node>::set_highlighting(bool highlighting) {
	pretty_printer_highlighting = highlighting;
}

// pretty_printer_highlighting above is the tree printer; this is the terminal
// colour used by diagnostics rendering (idni::TC, term_colors.h).
template <NodeType node>
void api<node>::set_colors(bool colors) {
	idni::TC.set(colors);
}

template <NodeType node>
void api<node>::set_json(bool json) {
	print_json = json;
}

template <NodeType node>
void api<node>::set_severity(boost::log::trivial::severity_level level) {
	logging::set_filter(level);
}

// ------------------------------------------------------------
// tref API
// ------------------------------------------------------------

// Parsing
// ------------------------------------------------------------

/// Build a get_options struct for tree<node>::get() based on the parse
/// start symbol and the simplified flag.  When simplified=false, BA type
/// inference and rewriting hooks are both disabled.
template <NodeType node>
inline typename tree<node>::get_options get_options(typename node::type start, bool simplified) {
	typename tree<node>::get_options options;
	options.parse = { .start = start };
	options.use_default_types = false;
	if (!simplified) {
		options.infer_ba_types = false;
		options.reget_with_hooks = false;
	}
	return options;
}

template <NodeType node>
result<tref> api<node>::get_term(const std::string& input, bool simplified) {
	return with_budget<node>([&] {
		result<tref> r;
		TAU_TRY(tref e, tau::get(input, get_options<node>(tau::bf, simplified)));
		if (!e) r.error(code::parse_error, "Failed to parse term");
		else    r = e;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::get_formula(const std::string& input, bool simplified) {
	return with_budget<node>([&] {
		result<tref> r;
		TAU_TRY(tref fm, tau::get(input, get_options<node>(tau::wff, simplified)));
		if (!fm) {
			return r.with_assert_check_error(code::parse_error, "Failed to parse formula");
		}
		// An io_var's input/output bit is set while parsing a *spec*, so parsing
		// a bare wff leaves every io_var classified as neither -- and consumers
		// that need the distinction (transform_io_var,
		// existentially_quantify_output_streams, the LTL/PWR pipelines) then
		// reject it or, worse, silently treat inputs as outputs. Resolve here so
		// the same text yields the same classification whether it arrives as a
		// formula or inside a spec; this mirrors what get_nso_rr already does for
		// a bare wff/bf.
		return r.with_assert_check_value(resolve_io_vars<node>(
			*definitions<node>::instance().get_io_context(), fm));
	});
}

template <NodeType node>
result<tref> api<node>::get_function_def(const std::string& function_def, [[maybe_unused]] bool simplified) {
	return with_budget<node>([&] {
		result<tref> r;
		// AP1-5: parse and validate BEFORE registering -- routing through
		// get_definition registered unconditionally, so a rejected
		// definition stayed in the global store and leaked into later
		// apply_defs_to_spec calls.
		TAU_TRY(tref def, tau::get(function_def,
			get_options<node>(tau::rec_relation, true)));
		if (!def) {
			return r.with_assert_check_error(code::parse_error, "Failed to parse function definition");
		}
		// The second child of a rec_relation is the body;
		// accept bf or ref (a ref body may resolve to a bf later --
		// AP1-26: the code rejected refs while doc and the predicate
		// sibling accepted them)
		auto nt = tau::get(def)[1].get_type();
		if (nt != tau::bf && nt != tau::ref) {
			return r.with_assert_check_error(code::invalid_argument, "Not a function definition");
		}
		auto added = add_definition(tau::get(def).first(), tau::get(def).second());
		r.merge(std::move(added));
		if (r.has_error()) {
			DBG(assert(r.is_well_formed());)
			return r;
		}
		return r.with_assert_check_value(def);
	});
}

template <NodeType node>
result<tref> api<node>::get_predicate_def(const std::string& predicate_def, [[maybe_unused]] bool simplified) {
	return with_budget<node>([&] {
		result<tref> r;
		// AP1-5: parse and validate BEFORE registering (see get_function_def).
		TAU_TRY(tref def, tau::get(predicate_def,
			get_options<node>(tau::rec_relation, true)));
		if (!def) {
			return r.with_assert_check_error(code::parse_error, "Failed to parse predicate definition");
		}
		// TODO we could pre resolve all refs to wff
		auto nt = tau::get(def)[1].get_type();
		if (nt != tau::wff && nt != tau::ref) {
			return r.with_assert_check_error(code::invalid_argument, "Not a predicate definition");
		}
		auto added = add_definition(tau::get(def).first(), tau::get(def).second());
		r.merge(std::move(added));
		if (r.has_error()) {
			DBG(assert(r.is_well_formed());)
			return r;
		}
		return r.with_assert_check_value(def);
	});
}

template <NodeType node>
result<tref> api<node>::get_stream_def(const std::string& stream_def) {
	return with_budget<node>([&] {
		result<tref> r;
		TAU_TRY(tref def, tau::get(stream_def, get_options<node>(tau::stream_def, true)));
		if (!def) r.error(code::parse_error, "Failed to parse stream definition");
		else      r = tau::trim(def);
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::get_spec(const std::string& src) {
	return with_budget<node>([&] {
		result<tref> r;
		tau_spec<node> spec;
		if (!spec.parse(src)) {
			for (const auto& error : spec.errors())
				r.error(code::parse_error, error);
			if (!r.has_error()) r.error(code::parse_error, messages::failed_to_parse_spec);
			DBG(assert(r.is_well_formed());)
			return r;
		}
		TAU_TRY(tref s, spec.get());
		if (!s) r.error(code::parse_error, messages::failed_to_parse_spec);
		else r = s;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<size_t> api<node>::add_definition(tref head, tref body) {
	return with_budget<node>([&] {
		result<size_t> r;
		DBG(TAU_LOG_TRACE << "add_definition/head: " << LOG_FM_DUMP(head);)
		DBG(TAU_LOG_TRACE << "add_definition/body: " << LOG_FM_DUMP(body);)
		if (!head || !body) {
			if (!head) {
				DBG(TAU_LOG_TRACE << "add_definition/head is nullptr";)
			}
			if (!body) {
				DBG(TAU_LOG_TRACE << "add_definition/body is nullptr";)
			}
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		DBG(TAU_LOG_TRACE << "add_definition/adding head: " << LOG_FM_DUMP(head);)
		DBG(TAU_LOG_TRACE << "add_definition/adding body: " << LOG_FM_DUMP(body);)
		// AP1-6: 1-based -- the store's 0-based index made the very first
		// definition return 0, the documented failure value.
		return r.with_assert_check_value(definitions<node>::instance().add(
			tau::geth(head), tau::geth(body)) + 1);
	});
}

template <NodeType node>
void api<node>::reset_definitions() {
	definitions<node>::instance().clear();
}

template <NodeType node>
size_t api<node>::reset() {
	reset_definitions();
	run_reset_hooks();
	const size_t before = tref_count();
	std::unordered_set<tref> keep;
	// The Tau-BDD store is never swept and holds raw Tau trefs.
	tau_term_bdd<node>::collect_live_refs(keep);
	bintree<node>::gc(keep);
	const size_t after = tref_count();
	return before > after ? before - after : 0;
}

template <NodeType node>
result<tref> api<node>::get_definition(const std::string& definition, bool simplified) {
	return with_budget<node>([&] {
		result<tref> r;
		DBG(TAU_LOG_TRACE << "get_definition/definition: " << definition;)
		TAU_TRY(tref def, tau::get(definition, get_options<node>(tau::rec_relation, simplified)));
		DBG(TAU_LOG_TRACE << "get_definition/def: " << LOG_FM_DUMP(def);)
		if (!def) {
			return r.with_assert_check_error(code::parse_error, "Failed to parse definition");
		}
		auto added = add_definition(tau::get(def).first(), tau::get(def).second());
		r.merge(std::move(added));
		if (r.has_error()) {
			DBG(assert(r.is_well_formed());)
			return r;
		}
		return r.with_assert_check_value(def);
	});
}

template <NodeType node>
result<tref> api<node>::get_spec_or_term(const std::string& expression, bool simplified) {
	return with_budget<node>([&] {
		result<tref> r;
		// Try parsing as a full spec first (which handles multiline and
		// formula inputs); fall back to a bare bf term if that fails.
		// AP1-18: the spec attempt is quiet -- its parse errors reach the
		// report only when the term fallback ALSO fails, so a legitimate bare
		// term does not carry spurious errors on the successful path.
		tau_spec<node> spec;
		result<tref> spec_r;
		tref expr = nullptr;
		if (spec.parse(expression)) {
			spec_r = spec.get();
			if (spec_r.has_value()) expr = spec_r.value();
		}
		if (expr) {
			return r.with_assert_check_value(expr);
		}
		auto term_val = r.merge_take(get_term(expression, simplified));
		if (!term_val) {
			r.merge(std::move(spec_r));
			for (const auto& error : spec.errors()) r.error(code::parse_error, error);
			DBG(assert(r.is_well_formed());)
			return r;
		}
		return r.with_assert_check_value(std::move(*term_val));
	});
}

template <NodeType node>
result<tref> api<node>::get_formula_or_term(const std::string& expr, bool simplified) {
	return with_budget<node>([&] {
		result<tref> r;
		TAU_TRY(tref e, tau::get(expr, get_options<node>(tau::fm_or_term, simplified)));
		if (!e) {
			return r.with_assert_check_error(code::parse_error, "Failed to parse formula or term");
		}
		e = tau::trim(e);
		// AP1-32: classify io_vars like get_formula does, so the same text
		// yields the same tree through either entry (downstream re-resolution
		// hid the difference from sat machinery, but the trees differed).
		if (tau::get(e).is(tau::wff))
			e = resolve_io_vars<node>(
				*definitions<node>::instance().get_io_context(), e);
		if (!e) r.error(code::internal_error, "Failed to parse formula or term");
		else    r = e;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

// Querying
// ------------------------------------------------------------

template <NodeType node>
bool api<node>::contains(tref expression, typename node::type nt) {
	bool found = false;
	const auto searcher = [&nt, &found](tref n) -> bool {
		if (tau::get(n).get_type() == nt) return found = true, false;
		return true;
	};
	// searcher's answer depends only on a node's own type, so it gives the
	// same result for every occurrence of a shared subtree -- search_unique
	// is sound here and avoids revisiting shared subtrees.
	pre_order<node>(expression).search_unique(searcher);
	return found;
}

template <NodeType node>
bool api<node>::is_term(tref term) {
	return tau::get(term).is_term();
}

template <NodeType node>
bool api<node>::is_formula(tref fm) {
	return tau::get(fm).is(tau::wff);
}

// Using definitions
// ------------------------------------------------------------

template <NodeType node>
result<tref> api<node>::apply_def(tref def, tref expr) {
	return with_budget<node>([&] {
		return apply_defs(subtree_set<node>{ def }, expr);
	});
}

template <NodeType node>
result<tref> api<node>::apply_defs(subtree_set<node> defs, tref expr) {
	return with_budget<node>([&] {
		result<tref> r;
		if (!expr) {
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		TAU_TRY(auto nso_rr, get_nso_rr(expr));
		io_context<node>& ctx = *definitions<node>::instance().get_io_context();
		for (tref def : defs) if (def) {
			const auto& t = tau::get(def);
			if (t.is(tau::rec_relation)) {
				nso_rr.rec_relations.emplace_back(
					tau::geth(t.first()),
					tau::geth(resolve_io_vars<node>(ctx, t.second())));
			}
		}
		TAU_TRY_OR(r, nso_rr_apply<node>(nso_rr),
			code::internal_error, "Failed to apply definitions");
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::apply_all_defs(tref expr) {
	return with_budget<node>([&] {
		// AP1-4: this must apply the globally registered definitions --
		// routing through apply_defs({}) applied nothing, so after
		// get_definition() the dnf/cnf/nnf/solve/lgrs pipelines received
		// refs unexpanded, contradicting the documented contract. Mirrors
		// the normalizer's apply_defs_to_spec.
		result<tref> r;
		if (!expr) {
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		TAU_TRY(auto nso_rr, get_nso_rr(expr));
		const auto& defs = definitions<node>::instance().get_sym_defs();
		nso_rr.rec_relations.insert(nso_rr.rec_relations.end(),
			defs.begin(), defs.end());
		TAU_TRY_OR(r, nso_rr_apply<node>(nso_rr),
			code::internal_error, "Failed to apply definitions");
		DBG(assert(r.is_well_formed());)
		return r;
	});
}


// Printing
// ------------------------------------------------------------

template <NodeType node>
std::ostream& api<node>::print(std::ostream& os, tref expression) {
	if (!expression) return os;
	return tau::get(expression).print(os);
}

template <NodeType node>
std::string api<node>::to_str(tref expression) {
	if (!expression) return "";
	return tau::get(expression).to_str();
}

// Substitution
// ------------------------------------------------------------

template <NodeType node>
result<tref> api<node>::substitute(tref expr, tref that, tref with) {
	return with_budget<node>([&] {
		result<tref> r;
		if (!expr || !that || !with)
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		DBG(TAU_LOG_TRACE << "substitute: \n" << LOG_FM_DUMP(expr) << "\n" << LOG_FM_DUMP(that) << "\n" << LOG_FM_DUMP(with);)
		// Enforce that all three are consistently terms or formulas.
		// Mismatches would produce an ill-typed tree.
		bool e = is_term(expr), t = is_term(that), w = is_term(with);
		if ((e && e != t) || (e && e != w) || (!e && t != w))
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		tref s = tau::get(expr).substitute(that, with);
		if (!s) r.error(code::internal_error, "Substitution failed");
		else    r = s;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::substitute(tref expr, std::map<tref, tref> that_with) {
	return with_budget<node>([&] {
		result<tref> r;
		if (!expr)
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		// Validate every pair the way the single-pair overload does and
		// collect the pairs into a structurally keyed map (matching compares
		// subtrees, not pointers), then apply them all in one simultaneous
		// pass: every match is found against the original expression and no
		// pair's replacement is re-matched by another pair, so {x/y, y/x}
		// swaps instead of collapsing both variables into one.
		bool e = is_term(expr);
		subtree_map<node, tref> changes;
		for (auto [that, with] : that_with) {
			if (!that || !with)
				return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
			bool t = is_term(that), w = is_term(with);
			if ((e && e != t) || (e && e != w) || (!e && t != w))
				return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
			// two structurally equal match patterns are ambiguous
			if (!changes.emplace(that, with).second)
				return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		tref s = tau::get(expr).substitute(changes);
		if (!s) r.error(code::internal_error, "Substitution failed");
		else    r = s;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

// Normal forms
// ------------------------------------------------------------

template <NodeType node>
result<tref> api<node>::boole_normal_form(tref expr) {
	return with_budget<node>([&] {
		// Simplify, apply all registered definitions, then compute BNF
		result<tref> r;
		TAU_TRY(auto simplified, simplify(expr));
		TAU_TRY(auto applied, apply_all_defs(simplified));
		TAU_TRY(tref b, tau_lang::boole_normal_form<node>(applied));
		if (!b) r.error(code::internal_error, "Boole normal form conversion failed");
		else    r = b;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::dnf(tref expr) {
	return with_budget<node>([&] {
		result<tref> r;
		TAU_TRY(auto simplified, simplify(expr));
		TAU_TRY(tref a, apply_all_defs(simplified));
		tref d = nullptr;
		// Dispatch to bf-level or wff-level DNF depending on root type
		switch (tau::get(a).get_type()) {
		case tau::bf:  d = reduce<node>(to_dnf<node, false>(a)); break;
		case tau::wff: d = reduce<node>(to_dnf<node>(a)); break;
		default: r.error(code::invalid_argument, messages::invalid_arguments);
			DBG(assert(r.is_well_formed());)
			return r;
		}
		if (!d) r.error(code::internal_error, "DNF conversion failed");
		else    r = d;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::cnf(tref expr) {
	return with_budget<node>([&] {
		result<tref> r;
		TAU_TRY(auto simplified, simplify(expr));
		TAU_TRY(tref a, apply_all_defs(simplified));
		tref c = nullptr;
		// Dispatch to wff-level or bf-level CNF depending on root type
		switch (tau::get(a).get_type()) {
		case tau::wff: c = reduce<node, true>(to_cnf<node>(a)); break;
		case tau::bf:  c = reduce<node, true>(to_cnf<node, false>(a)); break;
		default: r.error(code::invalid_argument, messages::invalid_arguments);
			DBG(assert(r.is_well_formed());)
			return r;
		}
		if (!c) r.error(code::internal_error, "CNF conversion failed");
		else    r = c;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::nnf(tref expr) {
	return with_budget<node>([&] {
		result<tref> r;
		TAU_TRY(auto simplified, simplify(expr));
		TAU_TRY(tref a, apply_all_defs(simplified));
		tref n = nullptr;
		// wff: full NNF via De Morgan; bf: push negation into sub-terms
		switch (tau::get(a).get_type()) {
		case tau::wff: n = to_nnf<node>(a); break;
		case tau::bf:  n = push_negation_in<node, false>(a); break;
		default: r.error(code::invalid_argument, messages::invalid_arguments);
			DBG(assert(r.is_well_formed());)
			return r;
		}
		if (!n) r.error(code::internal_error, "NNF conversion failed");
		else    r = n;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::onf(tref expr, tref var) {
	return with_budget<node>([&] {
		result<tref> r;
		if (!var) return r.with_assert_check_error(
			code::invalid_argument, messages::invalid_arguments);
		// No simplify() here -- see the declaration's comment.
		TAU_TRY(tref a, apply_all_defs(expr));
		if (!tau::get(a).is(tau::wff)) {
			r.error(code::invalid_argument, "Invalid formula");
			DBG(assert(r.is_well_formed());)
			return r;
		}
		tref o = tau_lang::onf<node>(a, var);
		if (!o) r.error(code::internal_error, "ONF conversion failed");
		else    r = o;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::without(tref formula, tref clause) {
	return with_budget<node>([&] {
		result<tref> r;
		if (!formula || !clause) return r.with_assert_check_error(
			code::invalid_argument, messages::invalid_arguments);
		TAU_TRY(tref k, normalize_formula(formula));
		TAU_TRY(tref c, normalize_formula(clause));
		tref w = normal_form_without<node>(k, c);
		if (!w) r.error(code::internal_error, "Normalization failed");
		else    r = w;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::pnf(tref expr) {
	return with_budget<node>([&] {
		result<tref> r;
		TAU_TRY(auto simplified, simplify(expr));
		TAU_TRY(tref a, apply_all_defs(simplified));
		tref p = tau_lang::pnf<node>(a);
		if (!p) r.error(code::internal_error, "PNF conversion failed");
		else    r = p;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::mnf(tref expr) {
	return with_budget<node>([&] {
		result<tref> r;
		TAU_TRY(auto simplified, simplify(expr));
		TAU_TRY(tref a, apply_all_defs(simplified));
		tref m = nullptr;
		// Dispatch to wff-level or bf-level MNF depending on root type
		switch (tau::get(a).get_type()) {
		case tau::wff: m = unequal_to_not_equal<node>(reduce<node>(
				to_dnf<node>(bf_reduce_canonical<node>()(a)))); break;
		case tau::bf:  m = bf_reduced_dnf<node>(a); break;
		default: r.error(code::invalid_argument, messages::invalid_arguments);
			DBG(assert(r.is_well_formed());)
			return r;
		}
		if (!m) r.error(code::internal_error, "MNF conversion failed");
		else    r = m;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

// Procedures
// ------------------------------------------------------------

template <NodeType node>
result<tref> api<node>::syntactic_term_simplification(tref term) {
	return with_budget<node>([&] {
		result<tref> r;
		TAU_TRY(auto simplified, simplify(term));
		tref s = syntactic_path_simplification<node>(simplified);
		if (!s) r.error(code::internal_error, "Syntactic term simplification failed");
		else    r = s;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::syntactic_formula_simplification(tref fm) {
	return with_budget<node>([&] {
		result<tref> r;
		TAU_TRY(auto simplified, simplify(fm));
		tref s = tau_lang::syntactic_formula_simplification<node>(simplified);
		if (!s) r.error(code::internal_error, "Syntactic formula simplification failed");
		else    r = s;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::normalize_formula(tref fm) {
	return with_budget<node>([&] {
		result<tref> r;
		if (!fm) {
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		DBG(TAU_LOG_TRACE << "normalize_formula(): " << LOG_FM_DUMP(fm);)
	#ifdef DEBUG
		auto& defs = definitions<node>::instance();
		if (defs.size() == 0) std::cout << "Definitions: empty\n";
		else std::cout << "Definitions:\n";
		for (size_t i = 0; i < defs.size(); i++)
			std::cout << "    [" << i + 1 << "] "
				<< tau_lang::to_str<node>(defs[i])
				<< " -> " << TAU_LOG_FM_DUMP(defs[i].first->get())
				<< "\n";
		std::cout << *defs.get_io_context();
	#endif

		TAU_TRY(fm, simplify(fm));
		TAU_TRY(auto nso_rr, get_nso_rr(fm));
		if (!nso_rr.main || tau::get(nso_rr.main).is(tau::bf)) {
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		TAU_TRY_OR(r, normalizer<node>(nso_rr),
			code::internal_error, "Normalization failed");
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::normalize_term(tref term) {
	return with_budget<node>([&] {
		result<tref> r;
		if (!term) {
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		DBG(TAU_LOG_TRACE << "normalize_term(): " << LOG_FM_DUMP(term);)
		TAU_TRY(term, simplify(term));
		DBG(TAU_LOG_TRACE << "inferred term: " << LOG_FM_DUMP(term);)
		TAU_TRY(auto nso_rr, get_nso_rr(term));
		tref main = nso_rr.main->get();
		if (!main || !tau::get(main).is(tau::bf)) {
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		tref n = contains(main, tau::ref)
			? bf_normalizer_with_rec_relation<node>(nso_rr)
			: bf_normalizer_without_rec_relation<node>(main);
		if (!n) r.error(code::internal_error, "Normalization failed");
		else    r = n;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::anti_prenex(tref fm) {
	return with_budget<node>([&] {
		result<tref> r;
		if (!fm) {
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		TAU_TRY(auto simplified, simplify(fm));
		TAU_TRY(auto a, tau_lang::anti_prenex<node>(simplified));
		r = a;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<tref> api<node>::eliminate_quantifiers(tref fm) {
	return with_budget<node>([&] {
		result<tref> r;
		if (!fm) {
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		// Quantifier elimination works on a formula; a term given here
		// used to come back unchanged as if it had been eliminated.
		if (!tau::get(fm).is(tau::wff)) {
			return r.with_assert_check_error(code::invalid_argument, "Invalid formula");
		}
		TAU_TRY(auto simplified, simplify(fm));
		TAU_TRY(auto applied, apply_all_defs(simplified));
		TAU_TRY(auto prenexed, tau_lang::anti_prenex<node>(applied));
		TAU_TRY(auto e, resolve_quantifiers<node>(prenexed));
		r = e;
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

// Whole-query fast path: hand a non-temporal query the owning BA can decide to
// its own solver instead of quantifier elimination + preprocessing, which
// explodes on free variables under a quantifier. Sound only on the ENTIRE
// top-level query (sat(fm) is exactly exists-free-vars. fm), and only a
// definite answer is trusted -- an undecided one falls through unchanged.

/// True iff `fm` is non-temporal and the owning BA can solve it whole.
template <NodeType node>
bool is_whole_query_ba_solvable(tref fm) {
	using tau = tree<node>;
	if constexpr (!pack_has_arithmetic_theory_v<node>) return false;
	else return fm
		&& !tau::get(fm).find_top(is_temporal_quantifier<node>)
		&& pack_can_solve<node>(fm);
}

/// Fast path for sat/unsat; nullopt when it does not apply or is undecided.
template <NodeType node>
std::optional<bool> ba_fast_path_sat(tref fm) {
	if constexpr (!pack_has_arithmetic_theory_v<node>) return std::nullopt;
	else {
	if (!is_whole_query_ba_solvable<node>(fm)) return std::nullopt;
	// nullopt already covers both translation failure and an unknown answer.
	auto status = pack_sat_status<node>(fm);
	// A spec without a temporal wrapper is implicitly `always`, while the
	// one-step query reads each stream atom (o1[t], o1[t-1], i1[t]) as an
	// independent existential: steps lose their link and the solver picks
	// the inputs. That relaxation keeps unsat sound but not sat.
	if (status.has_value() && *status && has_temp_var<node>(fm))
		return std::nullopt;
	return status;
	}
}

/// A formula, or a spec whose main part is a formula: the roots the
/// decision procedures accept (normalize_formula unwraps a spec).
template <NodeType node>
bool is_formula_or_spec_root(tref fm) {
	using tau = tree<node>;
	using tt = tau::traverser;
	if (!fm) return false;
	const auto& t = tau::get(fm);
	return t.is(tau::wff)
		|| (t.is(tau::spec) && (tt(fm) | tau::main | tau::wff | tt::ref));
}

/// Fast path for validity: fm is valid iff !fm is unsat.
template <NodeType node>
std::optional<bool> ba_fast_path_valid(tref fm) {
	using tau = tree<node>;
	if constexpr (!pack_has_arithmetic_theory_v<node>) return std::nullopt;
	else {
	// Only a formula can be negated; anything else is left to the caller's
	// own shape check (issue #132).
	if (!fm || !tau::get(fm).is(tau::wff)) return std::nullopt;
	if (!is_whole_query_ba_solvable<node>(fm)) return std::nullopt;
	// valid iff the negation is unsat, so the answer inverts.
	if (auto sat = pack_sat_status<node>(tau::build_wff_neg(fm)))
		return !*sat;
	return std::nullopt;
	}
}

template <NodeType node>
result<bool> api<node>::realizable(tref fm) {
	return with_budget<node>([&] {
		result<bool> r;
		TAU_TRY(auto simplified, simplify(fm));
		// G(A) ∧ G(B) ≡ G(A ∧ B): merge top-level G-conjuncts before
		// normalization so the downstream pipeline sees a single wff_always.
		fm = flatten_always_conjuncts<node>(simplified);
		// get_spec_or_term yields a spec for any formula, and normalize_formula
		// unwraps it, so a spec root is as decidable as the formula it wraps.
		if (!is_formula_or_spec_root<node>(fm)) {
			return r.with_assert_check_error(code::invalid_argument, "Invalid formula");
		}
		// Whole-query BA fast path; falls through when undecided. It decides
		// SATISFIABILITY (every stream chosen existentially), which equals
		// realizability only when no input stream is involved: over inputs it
		// answered `(o1:bv[1] = 1) && (i1:bv[1] = 0)` REALIZABLE, although the
		// environment owns i1 (found by the CROSS-bv1 fuzz suite).
		if (!atom_has_any_input<node>(fm) && !has_ctl_star_operators<node>(fm))
			if (auto fast = ba_fast_path_sat<node>(fm); fast.has_value()) {
				return r.with_assert_check_value(fast.value());
			}
		// normalize_formula() fails on failures that are reachable from user
		// input: a non-well-founded recurrence, a definition set whose
		// expansion never settles, a fallback type mismatch, or a get_nso_rr
		// failure. is_tau_formula_sat() dereferences its argument immediately,
		// so the failure has to be caught here; it decides unsatisfiable
		// rather than propagating an error, matching the definite answer
		// every other undecidable-shape gate in this function returns.
		TAU_TRY_OR(tref nf, normalize_formula(fm),
			code::internal_error,
			"Could not normalize the formula; "
			"its satisfiability cannot be decided");
		// LT-7: the synthesis backend reports "no verdict" as a result<T>
		// error, not an UNREALIZABLE answer; is_ltl_aba_realizable propagates
		// it through r below, same as any other error.
		// A data quantifier under a full-LTL operator survives normalization;
		// feeding that residue to is_tau_formula_sat breaks its no-quantifier
		// invariant, so route the RAW formula to the LTL-ABA solver instead.
		tref target = (realizability_has_game_operators<node>(fm)
			&& tau::get(nf).find_top(is_quantifier<node>))
			? fm : nf;
		if (has_ctl_star_operators<node>(fm)) {
			r = is_ctl_star_realizable<node>(target, 0, true);
		} else if (realizability_has_game_operators<node>(fm)) {
			// is_tau_formula_sat now answers satisfiability only,
			// where an unrealizable full-LTL formula is undecided
			// rather than false; realizable() needs the real
			// verdict, so ask the realizability procedure directly
			// instead of going through it.
			r = is_ltl_aba_realizable<node>(target, 0, true);
		} else if (auto s = is_formula(fm) ? sat_prepared(fm) : result<bool>();
			s.has_value() && !s.value()) {
			// (a spec root is not a formula: sat() used to reject it with
			// an error here, which fell through the same way)
			// unsat(fm) => unrealizable(fm): reject without running
			// synthesis. An undecided sat (error) is not a decided
			// false, so it falls through to the real check below.
			r = false;
		} else {
			// realizable() needs the real verdict, and only
			// is_ltl_aba_realizable can report a fragment it has no
			// game construction for.
			r = is_ltl_aba_realizable<node>(target, 0, true);
		}
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<bool> api<node>::unrealizable(tref fm) {
	return with_budget<node>([&] {
		result<bool> r;
		// AP1-11: null input is invalid, not "unrealizable".
		if (!fm) {
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		TAU_TRY(auto real, realizable(fm));
		return r.with_assert_check_value(!real);
	});
}

template <NodeType node>
result<bool> api<node>::sat(tref fm) {
	return with_budget<node>([&] {
		result<bool> r;
		TAU_TRY(auto simplified, simplify(fm));
		// A spec root (what get_spec yields; realizable and valid take it
		// too) is unwrapped, its definitions applied, to its main formula.
		if (simplified && tau::get(simplified).is(tau::spec)) {
			TAU_TRY(simplified, apply_all_defs(simplified));
		}
		// G(A) ∧ G(B) ≡ G(A ∧ B); merge top-level conjunctions of G so the
		// downstream safety pipeline sees one wff_always.  Non-mergeable
		// Boolean combinations (disjunction, negation, F-on-non-singletons,
		// etc.) survive flatten unchanged and are routed to the full-LTL
		// pipeline by is_tau_formula_sat itself — there's no longer a
		// pre-check that rejects them at this layer.
		fm = flatten_always_conjuncts<node>(simplified);
		if (!fm || !is_formula(fm)) {
			return r.with_assert_check_error(code::invalid_argument, "Invalid formula");
		}
		return sat_prepared(fm);
	});
}

// Mode of api::sat_factored, the environment variable taking precedence;
// selected through the environment, the shadow mode reports at exit.
template <NodeType node>
static int sat_factored_mode() {
	static const std::optional<int> env = []() -> std::optional<int> {
		const char* v = std::getenv("TAU_API_SAT_FACTORED");
		if (!v || !*v) return std::nullopt;
		return v[0] == '2' ? 2 : v[0] == '1' ? 1 : 0;
	}();
	static const bool report = env && *env == 2 && std::atexit([]() {
		std::fprintf(stderr, "api sat factored shadow: hits %zu,"
			" mismatches %zu\n", api<node>::sat_factored_hits,
			api<node>::sat_factored_mismatches);
	}) == 0;
	(void) report;
	return env ? *env : api<node>::sat_factored;
}

// sat() after its simplify/flatten prefix. realizable() has already paid
// that prefix (simplify is a full type-inference traversal with no memo)
// when it asks for the unsat shortcut, so it enters here directly.
template <NodeType node>
result<bool> api<node>::sat_prepared(tref fm) {
	return with_budget<node>([&] {
		result<bool> r;
		// whole-query BA fast path; falls through when undecided.
		if (auto fast = ba_fast_path_sat<node>(fm); fast.has_value()) {
			return r.with_assert_check_value(fast.value());
		}
		// Same contract as realizable() above: a normalization failure
		// decides unsatisfiable rather than propagating an error.
		TAU_TRY_OR(tref nf, normalize_formula(fm),
			code::internal_error,
			"Could not normalize the formula; "
			"its satisfiability cannot be decided");
		// Same synthesis-backend gate as realizable() -- see the note there:
		// is_tau_formula_sat's own result<T> error propagates through r.
		// A data quantifier under a full-LTL operator survives normalization;
		// feeding that residue to is_tau_formula_sat breaks its no-quantifier
		// invariant, so route the RAW formula to the LTL-ABA solver instead.
		tref target = (sat_has_ltl_operators<node>(fm)
			&& tau::get(nf).find_top(is_quantifier<node>))
			? fm : nf;
		// A conjunction is satisfiable exactly when each of its
		// variable-disjoint components is (a trace for the whole is the
		// traces of the components side by side), and the components are
		// the same across queries that share their conjuncts: a query
		// that conjoins one clause to a held formula decides that
		// clause's component and finds the others remembered.
		// factored_tau_sat is the per-component decision the Tau-BA
		// constants already use: it splits `always` hulls into their
		// conjuncts, groups the conjuncts by the names of their free
		// variables and decides each group with is_tau_formula_sat. It
		// declines (-1) unless the formula is a conjunction of at least
		// two conjuncts, on a conjunct holding an embedded BA constant or
		// a nameless free variable, on fewer than two groups and on a
		// group is_tau_formula_sat leaves undecided; the whole-formula
		// decision below then runs as before. A full-LTL formula whose
		// normal form keeps a data quantifier is routed raw (target != nf)
		// and skips the factoring.
		const int factored = target == nf && ba_component_factoring_enabled()
			? sat_factored_mode<node>() : 0;
		if (factored > 0)
			if (int f = factored_tau_sat<node>(target); f >= 0) {
				++sat_factored_hits;
				if (factored == 1)
					return r.with_assert_check_value(f == 1);
				TAU_TRY_OR(r, is_tau_formula_sat<node>(target, 0, true),
					code::internal_error,
					"is_tau_formula_sat returned neither a value "
					"nor an error while checking satisfiability");
				if (!r.has_value() || r.value() != (f == 1))
					++sat_factored_mismatches;
				return r;
			}
		TAU_TRY_OR(r, is_tau_formula_sat<node>(target, 0, true),
			code::internal_error,
			"is_tau_formula_sat returned neither a value nor an "
			"error while checking satisfiability");
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<bool> api<node>::unsat(tref fm) {
	return with_budget<node>([&] {
		result<bool> r;
		// AP1-11: null input is invalid, not "unsatisfiable" (see
		// unrealizable above).
		if (!fm) {
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		TAU_TRY(auto s, sat(fm));
		return r.with_assert_check_value(!s);
	});
}

/// The operands of fm's top-level `&&`, each `always (A && B)` split into
/// `always A` and `always B`, parentheses peeled.
template <NodeType node>
void collect_core_conjuncts(tref fm, trefs& out) {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	if (!t.has_child()) { out.push_back(fm); return; }
	const auto nt = t[0].value.nt;
	if (nt == tau::wff_parenthesis) {
		collect_core_conjuncts<node>(t[0].first(), out);
	} else if (nt == tau::wff_and) {
		collect_core_conjuncts<node>(t[0].first(), out);
		collect_core_conjuncts<node>(t[0].second(), out);
	} else if (nt == tau::wff_always) {
		trefs body;
		collect_core_conjuncts<node>(t[0].first(), body);
		for (tref b : body) {
			// `always` of an `always` conjunct is the conjunct itself
			const auto& bt = tau::get(b);
			out.push_back(bt.has_child()
				&& bt[0].value.nt == tau::wff_always
				? b : tau::build_wff_always(b));
		}
	} else out.push_back(fm);
}

template <NodeType node>
result<trefs> api<node>::unsat_core(tref fm, bool realizability) {
	return with_budget<node>([&] {
		result<trefs> r;
		if (!fm) return r.with_assert_check_error(
			code::invalid_argument, messages::invalid_arguments);
		TAU_TRY(fm, simplify(fm));
		if (fm && tau::get(fm).is(tau::spec)) {
			TAU_TRY(fm, apply_all_defs(fm));
		}
		if (!fm || !is_formula(fm)) return r.with_assert_check_error(
			code::invalid_argument, "Invalid formula");
		trefs cs;
		collect_core_conjuncts<node>(fm, cs);
		// An undecided sub-check is a losing candidate, not a failure of
		// the core: its error is demoted to a warning in its own scope and
		// its conjunct stays.
		size_t checks = 0;
		auto conflicts = [&](const trefs& part) -> std::optional<bool> {
			auto sg = r.open("unsat_core check", code::info_count,
				part.size());
			++checks;
			tref conj = tau::build_wff_and(part);
			auto v = realizability ? realizable(conj) : sat(conj);
			if (!v.has_value()) {
				std::ostringstream os;
				os << v.report();
				r.warning("no verdict for a subset of "
					+ std::to_string(part.size())
					+ " conjuncts; the core may not be minimal: "
					+ os.str());
				return std::nullopt;
			}
			return !*r.merge_take(std::move(v));
		};
		auto whole = realizability ? realizable(fm) : sat(fm);
		const std::optional<bool> decided = whole.has_value()
			? std::optional<bool>(whole.value()) : std::nullopt;
		r.merge(std::move(whole));
		++checks;
		if (!decided) return r.with_error(code::solver_error, "UNKNOWN: "
			"the specification gets no verdict, so it has no core");
		if (*decided) return r.with_assert_check_value(trefs{});
		// Deletion: drop cs[i] when the rest still conflicts. Monotone,
		// so what stays is subset-minimal (bar undecided sub-checks).
		for (size_t i = 0; i < cs.size() && cs.size() > 1; ) {
			trefs rest;
			for (size_t j = 0; j < cs.size(); ++j)
				if (j != i) rest.push_back(cs[j]);
			if (auto c = conflicts(rest); c && *c) {
				cs = std::move(rest);
				continue;
			}
			++i;
		}
		r.report().count("unsat_core checks", checks);
		return r.with_assert_check_value(std::move(cs));
	});
}

template <NodeType node>
result<bool> api<node>::valid(tref fm) {
	return with_budget<node>([&] {
		result<bool> r;
		TAU_TRY(auto simplified, simplify(fm));
		fm = flatten_always_conjuncts<node>(simplified);
		if (!fm) {
			return r.with_assert_check_error(code::invalid_argument, "Invalid formula");
		}
		TAU_TRY(auto vs, valid_spec(fm));
		return r.with_assert_check_value(vs);
	});
}

template <NodeType node>
result<bool> api<node>::valid_spec(tref fm) {
	return with_budget<node>([&] {
		result<bool> r;
		TAU_TRY(fm, simplify(fm));
		// valid() and valid_spec() are both public: the shape check sits
		// here so neither can hand a term to formula negation or to a
		// backend that expects a Boolean (issue #132: `valid x:bv[1]`
		// aborted on a cvc5 exception).
		if (!is_formula_or_spec_root<node>(fm)) {
			return r.with_assert_check_error(code::invalid_argument, "Invalid formula");
		}
		// Validity of a CTL* formula quantifies over every computation tree,
		// which no procedure here decides; is_tau_impl would read the opaque
		// A / E / - nodes as "not valid".
		if (has_ctl_star_operators<node>(fm)) {
			return r.with_error(code::solver_error,
				"UNKNOWN: validity of a formula with CTL* operators "
				"(A / E / semantic negation) cannot be decided");
		}
		// valid(φ) holds when no trace violates φ. sat of full LTL is
		// realizability, a game against the inputs, so it is asked about ¬φ
		// with every input read as an output: with no input left, realizable
		// is "some trace satisfies". This keeps valid φ implying sat φ, which
		// unsat(¬φ) alone would not (¬φ unrealizable does not make φ
		// realizable).
		if (sat_has_ltl_operators<node>(fm)) {
			// A spec root is unwrapped, with its definitions applied,
			// before it is negated: sat takes a formula, and negating the
			// spec node itself wraps it instead of its main formula.
			tref main = fm;
			if (tau::get(fm).is(tau::spec)) {
				TAU_TRY(main, apply_all_defs(fm));
				if (!main || !tau::get(main).is(tau::wff))
					return r.with_assert_check_error(
						code::invalid_argument, "Invalid formula");
			}
			auto s = sat(inputs_as_outputs<node>(tau::build_wff_neg(main)));
			if (!s.has_value()) {
				r.merge(std::move(s));
				return r.with_error(code::solver_error,
					"UNKNOWN: validity of this full-LTL formula could not "
					"be decided");
			}
			return r.with_assert_check_value(!s.value());
		}
		// whole-query BA fast path; falls through when undecided.
		if (auto fast = ba_fast_path_valid<node>(fm); fast.has_value()) {
			return r.with_assert_check_value(fast.value());
		}
		// Same contract as realizable(): is_tau_impl() normalizes both
		// arguments straight away and cannot be handed a null formula, so a
		// normalization failure decides invalid rather than propagating an
		// error.
		TAU_TRY_OR(tref nfm, normalize_formula(fm),
			code::internal_error,
			"Could not normalize the formula; "
			"its validity cannot be decided");
		// Valid iff T (tautology) implies the normalized formula, i.e. iff
		// no trace violates it. is_tau_impl checks unsat of the negation
		// with the inputs quantified universally, which only says the
		// system cannot force the negation; every input is read as an
		// output, as in the full-LTL branch above.
		nfm = inputs_as_outputs<node>(nfm);
		// Same synthesis-failure gate as realizable() -- see the note there:
		// is_tau_impl's own result<T> error propagates through r.
		TAU_TRY_OR(r, is_tau_impl<node>(tau::_T(), nfm),
			code::internal_error,
			"is_tau_impl returned neither a value nor an error "
			"while checking validity");
		DBG(assert(r.is_well_formed());)
		return r;
	});
}


// Solving
// ------------------------------------------------------------

// Solve: apply defs, reject temporal quantifiers, then run the solver.
template <NodeType node>
result<subtree_map<node, tref>> api<node>::solve(
	tref fm, solver_mode mode)
{
	return with_budget<node>([&] {
		result<subtree_map<node, tref>> r;
		// The solver takes a formula; a term has nothing to solve.
		if (!fm || !tau::get(fm).is(tau::wff)) {
			return r.with_assert_check_error(code::invalid_argument, "Invalid formula");
		}
		auto simplified_v = r.merge_take(simplify(fm));
		if (!simplified_v) {
			DBG(assert(r.is_well_formed());)
			return r;
		}
		auto applied_v = r.merge_take(apply_all_defs(*simplified_v));
		if (!applied_v) {
			DBG(assert(r.is_well_formed());)
			return r;
		}
		tref a = *applied_v;
		// Reject formula involving temporal quantification
		if (tau::get(a).find_top(is_temporal_quantifier<node>))
			return r.with_assert_check_error(code::invalid_argument,
				"Found temporal quantifier in formula");
		DBG(TAU_LOG_TRACE << "solve: " << LOG_FM(a);)
		// setting solver options
		solver_options options = {
			.splitter_one = node::ba::
				splitter_one(tau_type<node>()),
			.mode = mode
		};
		// Use fully-expanded formula (a) so function/predicate refs are resolved
		// before reaching type-specific solvers (fixes bug with typed functions).
		TAU_TRY_OR(r, tau_lang::solve<node>(a, options), code::internal_error,
			"tau_lang::solve returned neither a value nor an error");
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

// LGRS: normalize the equation, extract the bf_eq equality, then solve.
template <NodeType node>
result<subtree_map<node, tref>> api<node>::lgrs(tref equation) {
	return with_budget<node>([&] {
		result<subtree_map<node, tref>> r;
		using tt = tau::traverser;
		if (!equation || !tau::get(equation).is(tau::wff)) {
			return r.with_assert_check_error(code::invalid_argument, "Invalid formula");
		}
		TAU_TRY(auto simplified, simplify(equation));
		auto applied_v = r.merge_take(apply_all_defs(simplified));
		if (!applied_v) {
			DBG(assert(r.is_well_formed());)
			return r;
		}
		tref a = *applied_v;
		tref eq = apply_all_xor_def<node>(norm_all_equations<node>(a));
		tref equality = tt(eq) | tau::bf_eq | tt::ref;
		if (!eq || !equality)
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		// Exclude non-Boolean operations from equation. The two sides live under
		// the bf_eq, not under `eq`: `eq` is the wff wrapping it and has a
		// single child, so indexing it with [1] tripped the `c != nullptr`
		// assert in tree<node>::child_tree (Debug) and read a null child
		// (Release).
		if (tau::get(equality)[0].find_top(is_non_boolean_term<node>) ||
			tau::get(equality)[1].find_top(is_non_boolean_term<node>))
			return r.with_assert_check_error(code::invalid_argument,
				"Found non-Boolean operation in equation");

		DBG(TAU_LOG_TRACE << "lgrs/applied: " << LOG_FM(eq);)
		DBG(TAU_LOG_TRACE << "lgrs/equality: " << LOG_FM(equality);)

		TAU_TRY_OR(r, tau_lang::lgrs<node>(eq), code::internal_error,
			"tau_lang::lgrs returned neither a value nor an error");
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

// Execution
// ------------------------------------------------------------

template <NodeType node>
result<interpreter<node>> api<node>::get_interpreter(tref spec) {
	return with_budget<node>([&] {
		interpreter_options options;
		return get_interpreter(spec, options);
	});
}

template <NodeType node>
result<interpreter<node>> api<node>::get_interpreter(tref spec,
	interpreter_options& options)
{
	return with_budget<node>([&] {
		result<interpreter<node>> r;
		// Assign the remaps into the global io_context only after every
		// validation step succeeds: assigning them up front left them in
		// place -- corrupting later, unrelated calls -- on every one of the
		// early-return failure paths below.
		auto& ctx = *definitions<node>::instance().get_io_context();
		TAU_TRY(auto nso_rr, get_nso_rr(spec));
		TAU_TRY_OR(tref applied, nso_rr_apply<node>(nso_rr),
			code::internal_error, "Failed to apply definitions");
		TAU_TRY_OR(tref normalized, normalizer<node>(applied),
			code::internal_error, "Normalization failed");
		// normalized is ctx-resolved (get_nso_rr), never a bare-reparsed atom.
		if (has_free_vars<node>(normalized)) {
			return r.with_assert_check_error(code::invalid_argument, "Spec contains free variables");
		}
		ctx.input_remaps = options.input_remaps;
		ctx.output_remaps = options.output_remaps;
		// LT-7: make_interpreter reaches ltlsynt through
		// ltl_to_safety_formula_full; a backend failure must not terminate the
		// caller.  No interpreter is the honest answer here, and
		// make_interpreter's own result<T> error propagates through r.
		TAU_TRY_OR(r, interpreter<node>::make_interpreter(normalized, ctx),
			code::solver_error,
			"the specification could not be compiled");
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

template <NodeType node>
result<interpreter<node>> api<node>::get_interpreter(
	tau_spec<node>& spec)
{
	return with_budget<node>([&] {
		interpreter_options options;
		return get_interpreter(spec, options);
	});
}

template <NodeType node>
result<interpreter<node>> api<node>::get_interpreter(
	tau_spec<node>& spec,
	interpreter_options& options)
{
	return with_budget<node>([&] {
		result<interpreter<node>> r;
		// See the tref overload above: remaps are assigned into the global
		// io_context only once every validation step has succeeded.
		auto& ctx = *definitions<node>::instance().get_io_context();
		auto maybe_nso_rr = spec.get_nso_rr();
		if (!maybe_nso_rr) {
			for (const auto& error : spec.errors())
				r.error(code::parse_error, error);
			if (!r.has_error()) r.error(code::parse_error, "Failed to parse spec");
			DBG(assert(r.is_well_formed());)
			return r;
		}
		TAU_TRY_OR(tref applied, nso_rr_apply<node>(maybe_nso_rr.value()),
			code::internal_error, "Failed to apply definitions");
		TAU_TRY_OR(tref normalized, normalizer<node>(applied),
			code::internal_error, "Normalization failed");
		// normalized is ctx-resolved (get_nso_rr), never a bare-reparsed atom.
		if (has_free_vars<node>(normalized)) {
			return r.with_assert_check_error(code::invalid_argument, "Spec contains free variables");
		}
		ctx.input_remaps = options.input_remaps;
		ctx.output_remaps = options.output_remaps;
		// See the tref overload: make_interpreter's own result<T> error
		// propagates through r rather than terminating the caller.
		TAU_TRY_OR(r, interpreter<node>::make_interpreter(normalized, ctx),
			code::solver_error,
			"the specification could not be compiled");
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

// private helper methods
// ------------------------------------------------------------

// Private: extract rr<node> from expression tree.
// For spec nodes, delegates to tau_lang::get_nso_rr.
// For bare wff/bf nodes, wraps via resolve_io_vars.
template <NodeType node>
result<rr<node>> api<node>::get_nso_rr(tref expr) {
	return with_budget<node>([&] {
		result<rr<node>> r;
		if (!expr) {
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		rr<node> nso_rr;
		// AP1-16: by reference -- copying the io_context (three subtree maps
		// + remaps + console factory) per call was pure waste; all uses read.
		auto& ctx = *definitions<node>::instance().get_io_context();
		// A spec root is always unwrapped, whether or not it holds a ref: a
		// spec handed whole to the normalizer as its main formula is negated
		// as if it were a wff by the syntactic simplifier.
		if (tau::get(expr).is(tau::spec)) {
			if (auto mayb_nso_rr = tau_lang::get_nso_rr<node>(
				ctx, expr); mayb_nso_rr)
					nso_rr = mayb_nso_rr.value();
			else {
				return r.with_assert_check_error(code::internal_error,
					"Failed to resolve recurrence relations");
			}
		} else {
			nso_rr.main = tau::geth(resolve_io_vars<node>(ctx, expr));
			if (!nso_rr.main) {
				return r.with_assert_check_error(code::internal_error,
					"Failed to resolve I/O variables");
			}
		}
		return r.with_assert_check_value(std::move(nso_rr));
	});
}

// Type inference pipeline: infer BA types → canonize quantifier IDs
// → unnest G-in-G ambiguity → check semantics → update global scope.
template <NodeType node>
result<tref> api<node>::infer(tref expr, bool use_defaults) {
	return with_budget<node>([&] {
		result<tref> r;
		if (!expr) {
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}

		auto& defs = definitions<node>::instance();
		auto infer_result = infer_ba_types<node>(expr,
			defs.get_global_scope(),
			defs.get_definition_heads(),
			{ .use_defaults = use_defaults });
		tref inferred = canonize_quantifier_ids<node>(infer_result.first);
		if (!inferred) {
			DBG(LOG_TRACE << "inferred is nullptr";)
			return r.with_assert_check_error(code::internal_error, "Type inference failed");
		}
		defs.get_io_context()->update_types(infer_result.second);
		defs.merge_global_scope(infer_result.second);

		// Rewrite G(A && G(B)) → G(A) && G(B) before the semantic error check.
		// This arises because the CFG parser is ambiguous: G(X) && G(Y) can
		// be parsed as either G(X) && G(Y) (correct) or G(X && G(Y)) (nested).
		// G(A && G(B)) = G(A) && G(B) semantically, so this rewrite is safe.
		// Only applies when full-LTL operators (F/U/R/W) are absent; with LTL
		// operators the nesting check is already bypassed.
		inferred = unnest_nested_always<node>(inferred);

		//Check for semantic errors in expression
		TAU_TRY(auto sem_error, has_semantic_error<node>(inferred));
		if (sem_error) {
			DBG(LOG_TRACE << "transformed has semantic error";)
			return r.with_assert_check_error(code::invalid_argument, "Expression has semantic error");
		}
		DBG(TAU_LOG_TRACE << "inferred: " << LOG_FM_DUMP(inferred);)
		#ifdef DEBUG
		if (defs.size() == 0) std::cout << "Definitions: empty\n";
		else std::cout << "Definitions:\n";
		for (size_t i = 0; i < defs.size(); i++)
			std::cout << "    [" << i + 1 << "] "
				<< tau_lang::to_str<node>(defs[i])
				<< " -> " << TAU_LOG_FM_DUMP(defs[i].first->get())
				<< "\n";
		std::cout << *defs.get_io_context();
	#endif
		return r.with_assert_check_value(inferred);
	});
}

template <NodeType node>
result<tref> api<node>::simplify(tref expr, bool use_defaults) {
	return with_budget<node>([&] {
		result<tref> r;
		if (!expr) {
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		TAU_TRY(auto inferred, infer(expr, use_defaults));
		tref e = canonize_quantifier_ids<node>(tau::reget(inferred));
		if (!e) r.error(code::internal_error, "Simplification failed");
		else {
			DBG(TAU_LOG_TRACE << "simplified: " << LOG_FM_DUMP(e);)
			r = e;
		}
		DBG(assert(r.is_well_formed());)
		return r;
	});
}

} // namespace idni::tau_lang
