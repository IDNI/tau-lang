// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "api.h"

#include "sbf_parser.generated.h"
#include "tau_tree_builders.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "api"

namespace idni::tau_lang {

// Helper functions
// ------------------------------------------------------------

/// Extract the update specification from the interpreter's output assignment.
/// Looks for a stream variable named "u" at time_point-1 with tau type.
/// If the output for that variable is non-zero, unpacks and returns it
/// as a tref that can be fed back into interpreter::update().
/// @return The unpacked update formula, or nullptr if no update is present.
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
void api<node>::set_block_max_rounds(size_t n) {
	block_max_rounds = n ? n : std::numeric_limits<size_t>::max();
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
void api<node>::set_max_consistency_subsets(size_t n) {
	max_consistency_subsets = n;
}

template <NodeType node>
void api<node>::set_cache_bound(size_t n) {
	cache_bound = n;
}

template <NodeType node>
void api<node>::set_pwr_semantic_fallback(bool on) {
	pwr_semantic_fallback = on;
}

template <NodeType node>
void api<node>::set_indenting(bool indenting) {
	pretty_printer_indenting = indenting;
}

template <NodeType node>
void api<node>::set_highlighting(bool highlighting) {
	pretty_printer_highlighting = highlighting;
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
tref api<node>::get_term(const std::string& input, bool simplified) {
	return tau::get(input, get_options<node>(tau::bf, simplified));
}

template <NodeType node>
tref api<node>::get_formula(const std::string& input, bool simplified) {
	tref fm = tau::get(input, get_options<node>(tau::wff, simplified));
	if (!fm) return fm;
	// An io_var's input/output bit is set while parsing a *spec*, so parsing
	// a bare wff leaves every io_var classified as neither -- and consumers
	// that need the distinction (transform_io_var,
	// existentially_quantify_output_streams, the LTL/PWR pipelines) then
	// reject it or, worse, silently treat inputs as outputs. Resolve here so
	// the same text yields the same classification whether it arrives as a
	// formula or inside a spec; this mirrors what get_nso_rr already does for
	// a bare wff/bf.
	return resolve_io_vars<node>(
		*definitions<node>::instance().get_io_context(), fm);
}

template <NodeType node>
tref api<node>::get_function_def(const std::string& function_def, [[maybe_unused]] bool simplified) {
	// AP1-5: parse and validate BEFORE registering -- routing through
	// get_definition registered unconditionally, so a rejected
	// definition stayed in the global store and leaked into later
	// apply_defs_to_spec calls.
	tref def = tau::get(function_def,
		get_options<node>(tau::rec_relation, true));
	if (!def) return nullptr;
	// The second child of a rec_relation is the body;
	// accept bf or ref (a ref body may resolve to a bf later --
	// AP1-26: the code rejected refs while doc and the predicate
	// sibling accepted them)
	auto nt = tau::get(def)[1].get_type();
	if (nt != tau::bf && nt != tau::ref) return nullptr;
	add_definition(tau::get(def).first(), tau::get(def).second());
	return def;
}

template <NodeType node>
tref api<node>::get_predicate_def(const std::string& predicate_def, [[maybe_unused]] bool simplified) {
	// AP1-5: parse and validate BEFORE registering (see get_function_def).
	tref def = tau::get(predicate_def,
		get_options<node>(tau::rec_relation, true));
	if (!def) return nullptr;
	// TODO we could pre resolve all refs to wff
	auto nt = tau::get(def)[1].get_type();
	if (nt != tau::wff && nt != tau::ref) return nullptr;
	add_definition(tau::get(def).first(), tau::get(def).second());
	return def;
}

template <NodeType node>
tref api<node>::get_stream_def(const std::string& stream_def) {
	tref def = tau::get(stream_def, get_options<node>(tau::stream_def, true));
	if (!def) return nullptr;
	return tau::trim(def);
}

template <NodeType node>
tref api<node>::get_spec(const std::string& src) {
	tau_spec<node> spec;
	if (!spec.parse(src)) {
		for (const auto& error : spec.errors()) TAU_LOG_ERROR << error;
		return nullptr;
	}
	if (tref s = spec.get(); s) return s;
	return nullptr;
}

template <NodeType node>
size_t api<node>::add_definition(tref head, tref body) {
	DBG(TAU_LOG_TRACE << "add_definition/head: " << LOG_FM_DUMP(head);)
	DBG(TAU_LOG_TRACE << "add_definition/body: " << LOG_FM_DUMP(body);)
	if (!head || !body) {
		if (!head) {
			DBG(TAU_LOG_TRACE << "add_definition/head is nullptr";)
		}
		if (!body) {
			DBG(TAU_LOG_TRACE << "add_definition/body is nullptr";)
		}
		return 0;
	}
	DBG(TAU_LOG_TRACE << "add_definition/adding head: " << LOG_FM_DUMP(head);)
	DBG(TAU_LOG_TRACE << "add_definition/adding body: " << LOG_FM_DUMP(body);)
	// AP1-6: 1-based -- the store's 0-based index made the very first
	// definition return 0, the documented failure value.
	return definitions<node>::instance().add(
		tau::geth(head), tau::geth(body)) + 1;
}

template <NodeType node>
tref api<node>::get_definition(const std::string& definition, bool simplified) {
	DBG(TAU_LOG_TRACE << "get_definition/definition: " << definition;)
	tref def = tau::get(definition, get_options<node>(tau::rec_relation, simplified));
	DBG(TAU_LOG_TRACE << "get_definition/def: " << LOG_FM_DUMP(def);)
	if (!def) return nullptr;
	add_definition(tau::get(def).first(), tau::get(def).second());
	return def;
}

template <NodeType node>
tref api<node>::get_spec_or_term(const std::string& expression, bool simplified) {
	// Try parsing as a full spec first (which handles multiline and
	// formula inputs); fall back to a bare bf term if that fails.
	// AP1-18: the spec attempt is quiet -- its parse errors are logged
	// only when the term fallback ALSO fails, so a legitimate bare term
	// no longer emits spurious ERROR lines on the successful path.
	tau_spec<node> spec;
	tref expr = spec.parse(expression) ? spec.get() : nullptr;
	if (!expr) expr = get_term(expression, simplified);
	if (!expr)
		for (const auto& error : spec.errors())
			TAU_LOG_ERROR << error;
	return expr;
}

template <NodeType node>
tref api<node>::get_formula_or_term(const std::string& expr, bool simplified) {
	tref e = tau::get(expr, get_options<node>(tau::fm_or_term, simplified));
	if (!e) return nullptr;
	e = tau::trim(e);
	// AP1-32: classify io_vars like get_formula does, so the same text
	// yields the same tree through either entry (downstream re-resolution
	// hid the difference from sat machinery, but the trees differed).
	if (tau::get(e).is(tau::wff))
		e = resolve_io_vars<node>(
			*definitions<node>::instance().get_io_context(), e);
	return e;
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
	pre_order<node>(expression).search(searcher);
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
	// AP1-4: this must apply the globally registered definitions --
	// routing through apply_defs({}) applied nothing, so after
	// get_definition() the dnf/cnf/nnf/solve/lgrs pipelines received
	// refs unexpanded, contradicting the documented contract. Mirrors
	// the normalizer's apply_defs_to_spec.
	if (!expr) return nullptr;
	auto maybe_nso_rr = get_nso_rr(expr);
	if (!maybe_nso_rr) return nullptr;
	auto& nso_rr = maybe_nso_rr.value();
	const auto& defs = definitions<node>::instance().get_sym_defs();
	nso_rr.rec_relations.insert(nso_rr.rec_relations.end(),
		defs.begin(), defs.end());
	return nso_rr_apply<node>(nso_rr);
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
	// Enforce that all three are consistently terms or formulas.
	// Mismatches would produce an ill-typed tree.
	bool e = is_term(expr), t = is_term(that), w = is_term(with);
	if ((e && e != t) || (e && e != w) || (!e && t != w)) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		return nullptr;
	}
	return tau::get(expr).substitute(that, with);
}

template <NodeType node>
tref api<node>::substitute(tref expr, std::map<tref, tref> that_with) {
	for (auto [that, with] : that_with)
		expr = substitute(expr, that, with);
	return expr;
}

// Normal forms
// ------------------------------------------------------------

template <NodeType node>
tref api<node>::boole_normal_form(tref expr) {
	// Simplify, apply all registered definitions, then compute BNF
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
		// Dispatch to bf-level or wff-level DNF depending on root type
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
		// Dispatch to wff-level or bf-level CNF depending on root type
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
		// wff: full NNF via De Morgan; bf: push negation into sub-terms
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
tref api<node>::normalize_formula(tref fm) {
	if (!fm) return nullptr;
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
	if (!fm) return nullptr;
	auto maybe_nso_rr = get_nso_rr(fm);
	if (!maybe_nso_rr || !maybe_nso_rr.value().main
		|| tau::get(maybe_nso_rr.value().main).is(tau::bf))
			return nullptr;
	return normalizer<node>(maybe_nso_rr.value());
}

template <NodeType node>
tref api<node>::normalize_term(tref term) {
	if (!term) return nullptr;
	DBG(TAU_LOG_TRACE << "normalize_term(): " << LOG_FM_DUMP(term);)
	term = simplify(term);
	if (!term) return nullptr;
	DBG(TAU_LOG_TRACE << "inferred term: " << LOG_FM_DUMP(term);)
	auto maybe_nso_rr = get_nso_rr(term);
	if (!maybe_nso_rr) return nullptr;
	auto& nso_rr = maybe_nso_rr.value();
	tref main = nso_rr.main->get();
	if (!main || !tau::get(main).is(tau::bf)) return nullptr;
	if (contains(main, tau::ref))
		return bf_normalizer_with_rec_relation<node>(nso_rr);
	return bf_normalizer_without_rec_relation<node>(main);
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
bool api<node>::realizable(tref fm) {
	fm = simplify(fm);
	// G(A) ∧ G(B) ≡ G(A ∧ B): merge top-level G-conjuncts before
	// normalization so the downstream pipeline sees a single wff_always.
	if (fm) fm = flatten_always_conjuncts<node>(fm);
	if (!fm || !is_formula(fm)) return false;
	// normalize_formula() returns nullptr on failures that are reachable
	// from user input: a non-well-founded recurrence, a definition set
	// whose expansion never settles, a fallback type mismatch, or a
	// get_nso_rr failure. is_tau_formula_sat() dereferences its argument
	// immediately, so the null has to be caught here.
	tref nfm = normalize_formula(fm);
	if (!nfm) {
		TAU_LOG_ERROR << "Could not normalize the formula; "
			"its satisfiability cannot be decided";
		return false;
	}
	// LT-7: the synthesis backend reports "no verdict" by throwing
	// ltl_synthesis_error -- a timed-out, killed or misused ltlsynt is NOT an
	// UNREALIZABLE answer.  Nothing below this layer catches it, so without a
	// handler here a slow specification would terminate the process instead of
	// answering.  Convert it into a logged error verdict, the same shape the
	// normalize_formula null-gate above uses.  The verdict is `false`, but the
	// log says UNKNOWN so it is not mistaken for a decided UNREALIZABLE.
	try {
		return is_tau_formula_sat<node>(nfm, 0, true);
	} catch (const ltl_synthesis_error& e) {
		TAU_LOG_ERROR << "UNKNOWN: the synthesis backend failed or timed out ("
			<< e.what() << "); realizability could not be decided";
		return false;
	}
}

template <NodeType node>
bool api<node>::unrealizable(tref fm) {
	// AP1-11: null input is invalid, not "unrealizable" -- match the
	// htref overload's false (a bool API cannot express errors, and
	// claiming a verdict for unparseable input is the worse lie).
	if (!fm) return false;
	return !realizable(fm);
}

template <NodeType node>
bool api<node>::sat(tref fm) {
	fm = simplify(fm);
	// G(A) ∧ G(B) ≡ G(A ∧ B); merge top-level conjunctions of G so the
	// downstream safety pipeline sees one wff_always.  Non-mergeable
	// Boolean combinations (disjunction, negation, F-on-non-singletons,
	// etc.) survive flatten unchanged and are routed to the full-LTL
	// pipeline by is_tau_formula_sat itself — there's no longer a
	// pre-check that rejects them at this layer.
	if (fm) fm = flatten_always_conjuncts<node>(fm);
	return fm && realizable(fm);
}

template <NodeType node>
bool api<node>::unsat(tref fm) {
	// AP1-11: null input is invalid, not "unsatisfiable" (see
	// unrealizable above).
	if (!fm) return false;
	return !sat(fm);
}

template <NodeType node>
bool api<node>::valid(tref fm) {
	fm = simplify(fm);
	if (fm) fm = flatten_always_conjuncts<node>(fm);
	return fm && valid_spec(fm);
}

template <NodeType node>
bool api<node>::valid_spec(tref fm) {
	fm = simplify(fm);
	if (!fm) return false;
	// Same null contract as realizable(): is_tau_impl() normalizes both
	// arguments straight away and cannot be handed a null formula.
	tref nfm = normalize_formula(fm);
	if (!nfm) {
		TAU_LOG_ERROR << "Could not normalize the formula; "
			"its validity cannot be decided";
		return false;
	}
	// Valid iff T (tautology) implies the normalized formula.
	// Same synthesis-failure gate as realizable() -- see the note there.
	try {
		return is_tau_impl<node>(tau::_T(), nfm);
	} catch (const ltl_synthesis_error& e) {
		TAU_LOG_ERROR << "UNKNOWN: the synthesis backend failed or timed out ("
			<< e.what() << "); validity could not be decided";
		return false;
	}
}


// Solving
// ------------------------------------------------------------

// Solve: apply defs, reject temporal quantifiers, then run the solver.
template <NodeType node>
std::optional<subtree_map<node, tref>> api<node>::solve(
	tref fm, solver_mode mode)
{
	fm = simplify(fm);
	if (!fm) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		return {};
	}
	tref a = apply_all_defs(fm);
	if (!a) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		return {};
	}
	// Reject formula involving temporal quantification
	if (tau::get(a).find_top(is_temporal_quantifier<node>)) {
		TAU_LOG_ERROR << "Found temporal quantifier in formula: "
			<< TAU_TO_STR(fm);
		return {};
	}
	DBG(TAU_LOG_TRACE << "solve: " << LOG_FM(a);)
	// setting solver options
	solver_options options = {
		.splitter_one = node::ba::
			splitter_one(tau_type<node>()),
		.mode = mode
	};
	bool solve_error = false;
	// Use fully-expanded formula (a) so function/predicate refs are resolved
	// before reaching type-specific solvers (fixes bug with typed functions).
	auto solution = tau_lang::solve<node>(a, options, solve_error);
	if (solve_error) {
		TAU_LOG_ERROR << "Internal error in solver";
		return {};
	}
	return solution;
}

// LGRS: normalize the equation, extract the bf_eq equality, then solve.
template <NodeType node>
std::optional<subtree_map<node, tref>> api<node>::lgrs(tref equation) {
	using tt = tau::traverser;
	equation = simplify(equation);
	tref a = apply_all_defs(equation);
	if (!a) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		return {};
	}
	tref eq = apply_all_xor_def<node>(norm_all_equations<node>(a));
	tref equality = tt(eq) | tau::bf_eq | tt::ref;
	if (!eq || !equality) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		return {};
	}
	// Exclude non-Boolean operations from equation. The two sides live under
	// the bf_eq, not under `eq`: `eq` is the wff wrapping it and has a
	// single child, so indexing it with [1] tripped the `c != nullptr`
	// assert in tree<node>::child_tree (Debug) and read a null child
	// (Release).
	if (tau::get(equality)[0].find_top(is_non_boolean_term<node>) ||
		tau::get(equality)[1].find_top(is_non_boolean_term<node>)) {
		TAU_LOG_ERROR << "Found non-Boolean operation in equation";
		return {};
	}

