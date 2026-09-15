// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "api.h"

#include "sbf_parser.generated.h"
#include "tau_tree_builders.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "api"

namespace idni::tau_lang {

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
	sbf_parser::instance().get_grammar().set_enabled_productions(guards);
}

template <NodeType node>
void api<node>::set_blasting(bool blasting) {
	bv_blasting = blasting;
}

template <NodeType node>
void api<node>::set_bv_widening(bool widening) { bv_widening = widening; }

template <NodeType node>
void api<node>::set_bv_max_width(size_t width) {
	if (width > 0) bv_max_width = width;
}

template <NodeType node>
void api<node>::set_blast_placement(int site) {
	blast_placement = (site >= static_cast<int>(blast_site::per_leaf)
		&& site <= static_cast<int>(blast_site::per_formula))
			? static_cast<blast_site>(site) : blast_site::per_leaf;
}

template <NodeType node>
void api<node>::set_blast_method(int mode) {
	blast_method = (mode >= static_cast<int>(blast_mode::anti_prenex_result)
		&& mode <= static_cast<int>(blast_mode::defer))
			? static_cast<blast_mode>(mode)
			: blast_mode::anti_prenex_result;
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
void api<node>::set_bv_quantifier_free_decision(bool state) {
	bv_quantifier_free_decision = state;
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
void api<node>::set_spec_size_warn(size_t n) {
	interpreter<node>::spec_size_warn_threshold = n;
}

template <NodeType node>
void api<node>::set_max_revision_alts(size_t n) {
	interpreter<node>::max_revision_alts = n;
}

template <NodeType node>
void api<node>::set_indenting(bool indenting) {
	pretty_printer_indenting = indenting;
}

template <NodeType node>
void api<node>::set_ba_component_factoring(bool state) {
	ba_component_factoring = state;
}

template <NodeType node>
void api<node>::set_ba_decision_pins(size_t n) {
	ba_decision_pins = n;
}

template <NodeType node>
void api<node>::set_bv_case_split(bool state) {
	bv_case_split = state;
}

template <NodeType node>
void api<node>::set_bv_case_split_max_tests(size_t n) {
	bv_case_split_max_tests = n ? n : std::numeric_limits<size_t>::max();
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

// local helper function to get get_options based on simplified flag
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
	result<tref> r;
	tref e = tau::get(input, get_options<node>(tau::bf, simplified));
	if (!e) return r.with_error(code::parse_error, "Failed to parse term");
	return r.with_assert_check_value(e);
}

template <NodeType node>
result<tref> api<node>::get_formula(const std::string& input, bool simplified) {
	result<tref> r;
	tref fm = tau::get(input, get_options<node>(tau::wff, simplified));
	if (!fm) return r.with_error(code::parse_error, "Failed to parse formula");
	return r.with_assert_check_value(fm);
}

template <NodeType node>
result<tref> api<node>::get_function_def(const std::string& function_def, [[maybe_unused]] bool simplified) {
	result<tref> r;
	// Always simplify to resolve refs
	TAU_TRY(tref def, get_definition(function_def, true));
	auto nt = tau::get(def)[1].get_type();
	if (nt != tau::bf) {
		return r.with_error(code::invalid_argument, "Not a function definition");
	}
	return r.with_assert_check_value(def);
}

template <NodeType node>
result<tref> api<node>::get_predicate_def(const std::string& predicate_def, [[maybe_unused]] bool simplified) {
	result<tref> r;
	// Always simplify to resolve refs
	TAU_TRY(tref def, get_definition(predicate_def, true));
	auto nt = tau::get(def)[1].get_type();
	if (nt != tau::wff) {
		return r.with_error(code::invalid_argument, "Not a predicate definition");
	}
	return r.with_assert_check_value(def);
}

template <NodeType node>
result<tref> api<node>::get_stream_def(const std::string& stream_def) {
	result<tref> r;
	tref def = tau::get(stream_def, get_options<node>(tau::stream_def, true));
	if (!def) return r.with_error(code::parse_error, "Failed to parse stream definition");
	return r.with_assert_check_value(tau::trim(def));
}

template <NodeType node>
result<tref> api<node>::get_spec(const std::string& src) {
	result<tref> r;
	tau_spec<node> spec;
	if (!spec.parse(src)) {
		for (const auto& error : spec.errors()) {
			TAU_LOG_ERROR << error;
			r.error(code::parse_error, error);
		}
		if (r.has_error()) {
			DBG(assert(r.is_well_formed());)
			return r;
		}
		return r.with_error(code::parse_error,
			messages::failed_to_parse_spec);
	}
	tref s = spec.get();
	if (!s) return r.with_error(code::parse_error,
		messages::failed_to_parse_spec);
	return r.with_assert_check_value(s);
}

template <NodeType node>
result<size_t> api<node>::add_definition(tref head, tref body) {
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
		return r.with_error(code::invalid_argument);
	}
	DBG(TAU_LOG_TRACE << "add_definition/adding head: " << LOG_FM_DUMP(head);)
	DBG(TAU_LOG_TRACE << "add_definition/adding body: " << LOG_FM_DUMP(body);)
	return r.with_assert_check_value(definitions<node>::instance().add(tau::geth(head), tau::geth(body)));
}

template <NodeType node>
result<tref> api<node>::get_definition(const std::string& definition, bool simplified) {
	result<tref> r;
	DBG(TAU_LOG_TRACE << "get_definition/definition: " << definition;)
	tref def = tau::get(definition, get_options<node>(tau::rec_relation, simplified));
	DBG(TAU_LOG_TRACE << "get_definition/def: " << LOG_FM_DUMP(def);)
	if (!def) {
		return r.with_error(code::parse_error, "Failed to parse definition");
	}
	r.merge(add_definition(tau::get(def).first(), tau::get(def).second()));
	if (r.has_error()) {
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return r.with_assert_check_value(def);
}

template <NodeType node>
result<tref> api<node>::get_spec_or_term(const std::string& expression, bool simplified) {
	result<tref> r;
	// try multiline first (includes a formula too), if it fails try just a term
	auto spec = get_spec(expression);
	if (spec.has_value()) {
		return r.with_assert_check_value(spec.value());
	}
	auto term = get_term(expression, simplified);
	if (!term.has_value()) {
		// both attempts failed: keep each one's diagnostics
		r.merge(std::move(spec)).merge(std::move(term));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return r.with_assert_check_value(term.value());
}

template <NodeType node>
result<tref> api<node>::get_formula_or_term(const std::string& expr, bool simplified) {
	result<tref> r;
	tref e = tau::get(expr, get_options<node>(tau::fm_or_term, simplified));
	if (!e) return r.with_error(code::parse_error, "Failed to parse formula or term");
	return r.with_assert_check_value(tau::trim(e));
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
tref api<node>::apply_def(tref def, tref expr) {
	return apply_defs(subtree_set<node>{ def }, expr);
}

template <NodeType node>
tref api<node>::apply_defs(subtree_set<node> defs, tref expr) {
	if (!expr) return nullptr;
	auto maybe_nso_rr = get_nso_rr(expr);
	if (!maybe_nso_rr) return nullptr;
	auto& nso_rr = maybe_nso_rr.value();
	io_context<node>& ctx = *definitions<node>::instance().get_io_context();
	for (tref def : defs) if (def) {
		const auto& t = tau::get(def);
		if (t.is(tau::rec_relation)) {
			nso_rr.rec_relations.emplace_back(
				tau::geth(t.first()),
				tau::geth(resolve_io_vars<node>(ctx, t.second())));
		}
	}
	return nso_rr_apply<node>(nso_rr);
}

template <NodeType node>
tref api<node>::apply_all_defs(tref expr) {
	return apply_defs(subtree_set<node>{}, expr);
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
tref api<node>::substitute(tref expr, tref that, tref with) {
	if (!expr || !that || !with) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		return nullptr;
	}
	DBG(TAU_LOG_TRACE << "substitute: \n" << LOG_FM_DUMP(expr) << "\n" << LOG_FM_DUMP(that) << "\n" << LOG_FM_DUMP(with);)
	bool e = is_term(expr), t = is_term(that), w = is_term(with);
	if ((e && e != t) || (e && e != w) || (!e && t != w)) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		return nullptr;
	}
	return tau::get(expr).substitute(that, with);
}

template <NodeType node>
tref api<node>::substitute(tref expr, std::map<tref, tref> that_with) {
	if (!expr) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		return nullptr;
	}
	// Validate every pair the way the single-pair overload does and
	// collect the pairs into a structurally keyed map (matching compares
	// subtrees, not pointers), then apply them all in one simultaneous
	// pass: every match is found against the original expression and no
	// pair's replacement is re-matched by another pair, so {x/y, y/x}
	// swaps instead of collapsing both variables into one.
	bool e = is_term(expr);
	subtree_map<node, tref> changes;
	for (auto [that, with] : that_with) {
		if (!that || !with) {
			TAU_LOG_ERROR << "Invalid argument(s)";
			return nullptr;
		}
		bool t = is_term(that), w = is_term(with);
		if ((e && e != t) || (e && e != w) || (!e && t != w)) {
			TAU_LOG_ERROR << "Invalid argument(s)";
			return nullptr;
		}
		// two structurally equal match patterns are ambiguous
		if (!changes.emplace(that, with).second) {
			TAU_LOG_ERROR << "Invalid argument(s)";
			return nullptr;
		}
	}
	return tau::get(expr).substitute(changes);
}

// Normal forms
// ------------------------------------------------------------

template <NodeType node>
tref api<node>::boole_normal_form(tref expr) {
	expr = simplify(expr);
	if (!expr) return nullptr;
	if (tref a = apply_all_defs(expr); a)
		return tau_lang::boole_normal_form<node>(a);
	return nullptr;
}

template <NodeType node>
tref api<node>::dnf(tref expr) {
	expr = simplify(expr);
	if (!expr) return nullptr;
	tref a = apply_all_defs(expr);
	if (a) {
		switch (tau::get(a).get_type()) {
		case tau::bf:  return reduce<node>(to_dnf<node, false>(a));
		case tau::wff: return reduce<node>(to_dnf<node>(a));
		default: return nullptr;
		}
	}
	return nullptr;
}

template <NodeType node>
tref api<node>::cnf(tref expr) {
	expr = simplify(expr);
	if (!expr) return nullptr;
	tref a = apply_all_defs(expr);
	if (a) {
		switch (tau::get(a).get_type()) {
		case tau::wff: return reduce<node, true>(to_cnf<node>(a));
		case tau::bf:  return reduce<node, true>(to_cnf<node, false>(a));
		default: return nullptr;
		}
	}
	return nullptr;
}

template <NodeType node>
tref api<node>::nnf(tref expr) {
	expr = simplify(expr);
	if (!expr) return nullptr;
	tref a = apply_all_defs(expr);
	if (a) {
		switch (tau::get(a).get_type()) {
		case tau::wff: return to_nnf<node>(a);
		case tau::bf:  return push_negation_in<node, false>(a);
		default: return nullptr;
		}
	}
	return nullptr;
}

// Procedures
// ------------------------------------------------------------

template <NodeType node>
tref api<node>::syntactic_term_simplification(tref term) {
	term = simplify(term);
	if (!term) return nullptr;
	return syntactic_path_simplification<node>(term);
}

template <NodeType node>
tref api<node>::syntactic_formula_simplification(tref fm) {
	fm = simplify(fm);
	if (!fm) return nullptr;
	return tau_lang::syntactic_formula_simplification<node>(fm);
}

template <NodeType node>
result<tref> api<node>::normalize_formula(tref fm) {
	result<tref> r;
	if (!fm) {
		return r.with_error(code::invalid_argument);
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

	fm = simplify(fm);
	if (!fm) {
		return r.with_error(code::internal_error, "simplification produced no formula");
	}
	auto maybe_nso_rr = get_nso_rr(fm);
	if (!maybe_nso_rr || !maybe_nso_rr.value().main
		|| tau::get(maybe_nso_rr.value().main).is(tau::bf))
	{
		return r.with_error(code::invalid_argument, "Not a formula");
	}
	TAU_TRY(tref out, normalizer<node>(maybe_nso_rr.value()));
	return r.with_assert_check_value(out);
}

template <NodeType node>
result<tref> api<node>::normalize_term(tref term) {
	result<tref> r;
	if (!term) {
		return r.with_error(code::invalid_argument);
	}
	DBG(TAU_LOG_TRACE << "normalize_term(): " << LOG_FM_DUMP(term);)
	term = simplify(term);
	if (!term) {
		return r.with_error(code::internal_error, "simplification produced no term");
	}
	DBG(TAU_LOG_TRACE << "inferred term: " << LOG_FM_DUMP(term);)
	auto maybe_nso_rr = get_nso_rr(term);
	if (!maybe_nso_rr) {
		return r.with_error(code::invalid_argument);
	}
	auto& nso_rr = maybe_nso_rr.value();
	tref main = nso_rr.main->get();
	if (!main || !tau::get(main).is(tau::bf)) {
		return r.with_error(code::invalid_argument, "Not a term");
	}
	tref out = contains(main, tau::ref)
		? bf_normalizer_with_rec_relation<node>(nso_rr)
		: bf_normalizer_without_rec_relation<node>(main);
	if (!out) return r.with_error(code::internal_error,
		"term normalization produced no term");
	return r.with_assert_check_value(out);
}

template <NodeType node>
tref api<node>::anti_prenex(tref fm) {
	if (!fm) return nullptr;
	fm = simplify(fm);
	if (!fm) return nullptr;
	return tau_lang::anti_prenex<node>(fm);
}

template <NodeType node>
tref api<node>::eliminate_quantifiers(tref fm) {
	if (!fm) return nullptr;
	fm = simplify(fm);
	if (!fm) return nullptr;
	if (tref a = apply_all_defs(fm); a)
		return resolve_quantifiers<node>(tau_lang::anti_prenex<node>(a));
	return nullptr;
}

template <NodeType node>
result<bool> api<node>::realizable(tref fm) {
	result<bool> r;
	fm = simplify(fm);
	if (!fm) {
		return r.with_error(code::invalid_argument);
	}
	// A spec whose main is a formula is as decidable as the formula itself
	// (normalize_formula unwraps it); the string overloads always arrive
	// here with a spec root.
	using tt = tau::traverser;
	const bool is_fm = is_formula(fm) || (tau::get(fm).is(tau::spec)
		&& (tt(fm) | tau::main | tau::wff | tt::ref));
	if (!is_fm) {
		return r.with_assert_check_value(false);
	}
	TAU_TRY(tref nfm, normalize_formula(fm));
	TAU_TRY(bool s, is_tau_formula_sat<node>(nfm, 0, true));
	return r.with_assert_check_value(s);
}

template <NodeType node>
result<bool> api<node>::unrealizable(tref fm) {
	result<bool> r;
	TAU_TRY(bool v, realizable(fm));
	return r.with_assert_check_value(!v);
}

template <NodeType node>
result<bool> api<node>::sat(tref fm) {
	result<bool> r;
	fm = simplify(fm);
	if (!fm) {
		return r.with_error(code::invalid_argument);
	}
	TAU_TRY(bool nb, has_no_boolean_combs_of_models<node>(fm));
	if (!nb) {
		return r.with_assert_check_value(false);
	}
	TAU_TRY(bool v, realizable(fm));
	return r.with_assert_check_value(v);
}

template <NodeType node>
result<bool> api<node>::unsat(tref fm) {
	result<bool> r;
	TAU_TRY(bool v, sat(fm));
	return r.with_assert_check_value(!v);
}

template <NodeType node>
result<bool> api<node>::valid(tref fm) {
	result<bool> r;
	fm = simplify(fm);
	if (!fm) {
		return r.with_error(code::invalid_argument);
	}
	TAU_TRY(bool nb, has_no_boolean_combs_of_models<node>(fm));
	if (!nb) {
		return r.with_assert_check_value(false);
	}
	TAU_TRY(bool v, valid_spec(fm));
	return r.with_assert_check_value(v);
}

template <NodeType node>
result<bool> api<node>::valid_spec(tref fm) {
	result<bool> r;
	fm = simplify(fm);
	if (!fm) {
		return r.with_error(code::invalid_argument);
	}
	TAU_TRY(tref nfm, normalize_formula(fm));
	TAU_TRY(bool v, is_tau_impl<node>(tau::_T(), nfm));
	return r.with_assert_check_value(v);
}


// Solving
// ------------------------------------------------------------

template <NodeType node>
result<subtree_map<node, tref>> api<node>::solve(
	tref fm, solver_mode mode)
{
	result<subtree_map<node, tref>> r;
	fm = simplify(fm);
	if (!fm) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		return r.with_error(code::invalid_argument);
	}
	tref a = apply_all_defs(fm);
	if (!a) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		return r.with_error(code::invalid_argument);
	}
	// Reject formula involving temporal quantification
	if (tau::get(a).find_top(is_temporal_quantifier<node>)) {
		TAU_LOG_ERROR << "Found temporal quantifier in formula: "
			<< TAU_TO_STR(fm);
		return r.with_error(code::unsupported_operation,
			"Found temporal quantifier in formula");
	}
	DBG(TAU_LOG_TRACE << "solve: " << LOG_FM(fm);)
	// setting solver options
	solver_options options = {
		.splitter_one = node::ba::
			splitter_one(tau_type<node>()),
		.mode = mode
	};
	r = tau_lang::solve<node>(fm, options);
	// keep the log line the solver-error path always had; an ordinary
	// "no solution" (code::unsat) stays quiet as before
	if (report_has_code(r.report(), code::solver_error))
		TAU_LOG_ERROR << "Internal error in solver";
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<subtree_map<node, tref>> api<node>::lgrs(tref equation) {
	using tt = tau::traverser;
	result<subtree_map<node, tref>> r;
	equation = simplify(equation);
	tref a = apply_all_defs(equation);
	if (!a) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		return r.with_error(code::invalid_argument);
	}
	tref eq = apply_all_xor_def<node>(norm_all_equations<node>(a));
	tref equality = tt(eq) | tau::bf_eq | tt::ref;
	if (!eq || !equality) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		return r.with_error(code::invalid_argument);
	}
	// Exclude non-Boolean operations from equation. The two sides live under
	// the bf_eq, not under `eq`: `eq` is the wff wrapping it and has a
	// single child, so indexing it with [1] tripped the `c != nullptr`
	// assert in tree<node>::child_tree (Debug) and read a null child
	// (Release).
	if (tau::get(equality)[0].find_top(is_non_boolean_term<node>) ||
		tau::get(equality)[1].find_top(is_non_boolean_term<node>)) {
		TAU_LOG_ERROR << "Found non-Boolean operation in equation";
		return r.with_error(code::unsupported_operation,
			"Found non-Boolean operation in equation");
	}

	DBG(TAU_LOG_TRACE << "lgrs/applied: " << LOG_FM(eq);)
	DBG(TAU_LOG_TRACE << "lgrs/equality: " << LOG_FM(equality);)

	// a result<> on both sides: move-assign, not with_value
	r = tau_lang::lgrs<node>(eq);
	DBG(assert(r.is_well_formed());)
	return r;
}

