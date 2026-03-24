// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "api.h"

#include "sbf_parser.generated.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "api"

namespace idni::tau_lang {

// Helper functions
// ------------------------------------------------------------

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
tref api<node>::get_term(const std::string& input, bool simplified) {
	return tau::get(input, get_options<node>(tau::bf, simplified));
}

template <NodeType node>
tref api<node>::get_formula(const std::string& input, bool simplified) {
	return tau::get(input, get_options<node>(tau::wff, simplified));
}

template <NodeType node>
tref api<node>::get_function_def(const std::string& function_def, bool simplified) {
	tref def = get_definition(function_def, simplified);
	if (!def) return nullptr;
	auto nt = tau::get(def)[1].get_type();
	if (nt == tau::bf || nt == tau::ref) return def; // TODO ref can be wff
	return nullptr;
}

template <NodeType node>
tref api<node>::get_predicate_def(const std::string& predicate_def, bool simplified) {
	tref def = get_definition(predicate_def, simplified);
	if (!def) return nullptr;
	auto nt = tau::get(def)[1].get_type();
	// TODO we could pre resolve all refs to wff
	if (nt == tau::wff || nt == tau::ref) return def;
	return nullptr;
}

template <NodeType node>
tref api<node>::get_stream_def(const std::string& stream_def) {
	tref def = tau::get(stream_def, get_options<node>(tau::stream_def, true));
	return tau::trim(def);
}

template <NodeType node>
tref api<node>::get_spec(const std::string& src) {
	tau_spec<node> spec;
	if (!spec.parse(src)) return nullptr;
	if (tref s = spec.get(); s) return s;
	return nullptr;
}

template <NodeType node>
size_t api<node>::add_definition(tref head, tref body) {
	DBG(TAU_LOG_TRACE << "add_definition/head: " << LOG_FM_DUMP(head);)
	DBG(TAU_LOG_TRACE << "add_definition/body: " << LOG_FM_DUMP(body);)
	head = infer(head);
	body = infer(body);
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
	return definitions<node>::instance().add(tau::geth(head), tau::geth(body));
}

template <NodeType node>
tref api<node>::get_definition(const std::string& definition, bool simplified) {
	DBG(TAU_LOG_TRACE << "get_definition/definition: " << definition;)
	tref def = tau::get(definition, get_options<node>(tau::rec_relation, simplified));
	DBG(TAU_LOG_TRACE << "get_definition/def: " << LOG_FM_DUMP(def);)
	add_definition(tau::get(def).first(), tau::get(def).second());
	return def;
}

template <NodeType node>
tref api<node>::get_spec_or_term(const std::string& expression, bool simplified) {
	tref       expr = get_spec(expression); // try multiline first (includes a formula too)
	if (!expr) expr = get_term(expression, simplified); // if it fails, try just a term
	return expr;
}

template <NodeType node>
tref api<node>::get_formula_or_term(const std::string& expr, bool simplified) {
	tref e = tau::get(expr, get_options<node>(tau::fm_or_term, simplified));
	if (e) return tau::trim(e);
	return nullptr;
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
	return apply_defs(std::set<tref>{ def }, expr);
}

template <NodeType node>
tref api<node>::apply_defs(std::set<tref> defs, tref expr) {
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
	return apply_rr_to_formula<node>(nso_rr);
}

template <NodeType node>
tref api<node>::apply_all_defs(tref expr) {
	return apply_defs(std::set<tref>{}, expr);
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
	for (auto [that, with] : that_with)
		expr = substitute(expr, that, with);
	return expr;
}

// Normal forms
// ------------------------------------------------------------

template <NodeType node>
tref api<node>::boole_normal_form(tref expr) {
	expr = infer(expr);
	if (!expr) return nullptr;
	if (tref a = apply_all_defs(expr); a)
		return tau_lang::boole_normal_form<node>(a);
	return nullptr;
}

template <NodeType node>
tref api<node>::dnf(tref expr) {
	expr = infer(expr);
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
	expr = infer(expr);
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
	expr = infer(expr);
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
tref api<node>::simplify(tref expr) {
	if (!expr) return nullptr;
	expr = canonize_quantifier_ids<node>(tau::reget(expr));
	DBG(TAU_LOG_TRACE << "simplified: " << LOG_FM_DUMP(expr);)
	return expr;
}

template <NodeType node>
tref api<node>::syntactic_term_simplification(tref term) {
	term = infer(term);
	if (!term) return nullptr;
	return syntactic_path_simplification<node>::on(term);
}

template <NodeType node>
tref api<node>::syntactic_formula_simplification(tref fm) {
	fm = infer(fm);
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

	fm = infer(fm);
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
	term = infer(term);
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
	fm = infer(fm);
	if (!fm) return nullptr;
	return tau_lang::anti_prenex<node>(fm);
}

template <NodeType node>
tref api<node>::eliminate_quantifiers(tref fm) {
	if (!fm) return nullptr;
	fm = infer(fm);
	if (!fm) return nullptr;
	if (tref a = apply_all_defs(fm); a)
		return resolve_quantifiers<node>(tau_lang::anti_prenex<node>(a));
	return nullptr;
}

template <NodeType node>
bool api<node>::realizable(tref fm) {
	fm = infer(fm);
	return fm && is_formula(fm)
		&& is_tau_formula_sat<node>(normalize_formula(fm), 0, true);
}

template <NodeType node>
bool api<node>::unrealizable(tref fm) {
	return !realizable(fm);
}

template <NodeType node>
bool api<node>::sat(tref fm) {
	fm = infer(fm);
	return fm && has_no_boolean_combs_of_models<node>(fm) && realizable(fm);
}

template <NodeType node>
bool api<node>::unsat(tref fm) {
	return !sat(fm);
}

template <NodeType node>
bool api<node>::valid(tref fm) {
	fm = infer(fm);
	return fm && has_no_boolean_combs_of_models<node>(fm) && valid_spec(fm);
}

template <NodeType node>
bool api<node>::valid_spec(tref fm) {
	fm = infer(fm);
	return fm && is_tau_impl<node>(tau::_T(), normalize_formula(fm));
}


// Solving
// ------------------------------------------------------------

template <NodeType node>
std::optional<subtree_map<node, tref>> api<node>::solve(
	tref fm, solver_mode mode)
{
	fm = infer(fm);
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
	DBG(TAU_LOG_TRACE << "solve: " << LOG_FM(fm);)
	// setting solver options
	solver_options options = {
		.splitter_one = node::ba::
			splitter_one(tau_type<node>()),
		.mode = mode
	};
	bool solve_error = false;
	auto solution = tau_lang::solve<node>(fm, options, solve_error);
	if (solve_error) {
		TAU_LOG_ERROR << "Internal error in solver";
		return {};
	}
	return solution;
}

template <NodeType node>
std::optional<subtree_map<node, tref>> api<node>::lgrs(tref equation) {
	using tt = tau::traverser;
	equation = infer(equation);
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
	// Exclude non-Boolean operations from equation
	if (tau::get(eq)[0].find_top(is_non_boolean_term<node>) ||
		tau::get(eq)[1].find_top(is_non_boolean_term<node>)) {
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
	auto& ctx = *definitions<node>::instance().get_io_context();
	ctx.input_remaps = options.input_remaps;
	ctx.output_remaps = options.output_remaps;
	auto maybe_nso_rr = tau_lang::get_nso_rr<node>(spec);
	if (!maybe_nso_rr) return {};
	tref applied = apply_rr_to_formula<node>(maybe_nso_rr.value());
	if (!applied) return {};
	tref normalized = normalizer<node>(applied);
	if (!normalized) return {};
	if (has_free_vars<node>(normalized)) return {};
	return interpreter<node>::make_interpreter(normalized, ctx);
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
	auto& ctx = *definitions<node>::instance().get_io_context();
	ctx.input_remaps = options.input_remaps;
	ctx.output_remaps = options.output_remaps;
	auto maybe_nso_rr = spec.get_nso_rr();
	if (!maybe_nso_rr) {
		for (const auto& error : spec.errors()) TAU_LOG_ERROR << error;
		return {};
	}
	tref applied = apply_rr_to_formula<node>(maybe_nso_rr.value());
	if (!applied) return {};
	tref normalized = normalizer<node>(applied);
	if (!normalized) return {};
	if (has_free_vars<node>(normalized)) return {};
	return interpreter<node>::make_interpreter(normalized, ctx);
}

// private helper methods
// ------------------------------------------------------------

template <NodeType node>
std::optional<rr<node>> api<node>::get_nso_rr(tref expr) {
	rr<node> nso_rr;
	auto ctx = *definitions<node>::instance().get_io_context();
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

template <NodeType node>
tref api<node>::infer(tref expr, bool use_defaults) {
	if (!expr) return nullptr;

	expr = simplify(expr);

	auto& defs = definitions<node>::instance();
	auto result = infer_ba_types<node>(expr,
		defs.get_global_scope(),
		defs.get_definition_heads(),
		{ .use_defaults = use_defaults });
	tref inferred = result.first;
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

} // namespace idni::tau_lang