	DBG(TAU_LOG_TRACE << "lgrs/applied: " << LOG_FM(eq);)
	DBG(TAU_LOG_TRACE << "lgrs/equality: " << LOG_FM(equality);)

	return tau_lang::lgrs<node>(eq);
}

// Execution
// ------------------------------------------------------------

template <NodeType node>
std::optional<interpreter<node>> api<node>::get_interpreter(tref spec) {
	interpreter_options options;
	return get_interpreter(spec, options);
}

template <NodeType node>
std::optional<interpreter<node>> api<node>::get_interpreter(tref spec,
	interpreter_options& options)
{
	// Assign the remaps into the global io_context only after every
	// validation step succeeds: assigning them up front left them in
	// place -- corrupting later, unrelated calls -- on every one of the
	// early-return failure paths below.
	auto& ctx = *definitions<node>::instance().get_io_context();
	auto maybe_nso_rr = get_nso_rr(spec);
	if (!maybe_nso_rr) return {};
	tref applied = nso_rr_apply<node>(maybe_nso_rr.value());
	if (!applied) return {};
	tref normalized = normalizer<node>(applied);
	if (!normalized) return {};
	if (has_free_vars<node>(normalized)) return {};
	ctx.input_remaps = options.input_remaps;
	ctx.output_remaps = options.output_remaps;
	// LT-7: make_interpreter reaches ltlsynt through
	// ltl_to_safety_formula_full; a backend failure must not terminate the
	// caller.  No interpreter is the honest answer here.
	try {
		return interpreter<node>::make_interpreter(normalized, ctx);
	} catch (const ltl_synthesis_error& e) {
		TAU_LOG_ERROR << "UNKNOWN: the synthesis backend failed or timed out ("
			<< e.what() << "); the specification could not be compiled";
		return {};
	}
}

