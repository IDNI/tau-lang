// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "api.h"

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
	tau_parser::instance().get_grammar().add_dynamic("type_name", names);
}

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
void api<node>::set_max_cover_products(size_t n) {
	max_cover_products = n;
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
void api<node>::set_ba_component_factoring(bool state) {
	pack_set_ba_component_factoring<node>(state);
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
result<tref> api<node>::get_term(const std::string& input, bool simplified) {
	result<tref> r;
	tref e = tau::get(input, get_options<node>(tau::bf, simplified));
	if (!e) r.error(code::parse_error, "Failed to parse term");
	else    r = e;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::get_formula(const std::string& input, bool simplified) {
	result<tref> r;
	tref fm = tau::get(input, get_options<node>(tau::wff, simplified));
	if (!fm) {
		r.error(code::parse_error, "Failed to parse formula");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// An io_var's input/output bit is set while parsing a *spec*, so parsing
	// a bare wff leaves every io_var classified as neither -- and consumers
	// that need the distinction (transform_io_var,
	// existentially_quantify_output_streams, the LTL/PWR pipelines) then
	// reject it or, worse, silently treat inputs as outputs. Resolve here so
	// the same text yields the same classification whether it arrives as a
	// formula or inside a spec; this mirrors what get_nso_rr already does for
	// a bare wff/bf.
	r = resolve_io_vars<node>(
		*definitions<node>::instance().get_io_context(), fm);
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::get_function_def(const std::string& function_def, [[maybe_unused]] bool simplified) {
	result<tref> r;
	// AP1-5: parse and validate BEFORE registering -- routing through
	// get_definition registered unconditionally, so a rejected
	// definition stayed in the global store and leaked into later
	// apply_defs_to_spec calls.
	tref def = tau::get(function_def,
		get_options<node>(tau::rec_relation, true));
	if (!def) {
		r.error(code::parse_error, "Failed to parse function definition");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// The second child of a rec_relation is the body;
	// accept bf or ref (a ref body may resolve to a bf later --
	// AP1-26: the code rejected refs while doc and the predicate
	// sibling accepted them)
	auto nt = tau::get(def)[1].get_type();
	if (nt != tau::bf && nt != tau::ref) {
		r.error(code::invalid_argument, "Not a function definition");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto added = add_definition(tau::get(def).first(), tau::get(def).second());
	if (!added.has_value()) r.merge(std::move(added));
	else r = def;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::get_predicate_def(const std::string& predicate_def, [[maybe_unused]] bool simplified) {
	result<tref> r;
	// AP1-5: parse and validate BEFORE registering (see get_function_def).
	tref def = tau::get(predicate_def,
		get_options<node>(tau::rec_relation, true));
	if (!def) {
		r.error(code::parse_error, "Failed to parse predicate definition");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// TODO we could pre resolve all refs to wff
	auto nt = tau::get(def)[1].get_type();
	if (nt != tau::wff && nt != tau::ref) {
		r.error(code::invalid_argument, "Not a predicate definition");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto added = add_definition(tau::get(def).first(), tau::get(def).second());
	if (!added.has_value()) r.merge(std::move(added));
	else r = def;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::get_stream_def(const std::string& stream_def) {
	result<tref> r;
	tref def = tau::get(stream_def, get_options<node>(tau::stream_def, true));
	if (!def) r.error(code::parse_error, "Failed to parse stream definition");
	else      r = tau::trim(def);
	DBG(assert(r.is_well_formed());)
	return r;
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
		if (!r.has_error()) r.error(code::parse_error, "Failed to parse spec");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	if (tref s = spec.get(); s) r = s;
	else r.error(code::parse_error, "Failed to parse spec");
	DBG(assert(r.is_well_formed());)
	return r;
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
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	DBG(TAU_LOG_TRACE << "add_definition/adding head: " << LOG_FM_DUMP(head);)
	DBG(TAU_LOG_TRACE << "add_definition/adding body: " << LOG_FM_DUMP(body);)
	// AP1-6: 1-based -- the store's 0-based index made the very first
	// definition return 0, the documented failure value.
	r = definitions<node>::instance().add(
		tau::geth(head), tau::geth(body)) + 1;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::get_definition(const std::string& definition, bool simplified) {
	result<tref> r;
	DBG(TAU_LOG_TRACE << "get_definition/definition: " << definition;)
	tref def = tau::get(definition, get_options<node>(tau::rec_relation, simplified));
	DBG(TAU_LOG_TRACE << "get_definition/def: " << LOG_FM_DUMP(def);)
	if (!def) {
		r.error(code::parse_error, "Failed to parse definition");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto added = add_definition(tau::get(def).first(), tau::get(def).second());
	if (!added.has_value()) r.merge(std::move(added));
	else r = def;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::get_spec_or_term(const std::string& expression, bool simplified) {
	result<tref> r;
	// Try parsing as a full spec first (which handles multiline and
	// formula inputs); fall back to a bare bf term if that fails.
	// AP1-18: the spec attempt is quiet -- its parse errors are logged
	// only when the term fallback ALSO fails, so a legitimate bare term
	// no longer emits spurious ERROR lines on the successful path.
	tau_spec<node> spec;
	tref expr = spec.parse(expression) ? spec.get() : nullptr;
	if (expr) {
		r = expr;
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto term = get_term(expression, simplified);
	if (!term.has_value()) {
		for (const auto& error : spec.errors()) TAU_LOG_ERROR << error;
		r.merge(std::move(term));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	r = term.value();
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::get_formula_or_term(const std::string& expr, bool simplified) {
	result<tref> r;
	tref e = tau::get(expr, get_options<node>(tau::fm_or_term, simplified));
	if (!e) {
		r.error(code::parse_error, "Failed to parse formula or term");
		DBG(assert(r.is_well_formed());)
		return r;
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
	return apply_defs(subtree_set<node>{ def }, expr);
}

template <NodeType node>
result<tref> api<node>::apply_defs(subtree_set<node> defs, tref expr) {
	result<tref> r;
	if (!expr) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto maybe_nso_rr = get_nso_rr(expr);
	if (!maybe_nso_rr.has_value()) {
		r.merge(std::move(maybe_nso_rr));
		DBG(assert(r.is_well_formed());)
		return r;
	}
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
	auto applied = r.take_or_error(nso_rr_apply<node>(nso_rr),
		code::internal_error, "Failed to apply definitions");
	if (applied) r = *applied;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::apply_all_defs(tref expr) {
	// AP1-4: this must apply the globally registered definitions --
	// routing through apply_defs({}) applied nothing, so after
	// get_definition() the dnf/cnf/nnf/solve/lgrs pipelines received
	// refs unexpanded, contradicting the documented contract. Mirrors
	// the normalizer's apply_defs_to_spec.
	result<tref> r;
	if (!expr) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto maybe_nso_rr = get_nso_rr(expr);
	if (!maybe_nso_rr.has_value()) {
		r.merge(std::move(maybe_nso_rr));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto& nso_rr = maybe_nso_rr.value();
	const auto& defs = definitions<node>::instance().get_sym_defs();
	nso_rr.rec_relations.insert(nso_rr.rec_relations.end(),
		defs.begin(), defs.end());
	auto applied = r.take_or_error(nso_rr_apply<node>(nso_rr),
		code::internal_error, "Failed to apply definitions");
	if (applied) r = *applied;
	DBG(assert(r.is_well_formed());)
	return r;
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
	result<tref> r;
	if (!expr || !that || !with) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	DBG(TAU_LOG_TRACE << "substitute: \n" << LOG_FM_DUMP(expr) << "\n" << LOG_FM_DUMP(that) << "\n" << LOG_FM_DUMP(with);)
	// Enforce that all three are consistently terms or formulas.
	// Mismatches would produce an ill-typed tree.
	bool e = is_term(expr), t = is_term(that), w = is_term(with);
	if ((e && e != t) || (e && e != w) || (!e && t != w)) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	tref s = tau::get(expr).substitute(that, with);
	if (!s) r.error(code::internal_error, "Substitution failed");
	else    r = s;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::substitute(tref expr, std::map<tref, tref> that_with) {
	result<tref> r;
	for (auto [that, with] : that_with) {
		auto sub = substitute(expr, that, with);
		if (!sub.has_value()) {
			r.merge(std::move(sub));
			DBG(assert(r.is_well_formed());)
			return r;
		}
		expr = sub.value();
	}
	r = expr;
	DBG(assert(r.is_well_formed());)
	return r;
}

// Normal forms
// ------------------------------------------------------------

template <NodeType node>
result<tref> api<node>::boole_normal_form(tref expr) {
	// Simplify, apply all registered definitions, then compute BNF
	result<tref> r;
	auto simplified = simplify(expr);
	if (!simplified.has_value()) {
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto applied = apply_all_defs(simplified.value());
	if (!applied.has_value()) {
		r.merge(std::move(applied));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	tref b = tau_lang::boole_normal_form<node>(applied.value());
	if (!b) r.error(code::internal_error, "Boole normal form conversion failed");
	else    r = b;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::dnf(tref expr) {
	result<tref> r;
	auto simplified = simplify(expr);
	if (!simplified.has_value()) {
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto applied = apply_all_defs(simplified.value());
	if (!applied.has_value()) {
		r.merge(std::move(applied));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	tref a = applied.value();
	tref d = nullptr;
	// Dispatch to bf-level or wff-level DNF depending on root type
	switch (tau::get(a).get_type()) {
	case tau::bf:  d = reduce<node>(to_dnf<node, false>(a)); break;
	case tau::wff: d = reduce<node>(to_dnf<node>(a)); break;
	default: r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	if (!d) r.error(code::internal_error, "DNF conversion failed");
	else    r = d;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::cnf(tref expr) {
	result<tref> r;
	auto simplified = simplify(expr);
	if (!simplified.has_value()) {
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto applied = apply_all_defs(simplified.value());
	if (!applied.has_value()) {
		r.merge(std::move(applied));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	tref a = applied.value();
	tref c = nullptr;
	// Dispatch to wff-level or bf-level CNF depending on root type
	switch (tau::get(a).get_type()) {
	case tau::wff: c = reduce<node, true>(to_cnf<node>(a)); break;
	case tau::bf:  c = reduce<node, true>(to_cnf<node, false>(a)); break;
	default: r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	if (!c) r.error(code::internal_error, "CNF conversion failed");
	else    r = c;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::nnf(tref expr) {
	result<tref> r;
	auto simplified = simplify(expr);
	if (!simplified.has_value()) {
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto applied = apply_all_defs(simplified.value());
	if (!applied.has_value()) {
		r.merge(std::move(applied));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	tref a = applied.value();
	tref n = nullptr;
	// wff: full NNF via De Morgan; bf: push negation into sub-terms
	switch (tau::get(a).get_type()) {
	case tau::wff: n = to_nnf<node>(a); break;
	case tau::bf:  n = push_negation_in<node, false>(a); break;
	default: r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	if (!n) r.error(code::internal_error, "NNF conversion failed");
	else    r = n;
	DBG(assert(r.is_well_formed());)
	return r;
}

// Procedures
// ------------------------------------------------------------

template <NodeType node>
result<tref> api<node>::syntactic_term_simplification(tref term) {
	result<tref> r;
	auto simplified = simplify(term);
	if (!simplified.has_value()) {
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	tref s = syntactic_path_simplification<node>(simplified.value());
	if (!s) r.error(code::internal_error, "Syntactic term simplification failed");
	else    r = s;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::syntactic_formula_simplification(tref fm) {
	result<tref> r;
	auto simplified = simplify(fm);
	if (!simplified.has_value()) {
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	tref s = tau_lang::syntactic_formula_simplification<node>(simplified.value());
	if (!s) r.error(code::internal_error, "Syntactic formula simplification failed");
	else    r = s;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::normalize_formula(tref fm) {
	result<tref> r;
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
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

	auto simplified = simplify(fm);
	if (!simplified.has_value()) {
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	fm = simplified.value();
	auto maybe_nso_rr = get_nso_rr(fm);
	if (!maybe_nso_rr.has_value()) {
		r.merge(std::move(maybe_nso_rr));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	if (!maybe_nso_rr.value().main
		|| tau::get(maybe_nso_rr.value().main).is(tau::bf)) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto n = r.take_or_error(normalizer<node>(maybe_nso_rr.value()),
		code::internal_error, "Normalization failed");
	if (n) r = *n;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::normalize_term(tref term) {
	result<tref> r;
	if (!term) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	DBG(TAU_LOG_TRACE << "normalize_term(): " << LOG_FM_DUMP(term);)
	auto simplified = simplify(term);
	if (!simplified.has_value()) {
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	term = simplified.value();
	DBG(TAU_LOG_TRACE << "inferred term: " << LOG_FM_DUMP(term);)
	auto maybe_nso_rr = get_nso_rr(term);
	if (!maybe_nso_rr.has_value()) {
		r.merge(std::move(maybe_nso_rr));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto& nso_rr = maybe_nso_rr.value();
	tref main = nso_rr.main->get();
	if (!main || !tau::get(main).is(tau::bf)) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	tref n = contains(main, tau::ref)
		? bf_normalizer_with_rec_relation<node>(nso_rr)
		: bf_normalizer_without_rec_relation<node>(main);
	if (!n) r.error(code::internal_error, "Normalization failed");
	else    r = n;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::anti_prenex(tref fm) {
	result<tref> r;
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto simplified = simplify(fm);
	if (!simplified.has_value()) {
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	tref a = tau_lang::anti_prenex<node>(simplified.value());
	if (!a) r.error(code::internal_error, "Anti-prenex conversion failed");
	else    r = a;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::eliminate_quantifiers(tref fm) {
	result<tref> r;
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto simplified = simplify(fm);
	if (!simplified.has_value()) {
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto applied = apply_all_defs(simplified.value());
	if (!applied.has_value()) {
		r.merge(std::move(applied));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	tref e = resolve_quantifiers<node>(
		tau_lang::anti_prenex<node>(applied.value()));
	if (!e) r.error(code::internal_error, "Quantifier elimination failed");
	else    r = e;
	DBG(assert(r.is_well_formed());)
	return r;
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
	return pack_sat_status<node>(fm);
	}
}

/// Fast path for validity: fm is valid iff !fm is unsat.
template <NodeType node>
std::optional<bool> ba_fast_path_valid(tref fm) {
	using tau = tree<node>;
	if constexpr (!pack_has_arithmetic_theory_v<node>) return std::nullopt;
	else {
	if (!is_whole_query_ba_solvable<node>(fm)) return std::nullopt;
	// valid iff the negation is unsat, so the answer inverts.
	if (auto sat = pack_sat_status<node>(tau::build_wff_neg(fm)))
		return !*sat;
	return std::nullopt;
	}
}

template <NodeType node>
result<bool> api<node>::realizable(tref fm) {
	result<bool> r;
	auto simplified = simplify(fm);
	if (!simplified.has_value()) {
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// G(A) ∧ G(B) ≡ G(A ∧ B): merge top-level G-conjuncts before
	// normalization so the downstream pipeline sees a single wff_always.
	fm = flatten_always_conjuncts<node>(simplified.value());
	if (!fm || !is_formula(fm)) {
		r.error(code::invalid_argument, "Invalid formula");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// whole-query BA fast path; falls through when undecided.
	if (auto fast = ba_fast_path_sat<node>(fm); fast.has_value()) {
		r = fast.value();
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// normalize_formula() fails on failures that are reachable from user
	// input: a non-well-founded recurrence, a definition set whose
	// expansion never settles, a fallback type mismatch, or a get_nso_rr
	// failure. is_tau_formula_sat() dereferences its argument immediately,
	// so the failure has to be caught here; it decides unsatisfiable
	// rather than propagating an error, matching the definite answer
	// every other undecidable-shape gate in this function returns.
	auto nf = normalize_formula(fm);
	if (!nf.has_value()) {
		r.merge(std::move(nf));
		if (!r.has_error()) r.error(code::internal_error,
			"Could not normalize the formula; "
			"its satisfiability cannot be decided");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// LT-7: the synthesis backend reports "no verdict" by throwing
	// ltl_synthesis_error -- a timed-out, killed or misused ltlsynt is NOT an
	// UNREALIZABLE answer.  Nothing below this layer catches it, so without a
	// handler here a slow specification would terminate the process instead of
	// answering.  Convert it into a logged error verdict.
	try {
		// A data quantifier under a full-LTL operator survives normalization;
		// feeding that residue to is_tau_formula_sat breaks its no-quantifier
		// invariant, so route the RAW formula to the LTL-ABA solver instead.
		tref target = (has_ltl_operators<node>(fm)
			&& tau::get(nf.value()).find_top(is_quantifier<node>))
			? fm : nf.value();
		auto sat = is_tau_formula_sat<node>(target, 0, true);
		if (auto v = r.take_or_error(std::move(sat), code::internal_error,
			"is_tau_formula_sat returned neither a value nor an "
			"error while checking realizability"))
			r = *v;
	} catch (const ltl_synthesis_error& e) {
		TAU_LOG_ERROR << "UNKNOWN: the synthesis backend failed or timed out ("
			<< e.what() << "); realizability could not be decided";
		r.error(code::solver_error,
			"the synthesis backend failed or timed out");
	}
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<bool> api<node>::unrealizable(tref fm) {
	result<bool> r;
	// AP1-11: null input is invalid, not "unrealizable".
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto real = realizable(fm);
	if (!real.has_value()) r.merge(std::move(real));
	else                   r = !real.value();
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<bool> api<node>::sat(tref fm) {
	result<bool> r;
	auto simplified = simplify(fm);
	if (!simplified.has_value()) {
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// G(A) ∧ G(B) ≡ G(A ∧ B); merge top-level conjunctions of G so the
	// downstream safety pipeline sees one wff_always.  Non-mergeable
	// Boolean combinations (disjunction, negation, F-on-non-singletons,
	// etc.) survive flatten unchanged and are routed to the full-LTL
	// pipeline by is_tau_formula_sat itself — there's no longer a
	// pre-check that rejects them at this layer.
	fm = flatten_always_conjuncts<node>(simplified.value());
	if (!fm) {
		r.error(code::invalid_argument, "Invalid formula");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto real = realizable(fm);
	if (!real.has_value()) r.merge(std::move(real));
	else                   r = real.value();
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<bool> api<node>::unsat(tref fm) {
	result<bool> r;
	// AP1-11: null input is invalid, not "unsatisfiable" (see
	// unrealizable above).
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto s = sat(fm);
	if (!s.has_value()) r.merge(std::move(s));
	else                r = !s.value();
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<bool> api<node>::valid(tref fm) {
	result<bool> r;
	auto simplified = simplify(fm);
	if (!simplified.has_value()) {
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	fm = flatten_always_conjuncts<node>(simplified.value());
	if (!fm) {
		r.error(code::invalid_argument, "Invalid formula");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto vs = valid_spec(fm);
	if (!vs.has_value()) r.merge(std::move(vs));
	else                 r = vs.value();
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<bool> api<node>::valid_spec(tref fm) {
	result<bool> r;
	auto simplified = simplify(fm);
	if (!simplified.has_value()) {
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	fm = simplified.value();
	// whole-query BA fast path; falls through when undecided.
	if (auto fast = ba_fast_path_valid<node>(fm); fast.has_value()) {
		r = fast.value();
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// Same contract as realizable(): is_tau_impl() normalizes both
	// arguments straight away and cannot be handed a null formula, so a
	// normalization failure decides invalid rather than propagating an
	// error.
	auto nfm = normalize_formula(fm);
	if (!nfm.has_value()) {
		r.merge(std::move(nfm));
		if (!r.has_error()) r.error(code::internal_error,
			"Could not normalize the formula; "
			"its validity cannot be decided");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// Valid iff T (tautology) implies the normalized formula.
	// Same synthesis-failure gate as realizable() -- see the note there.
	try {
		auto impl = is_tau_impl<node>(tau::_T(), nfm.value());
		if (auto v = r.take_or_error(std::move(impl), code::internal_error,
			"is_tau_impl returned neither a value nor an error "
			"while checking validity"))
			r = *v;
	} catch (const ltl_synthesis_error& e) {
		TAU_LOG_ERROR << "UNKNOWN: the synthesis backend failed or timed out ("
			<< e.what() << "); validity could not be decided";
		r.error(code::solver_error,
			"the synthesis backend failed or timed out");
	}
	DBG(assert(r.is_well_formed());)
	return r;
}


// Solving
// ------------------------------------------------------------

// Solve: apply defs, reject temporal quantifiers, then run the solver.
template <NodeType node>
result<subtree_map<node, tref>> api<node>::solve(
	tref fm, solver_mode mode)
{
	result<subtree_map<node, tref>> r;
	auto simplified = simplify(fm);
	if (!simplified.has_value()) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto applied = apply_all_defs(simplified.value());
	if (!applied.has_value()) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		r.merge(std::move(applied));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	tref a = applied.value();
	// Reject formula involving temporal quantification
	if (tau::get(a).find_top(is_temporal_quantifier<node>)) {
		TAU_LOG_ERROR << "Found temporal quantifier in formula: "
			<< TAU_TO_STR(fm);
		r.error(code::invalid_argument,
			"Found temporal quantifier in formula");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	DBG(TAU_LOG_TRACE << "solve: " << LOG_FM(a);)
	// setting solver options
	solver_options options = {
		.splitter_one = node::ba::
			splitter_one(tau_type<node>()),
		.mode = mode
	};
	// Use fully-expanded formula (a) so function/predicate refs are resolved
	// before reaching type-specific solvers (fixes bug with typed functions).
	auto solution = tau_lang::solve<node>(a, options);
	if (auto v = r.take_or_error(std::move(solution), code::internal_error,
		"tau_lang::solve returned neither a value nor an error"))
		r = std::move(*v);
	DBG(assert(r.is_well_formed());)
	return r;
}

// LGRS: normalize the equation, extract the bf_eq equality, then solve.
template <NodeType node>
result<subtree_map<node, tref>> api<node>::lgrs(tref equation) {
	result<subtree_map<node, tref>> r;
	using tt = tau::traverser;
	auto simplified = simplify(equation);
	if (!simplified.has_value()) {
		r.merge(std::move(simplified));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto applied = apply_all_defs(simplified.value());
	if (!applied.has_value()) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		r.merge(std::move(applied));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	tref a = applied.value();
	tref eq = apply_all_xor_def<node>(norm_all_equations<node>(a));
	tref equality = tt(eq) | tau::bf_eq | tt::ref;
	if (!eq || !equality) {
		TAU_LOG_ERROR << "Invalid argument(s)";
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// Exclude non-Boolean operations from equation. The two sides live under
	// the bf_eq, not under `eq`: `eq` is the wff wrapping it and has a
	// single child, so indexing it with [1] tripped the `c != nullptr`
	// assert in tree<node>::child_tree (Debug) and read a null child
	// (Release).
	if (tau::get(equality)[0].find_top(is_non_boolean_term<node>) ||
		tau::get(equality)[1].find_top(is_non_boolean_term<node>)) {
		TAU_LOG_ERROR << "Found non-Boolean operation in equation";
		r.error(code::invalid_argument,
			"Found non-Boolean operation in equation");
		DBG(assert(r.is_well_formed());)
		return r;
	}

	DBG(TAU_LOG_TRACE << "lgrs/applied: " << LOG_FM(eq);)
	DBG(TAU_LOG_TRACE << "lgrs/equality: " << LOG_FM(equality);)

	auto solution = tau_lang::lgrs<node>(eq);
	if (auto v = r.take_or_error(std::move(solution), code::internal_error,
		"tau_lang::lgrs returned neither a value nor an error"))
		r = std::move(*v);
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

template <NodeType node>
result<interpreter<node>> api<node>::get_interpreter(tref spec,
	interpreter_options& options)
{
	result<interpreter<node>> r;
	// Assign the remaps into the global io_context only after every
	// validation step succeeds: assigning them up front left them in
	// place -- corrupting later, unrelated calls -- on every one of the
	// early-return failure paths below.
	auto& ctx = *definitions<node>::instance().get_io_context();
	auto maybe_nso_rr = get_nso_rr(spec);
	if (!maybe_nso_rr.has_value()) {
		r.merge(std::move(maybe_nso_rr));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto applied = r.take_or_error(nso_rr_apply<node>(maybe_nso_rr.value()),
		code::internal_error, "Failed to apply definitions");
	if (!applied) {
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto normalized = r.take_or_error(normalizer<node>(*applied),
		code::internal_error, "Normalization failed");
	if (!normalized) {
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// normalized is ctx-resolved (get_nso_rr), never a bare-reparsed atom.
	if (has_free_vars<node>(*normalized)) {
		r.error(code::invalid_argument, "Spec contains free variables");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	ctx.input_remaps = options.input_remaps;
	ctx.output_remaps = options.output_remaps;
	// LT-7: make_interpreter reaches ltlsynt through
	// ltl_to_safety_formula_full; a backend failure must not terminate the
	// caller.  No interpreter is the honest answer here.
	try {
		auto intr = r.take_or_error(
			interpreter<node>::make_interpreter(*normalized, ctx),
			code::solver_error,
			"the specification could not be compiled");
		if (!intr) {
			DBG(assert(r.is_well_formed());)
			return r;
		}
		r = std::move(*intr);
	} catch (const ltl_synthesis_error& e) {
		TAU_LOG_ERROR << "UNKNOWN: the synthesis backend failed or timed out ("
			<< e.what() << "); the specification could not be compiled";
		r.error(code::solver_error,
			"the synthesis backend failed or timed out");
	}
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
	result<interpreter<node>> r;
	// See the tref overload above: remaps are assigned into the global
	// io_context only once every validation step has succeeded.
	auto& ctx = *definitions<node>::instance().get_io_context();
	auto maybe_nso_rr = spec.get_nso_rr();
	if (!maybe_nso_rr) {
		for (const auto& error : spec.errors()) {
			TAU_LOG_ERROR << error;
			r.error(code::parse_error, error);
		}
		if (!r.has_error()) r.error(code::parse_error, "Failed to parse spec");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto applied = r.take_or_error(nso_rr_apply<node>(maybe_nso_rr.value()),
		code::internal_error, "Failed to apply definitions");
	if (!applied) {
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto normalized = r.take_or_error(normalizer<node>(*applied),
		code::internal_error, "Normalization failed");
	if (!normalized) {
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// normalized is ctx-resolved (get_nso_rr), never a bare-reparsed atom.
	if (has_free_vars<node>(*normalized)) {
		r.error(code::invalid_argument, "Spec contains free variables");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	ctx.input_remaps = options.input_remaps;
	ctx.output_remaps = options.output_remaps;
	// See the tref overload: synthesis-backend failures are answered, not
	// propagated.
	try {
		auto intr = r.take_or_error(
			interpreter<node>::make_interpreter(*normalized, ctx),
			code::solver_error,
			"the specification could not be compiled");
		if (!intr) {
			DBG(assert(r.is_well_formed());)
			return r;
		}
		r = std::move(*intr);
	} catch (const ltl_synthesis_error& e) {
		TAU_LOG_ERROR << "UNKNOWN: the synthesis backend failed or timed out ("
			<< e.what() << "); the specification could not be compiled";
		r.error(code::solver_error,
			"the synthesis backend failed or timed out");
	}
	DBG(assert(r.is_well_formed());)
	return r;
}

// private helper methods
// ------------------------------------------------------------

// Private: extract rr<node> from expression tree.
// For spec nodes, delegates to tau_lang::get_nso_rr.
// For bare wff/bf nodes, wraps via resolve_io_vars.
template <NodeType node>
result<rr<node>> api<node>::get_nso_rr(tref expr) {
	result<rr<node>> r;
	if (!expr) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
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
			else {
				r.error(code::internal_error,
					"Failed to resolve recurrence relations");
				DBG(assert(r.is_well_formed());)
				return r;
			}
		} else {
			nso_rr.main = tau::geth(resolve_io_vars<node>(ctx, expr));
			if (!nso_rr.main) {
				r.error(code::internal_error,
					"Failed to resolve I/O variables");
				DBG(assert(r.is_well_formed());)
				return r;
			}
		}
	} else nso_rr.main = tau::geth(resolve_io_vars<node>(ctx, expr));
	r = std::move(nso_rr);
	DBG(assert(r.is_well_formed());)
	return r;
}

// Type inference pipeline: infer BA types → canonize quantifier IDs
// → unnest G-in-G ambiguity → check semantics → update global scope.
template <NodeType node>
result<tref> api<node>::infer(tref expr, bool use_defaults) {
	result<tref> r;
	if (!expr) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}

	auto& defs = definitions<node>::instance();
	auto infer_result = infer_ba_types<node>(expr,
		defs.get_global_scope(),
		defs.get_definition_heads(),
		{ .use_defaults = use_defaults });
	tref inferred = canonize_quantifier_ids<node>(infer_result.first);
	if (!inferred) {
		DBG(LOG_TRACE << "inferred is nullptr";)
		r.error(code::internal_error, "Type inference failed");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	defs.get_io_context()->update_types(infer_result.second);
	defs.set_global_scope(std::move(infer_result.second));

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
		r.error(code::invalid_argument, "Expression has semantic error");
		DBG(assert(r.is_well_formed());)
		return r;
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
	r = inferred;
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<tref> api<node>::simplify(tref expr, bool use_defaults) {
	result<tref> r;
	if (!expr) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inferred = infer(expr, use_defaults);
	if (!inferred.has_value()) {
		r.merge(std::move(inferred));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	tref e = canonize_quantifier_ids<node>(tau::reget(inferred.value()));
	if (!e) r.error(code::internal_error, "Simplification failed");
	else {
		DBG(TAU_LOG_TRACE << "simplified: " << LOG_FM_DUMP(e);)
		r = e;
	}
	DBG(assert(r.is_well_formed());)
	return r;
}

} // namespace idni::tau_lang