// Execution
// ------------------------------------------------------------

template <NodeType node>
result<interpreter<node>> api<node>::get_interpreter(tref spec) {
	interpreter_options options;
	return get_interpreter(spec, options);
}

// Shared tail of the two get_interpreter overloads: apply, normalize,
// reject free vars, then install the remaps and build the interpreter.
template <NodeType node>
result<interpreter<node>> build_interpreter(rr<node>&& nso_rr,
	interpreter_options& options)
{
	result<interpreter<node>> r;
	auto& ctx = *definitions<node>::instance().get_io_context();
	tref applied = nso_rr_apply<node>(nso_rr);
	if (!applied) {
		return r.with_error(code::internal_error, "Failed to apply rewriting rules");
	}
	TAU_TRY(tref normalized, normalizer<node>(applied));
	if (has_free_vars<node>(normalized)) {
		return r.with_error(code::invalid_argument,
			"Specification contains free variables");
	}
	ctx.input_remaps = options.input_remaps;
	ctx.output_remaps = options.output_remaps;
	auto i = interpreter<node>::make_interpreter(normalized, ctx);
	if (!i) {
		return r.with_error(code::internal_error, "Failed to build interpreter");
	}
	return r.with_value(std::move(i.value()));
}

template <NodeType node>
result<interpreter<node>> api<node>::get_interpreter(tref spec,
	interpreter_options& options)
{
	// Assign the remaps into the global io_context only after every
	// validation step succeeds: assigning them up front left them in
	// place -- corrupting later, unrelated calls -- on every one of the
	// early-return failure paths below.
	result<interpreter<node>> r;
	auto maybe_nso_rr = get_nso_rr(spec);
	if (!maybe_nso_rr) {
		return r.with_error(code::invalid_argument,
			messages::failed_to_build_rewriting_rules);
	}
	r = build_interpreter<node>(std::move(maybe_nso_rr.value()), options);
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<interpreter<node>> api<node>::get_interpreter(
	tau_spec<node>& spec)
{
	interpreter_options options;
	return get_interpreter(spec, options);
}

template <NodeType node>
result<interpreter<node>> api<node>::get_interpreter(
	tau_spec<node>& spec,
	interpreter_options& options)
{
	// See the tref overload above: remaps are assigned into the global
	// io_context only once every validation step has succeeded.
	result<interpreter<node>> r;
	auto maybe_nso_rr = spec.get_nso_rr();
	if (!maybe_nso_rr) {
		for (const auto& error : spec.errors()) {
			TAU_LOG_ERROR << error;
			r.error(code::parse_error, error);
		}
		if (r.has_error()) {
			DBG(assert(r.is_well_formed());)
			return r;
		}
		return r.with_error(code::invalid_argument,
			messages::failed_to_build_rewriting_rules);
	}
	r = build_interpreter<node>(std::move(maybe_nso_rr.value()), options);
	DBG(assert(r.is_well_formed());)
	return r;
}

// private helper methods
// ------------------------------------------------------------

template <NodeType node>
std::optional<rr<node>> api<node>::get_nso_rr(tref expr) {
	rr<node> nso_rr;
	auto ctx = *definitions<node>::instance().get_io_context();
	// A spec root is always unwrapped to its main formula and definitions,
	// whether or not it contains a ref: get_spec_or_term yields a spec for
	// any formula, and a spec node handed whole to the normalizer as its
	// main formula is negated as if it were a wff by the syntactic
	// simplifier (a Debug abort in build_wff_neg, a malformed tree in
	// Release).
	if (tau::get(expr).is(tau::spec)) {
		if (auto mayb_nso_rr = tau_lang::get_nso_rr<node>(ctx, expr);
			mayb_nso_rr) nso_rr = mayb_nso_rr.value();
		else return {};
	} else {
		nso_rr.main = tau::geth(resolve_io_vars<node>(ctx, expr));
		if (!nso_rr.main) return {};
	}
	return nso_rr;
}

template <NodeType node>
tref api<node>::infer(tref expr, bool use_defaults) {
	if (!expr) return nullptr;

	auto& defs = definitions<node>::instance();
	auto result = infer_ba_types<node>(expr,
		defs.get_global_scope(),
		defs.get_definition_heads(),
		{ .use_defaults = use_defaults });
	tref inferred = canonize_quantifier_ids<node>(result.first);
	// If type inference failed
	if (!inferred) {
		DBG(LOG_TRACE << "inferred is nullptr";)
		return nullptr;
	}
	defs.get_io_context()->update_types(result.second);
	defs.set_global_scope(result.second);

	//Check for semantic errors in expression
	if (has_semantic_error<node>(inferred)) {
		DBG(LOG_TRACE << "transformed has semantic error";)
		return nullptr;
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
	return inferred;
}

template <NodeType node>
tref api<node>::simplify(tref expr, bool use_defaults) {
	if (!expr) return nullptr;
	expr = infer(expr, use_defaults);
	if (!expr) return nullptr;
	expr = canonize_quantifier_ids<node>(tau::reget(expr));
	DBG(TAU_LOG_TRACE << "simplified: " << LOG_FM_DUMP(expr);)
	return expr;
}

} // namespace idni::tau_lang