template <NodeType node>
std::optional<interpreter<node>> api<node>::get_interpreter(
	tau_spec<node>& spec)
{
	interpreter_options options;
	return get_interpreter(spec, options);
}

template <NodeType node>
std::optional<interpreter<node>> api<node>::get_interpreter(
	tau_spec<node>& spec,
	interpreter_options& options)
{
	// See the tref overload above: remaps are assigned into the global
	// io_context only once every validation step has succeeded.
	auto& ctx = *definitions<node>::instance().get_io_context();
	auto maybe_nso_rr = spec.get_nso_rr();
	if (!maybe_nso_rr) {
		for (const auto& error : spec.errors()) TAU_LOG_ERROR << error;
		return {};
	}
	tref applied = nso_rr_apply<node>(maybe_nso_rr.value());
	if (!applied) return {};
	tref normalized = normalizer<node>(applied);
	if (!normalized) return {};
	if (has_free_vars<node>(normalized)) return {};
	ctx.input_remaps = options.input_remaps;
	ctx.output_remaps = options.output_remaps;
	// See the tref overload: synthesis-backend failures are answered, not
	// propagated.
	try {
		return interpreter<node>::make_interpreter(normalized, ctx);
	} catch (const ltl_synthesis_error& e) {
		TAU_LOG_ERROR << "UNKNOWN: the synthesis backend failed or timed out ("
			<< e.what() << "); the specification could not be compiled";
		return {};
	}
}

// private helper methods
// ------------------------------------------------------------

// Private: extract rr<node> from expression tree.
// For spec nodes, delegates to tau_lang::get_nso_rr.
// For bare wff/bf nodes, wraps via resolve_io_vars.
template <NodeType node>
std::optional<rr<node>> api<node>::get_nso_rr(tref expr) {
	rr<node> nso_rr;
	// AP1-16: by reference -- copying the io_context (three subtree maps
	// + remaps + console factory) per call was pure waste; all uses read.
	auto& ctx = *definitions<node>::instance().get_io_context();
	if (contains(expr, tau::ref)) {
		typename node::type type = tau::get(expr).get_type();
		if (type == tau::spec) {
			if (auto mayb_nso_rr = tau_lang::get_nso_rr<node>(
				ctx, expr); mayb_nso_rr)
					nso_rr = mayb_nso_rr.value();
			else return {};
		} else {
			nso_rr.main = tau::geth(resolve_io_vars<node>(ctx, expr));
			if (!nso_rr.main) return {};
		}
	} else nso_rr.main = tau::geth(resolve_io_vars<node>(ctx, expr));
	return nso_rr;
}

// Type inference pipeline: infer BA types → canonize quantifier IDs
// → unnest G-in-G ambiguity → check semantics → update global scope.
template <NodeType node>
tref api<node>::infer(tref expr, bool use_defaults) {
	if (!expr) return nullptr;

	auto& defs = definitions<node>::instance();
	auto result = infer_ba_types<node>(expr,
		defs.get_global_scope(),
		defs.get_definition_heads(),
		{ .use_defaults = use_defaults });
	tref inferred = canonize_quantifier_ids<node>(result.first);
	if (!inferred) {
		DBG(LOG_TRACE << "inferred is nullptr";)
		return nullptr;
	}
	defs.get_io_context()->update_types(result.second);
	defs.set_global_scope(std::move(result.second));

	// Rewrite G(A && G(B)) → G(A) && G(B) before the semantic error check.
	// This arises because the CFG parser is ambiguous: G(X) && G(Y) can
	// be parsed as either G(X) && G(Y) (correct) or G(X && G(Y)) (nested).
	// G(A && G(B)) = G(A) && G(B) semantically, so this rewrite is safe.
	// Only applies when full-LTL operators (F/U/R/W) are absent; with LTL
	// operators the nesting check is already bypassed.
	inferred = unnest_nested_always<node>(inferred);

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
