// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "interpreter.h"
#include "repl_evaluator.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "repl_evaluator"

namespace idni::tau_lang {

using namespace cvc5;

#define TC_STATUS        TC.BG_LIGHT_CYAN()
#define TC_STATUS_OUTPUT TC(term::color::GREEN, term::color::BG_LIGHT_CYAN, \
							term::color::BRIGHT)
#define TC_ERROR         TC(term::color::RED,   term::color::BRIGHT)
#define TC_PROMPT        TC(term::color::WHITE, term::color::BRIGHT)
#define TC_OUTPUT        TC.GREEN() // TODO: change to term::color::GREEN()

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::not_implemented_yet() {
	std::cout << "Not implemented yet.\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::invalid_argument() const {
	TAU_LOG_ERROR << "Invalid argument\n";
	return nullptr;
}

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<size_t> repl_evaluator<BAs...>::get_history_index(
	const tt& n, const size_t size, bool silent) const
{
	if (size == 0) {
		if (!silent) std::cout << "history is empty\n";
		return {};
	}
	auto mem_type = n | tt::only_child | tt::nt;
	auto is_relative = (mem_type == tau::rel_history);
	auto mem_id = n | mem_type | tau::history_id;
	size_t idx = 0;
	if (mem_id) idx = mem_id | tt::num;
	// TAU_LOG_TRACE << "get_history_index idx: " << idx
	// 	<< "       relative? " << is_relative << "    "
	// 	<< TAU_LOG_FM(n.value());
	if ((is_relative && idx >= size)
		|| (!is_relative && (idx == 0 || idx > size)))
	{
		if (!silent) {
			std::cout << "history location " << TC_OUTPUT
				<< (is_relative ? "%-" : "%");
			if (!is_relative || idx) std::cout << idx;
			std::cout << TC.CLEAR() << " does not exist\n";
		}
		return {};
	}
	// TAU_LOG_TRACE << "get_history_index result: "
	// 	<< (is_relative ? size - idx - 1 : idx - 1);
	return { is_relative ? size - idx - 1 : idx - 1 };
}

template <typename... BAs>
requires BAsPack<BAs...>
repl_evaluator<BAs...>::history_ref repl_evaluator<BAs...>::history_retrieve(
	const tt& n, bool silent) const
{
	if (auto pos = get_history_index(n, H.size(), silent); pos.has_value())
		return { { H[pos.value()], pos.value() } };
	TAU_LOG_ERROR << "History location does not exist\n";
	return {};
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::print_history(const htref& mem, const size_t id,
	const size_t size, bool print_relative_index) const
{
	std::cout << TC_OUTPUT << "%" << id + 1 << TC.CLEAR();
	if (print_relative_index) {
		std::cout << "/" << TC_OUTPUT;
		if (size - id == 1) std::cout << "%";
		else std::cout << "%-" << size - id - 1;
		std::cout << TC.CLEAR();
	}
	std::cout << ": ";
	tau::get(mem).print(std::cout) << "\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<rr<node<tau_ba<BAs...>, BAs...>>>
	repl_evaluator<BAs...>::get_nso_rr_with_defs(const tt& spec)
{
	rr<node> nso_rr{ nullptr };
	auto check = get_type_and_arg(spec);
	if (!check) return {};
	auto [type, value] = check.value();
	auto& defs = definitions<node>::instance();
	spec_context<node>& ctx = defs.get_io_context();
	if (contains(value, tau::ref)) {
		if (type == tau::spec) {
			if (auto x = get_nso_rr<node>(ctx, value); x)
				nso_rr = x.value();
			else return {};
		} else nso_rr = rr<node>(tau::geth(
				resolve_io_vars<node>(ctx, value)));
		auto definitions = defs.get_sym_defs();
		nso_rr.rec_relations.insert(nso_rr.rec_relations.end(),
			definitions.begin(), definitions.end());
		auto& global_scope = defs.get_global_scope();
		if (auto [infr, n_global_scope] = infer_ba_types<node>(
			build_spec<node>(nso_rr), global_scope); infr)
		{
			global_scope = n_global_scope;
			if (auto infr_rr = get_nso_rr<node>(ctx, infr); infr_rr)
				return infr_rr.value();
			else return {};
		} else return {};
	} else return rr<node>(tau::geth(resolve_io_vars<node>(ctx, value)));
	return {};
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::apply_rr_to_nso_rr_with_defs(const tt& spec) {
	auto nso_rr = get_nso_rr_with_defs(spec);
	if (!nso_rr) return {};
	return apply_rr_to_formula<node>(nso_rr.value());
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::history_print_cmd(const tt& command) {
	auto n = command | tau::history;
	if (!n) return;
	auto idx = get_history_index(n.value(), H.size());
	if (idx) print_history(H[idx.value()], idx.value(), H.size());
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::history_list_cmd() {
	if (H.size() == 0) std::cout << "history is empty\n";
	else for (size_t i = 0; i < H.size(); i++)
		print_history(H[i], i, H.size());
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::history_store(tref o) {
	auto v = tau::geth(o);
	// do not add into history if the last history value is the same
	if (!(H.size() && H.back() == v))
		H.push_back(v);
	if (opt.print_history_store)
		print_history(H.back(), H.size() - 1, H.size(), false);
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::history_store_cmd(const tt& command) {
	history_store(command | tt::first | tt::ref);
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::get_(typename node::type nt, tref n,
	bool suppress_error) const
{
	// TAU_LOG_TRACE << "get_/n: " << node::name(nt) << "        " << TAU_DUMP_TO_STR(n);
	if (tau::get(n).is(nt)) return n;
	else if (tau::get(n).is(tau::history)) {
		if (auto check = history_retrieve(n); check) {
			const auto& h = check.value().first;
			if (tau::get(h).is(nt)) return h->get();
			else if (!suppress_error)
				TAU_LOG_ERROR << "Argument has a wrong type";
			return nullptr;
		}
	}
	if (!suppress_error) TAU_LOG_ERROR << "Argument has a wrong type";
	return nullptr;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::get_bf(tref n, bool suppress_error) const {
	return get_(tau::bf, n, suppress_error);
}
template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::get_wff(tref n) const {
	return get_(tau::wff, n, false);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool repl_evaluator<BAs...>::contains(const tt& n, typename node::type nt) const {
	bool found = false;
	const auto searcher = [&nt, &found](tref n) -> bool {
		if (tau::get(n).get_type() == nt) return found = true, false;
		return true;
	};
	pre_order<node>(n.value()).search(searcher);
	return found;
}

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<std::pair<size_t, tref>>
	repl_evaluator<BAs...>::get_type_and_arg(const tt& n) const
{
	auto nt = n | tt::nt;
	switch (nt) {
	case tau::history:
		if (auto check = history_retrieve(n); check) {
			auto [value, _] = check.value();
			auto mem_type = tt(value) | tt::nt;
			return { { mem_type, value->get() } };
		} else return {};
	default: return { std::pair<size_t, tref>{ nt, n | tt::ref } };
	}
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::onf_cmd(const tt& n) {
	tref arg = n[1].get();
	tref var = n[2].get();
	tref applied = apply_rr_to_nso_rr_with_defs(arg);
	if (applied) return onf<node>(var, applied);
	return nullptr;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::dnf_cmd(const tt& n) {
	tref arg = n[1].get();
	tref applied = apply_rr_to_nso_rr_with_defs(arg);
	if (applied) {
		switch (tau::get(applied).get_type()) {
		case tau::bf:  return reduce<node>(to_dnf<node, false>(applied));
		case tau::wff: return reduce<node>(to_dnf<node>(applied));
		default: return invalid_argument();
		}
	}
	return nullptr;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::cnf_cmd(const tt& n) {
	tref arg = n[1].get();
	tref applied = apply_rr_to_nso_rr_with_defs(arg);
	if (applied) {
		switch (tau::get(applied).get_type()) {
		case tau::wff: return reduce<node, true>(to_cnf<node>(applied));
		case tau::bf:  return reduce<node, true>(to_cnf<node, false>(
					applied));
		default: return invalid_argument();
		}
	}
	return nullptr;
}


template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::nnf_cmd(const tt& n) {
	tref arg = n[1].get();
	tref applied = apply_rr_to_nso_rr_with_defs(arg);
	if (applied) {
		switch (tau::get(applied).get_type()) {
		case tau::wff: return to_nnf<node>(applied);
		case tau::bf:  return push_negation_in<node, false>(applied);
		default: return invalid_argument();
		}
	}
	return nullptr;
}


template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::mnf_cmd(const tt& n) {
	tref arg = n[1].get();
	tref applied = apply_rr_to_nso_rr_with_defs(arg);
	if (applied) {
		switch (tau::get(applied).get_type()) {
		case tau::wff: return unequal_to_not_equal<node>(
			reduce<node>(to_dnf<node>(
				bf_reduce_canonical<node>()(applied)
				)));
		case tau::bf:  return bf_reduced_dnf<node>(applied);
		default: return invalid_argument();
		}
	}
	return nullptr;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::subst_cmd(const tt& n) {
	tref arg1 = n | tt::second | tt::ref;
	tref arg2 = n | tt::third  | tt::ref;
	tref arg3 = n | tt::fourth | tt::ref;
	// TAU_LOG_TRACE << "subst_cmd arg1: " << TAU_DUMP_TO_STR(arg1);
	// TAU_LOG_TRACE << "subst_cmd arg2: " << TAU_DUMP_TO_STR(arg2);
	// TAU_LOG_TRACE << "subst_cmd arg3: " << TAU_DUMP_TO_STR(arg3);
	// Since the history command cannot be type-checked we do it here
	// First try to get bf
	tref in = get_bf(arg1, true);
	if (in) { // BF substitution
		tref thiz = get_bf(arg2), with = get_bf(arg3);
		if (!in || !thiz || !with) return invalid_argument();
		return tau::get(in).substitute(thiz, with);
	}
	// First argument was not a bf so it must be a wff
	in = get_wff(arg1);
	// Now sort out the remaining argument types
	tref with, thiz = get_bf(arg2, true);
	if (thiz) with = get_bf(arg3);
	else thiz = get_wff(arg2), with = get_wff(arg3);
	// Check for correct argument types
	if (!thiz || !in || !with) {
		TAU_LOG_ERROR << "Invalid argument\n";
		return nullptr;
	}
	return tau::get(in).substitute(thiz, with);
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::inst_cmd(const tt& n) {
	const auto& t = n.value_tree();
	if (!t[2][0].is(tau::variable)) {
		TAU_LOG_ERROR << "Invalid argument\n";
		return nullptr;
	}
	tref nn = tau::get(t.value, { t.first(), t.second(),
			t.third(), t.child(3) });
	return subst_cmd(nn);
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::normalize_cmd(const tt& n) {
	tref arg = n[1].get();
	auto opt_nso_rr = get_nso_rr_with_defs(arg);
	if (!opt_nso_rr || !opt_nso_rr.value().main) return invalid_argument();
	rr<node> nso_rr = opt_nso_rr.value();
	const auto& main = tau::get(nso_rr.main);
	bool contains_ref = contains(main.get(), tau::ref);
	if (!main.is(tau::bf)) return normalizer<node>(nso_rr);
	else if (contains_ref) return bf_normalizer_with_rec_relation<node>(nso_rr);
	return bf_normalizer_without_rec_relation<node>(main.get());
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::qelim_cmd(const tt& n) {
	tref arg = n[1].get();
	tref applied = apply_rr_to_nso_rr_with_defs(arg);
	if (applied) {
		applied = anti_prenex<node>(applied);
		return resolve_quantifiers<node>(applied);
	}
	return invalid_argument();
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::run_cmd(const tt& n) {

	tref arg = n[1].get();
	tref applied = apply_rr_to_nso_rr_with_defs(arg);
	if (!applied) {
		invalid_argument();
		return;
	}

	DBG(TAU_LOG_DEBUG << "run_cmd/applied: " << TAU_LOG_FM(applied);)

	auto dnf = normalizer<node>(applied);

	// Make sure that there is no free variable in the formula
	if (has_free_vars<node>(dnf)) return;

	auto& ctx = definitions<node>::instance().get_io_context();
	auto ins = finputs<node>(collect_input_streams<node>(dnf, ctx));
	auto outs = foutputs<node>(collect_output_streams<node>(dnf, ctx));
	run<node>(dnf, ins, outs, ctx);
}

template <NodeType node>
solver_mode get_solver_cmd_mode(tref n) {
	using tau = tree<node>;
	if (tref solver_mode = tau::get(n).find_top(is<node, tau::solver_mode>);
		solver_mode)
	{
		typename node::type mode = tau::get(solver_mode)[0].get_type();
		return (mode == tau::solver_mode_minimum) ? solver_mode::minimum
							: solver_mode::maximum;
	} else return solver_mode::general;
}

template <NodeType node>
size_t get_solver_cmd_type(tref n) {
	size_t type = find_ba_type<node>(n);
	return type > 0 ? type
		: get_ba_type_id<node>(
			node::nso_factory::default_type());
}

template <NodeType node>
void print_solver_cmd_solution(std::optional<solution<node>>& solution,
		size_t type_id)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	auto print_zero_case = [&type_id](tref var) {
		std::cout << "\t" << TAU_TO_STR(var) << " := {"
			<< node::nso_factory::zero(get_ba_type_tree<node>(type_id))
			<< "}:" << ba_types<node>::name(type_id) << "\n";
	};

	auto print_one_case = [&type_id](tref var) {
		std::cout << "\t" << TAU_TO_STR(var) << " := {"
			<< node::nso_factory::one(get_ba_type_tree<node>(type_id))
			<< "}:" << ba_types<node>::name(type_id) << "\n";
	};

	auto print_general_case = [](tref var, tref value) {
		std::cout << "\t" << TAU_TO_STR(var) << " := "
			<< TAU_TO_STR(value) << "\n";
	};

	if (!solution) { std::cout << "no solution\n"; return; }

	std::cout << "solution: {\n";
	for (auto [var, value]: solution.value()) {
		DBG(LOG_TRACE << LOG_FM_TREE(var));
		DBG(LOG_TRACE << LOG_FM_TREE(value));
		if (auto check = tt(value) | tau::bf_t; check)
			print_one_case(var);
		else if (check = tt(value) | tau::bf_f; check)
			print_zero_case(var);
		else
			print_general_case(var, value);
	}
	std::cout << "}\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::solve_cmd(const tt& n) {
	// setting solver options
	solver_options options = {
		.splitter_one = node::nso_factory::
			splitter_one(tau_type<node>()),
		.mode = get_solver_cmd_mode<node>(n.value()),
	};

	tref arg = n.value_tree().first();
	while (tau::get(arg).has_right_sibling())
		arg = tau::get(arg).right_sibling();
	tref applied = apply_rr_to_nso_rr_with_defs(arg);
	if (!applied) {
		TAU_LOG_ERROR << "Invalid argument(s)\n";
		return;
	}
	// Reject formula involving temporal quantification
	if (tau::get(applied).find_top(is_temporal_quantifier<node>)) {
		TAU_LOG_ERROR << "Found temporal quantifier in formula: " << TAU_TO_STR(applied);
		return;
	}

	DBG(TAU_LOG_TRACE << "solve_cmd/applied: " << applied << "\n";)

	size_t type = get_solver_cmd_type<node>(applied);
	bool solve_error = false;
	auto solution = solve<node>(applied, options, solve_error);
	if (solve_error) {
		TAU_LOG_ERROR << "Internal error in solver\n";
		return;
	}
	if (!solution) { std::cout << "no solution\n"; return; }

	print_solver_cmd_solution<node>(solution, type);
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::lgrs_cmd(const tt& n) {
	// getting the type
	size_t type = get_solver_cmd_type<node>(n.value());
	if (type == 0) {
		TAU_LOG_ERROR << "Invalid type\n";
		return;
	}

	tref arg = n.value_tree().first();
	while (tau::get(arg).has_right_sibling())
		arg = tau::get(arg).right_sibling();
	tref applied = apply_rr_to_nso_rr_with_defs(arg);
	applied = norm_all_equations<node>(applied);
	applied = apply_all_xor_def<node>(applied);
	tref equality = tt(applied) | tau::bf_eq | tt::ref;
	if (!applied || !equality) {
		TAU_LOG_ERROR << "Invalid argument(s)\n";
		return;
	}
	// Exclude non-Boolean operations from equation
	if (tau::get(applied)[0].find_top(is_non_boolean_term<node>) ||
		tau::get(applied)[1].find_top(is_non_boolean_term<node>)) {
		TAU_LOG_ERROR << "Found non-Boolean operation in equation\n";
		return;
	}

	DBG(TAU_LOG_TRACE << "lgrs_cmd/applied: " << TAU_LOG_FM(applied) << "\n";)
	DBG(TAU_LOG_TRACE << "lgrs_cmd/equality: " << TAU_LOG_FM(equality) << "\n";)

	auto solution = lgrs<node>(applied);
	if (!solution) { std::cout << "no solution\n"; return; }
	// trefs vars = tau::get(equations).select_top(is_child<node, tau::variable>);
	print_solver_cmd_solution<node>(solution, type);
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::valid_cmd(const tt& n) {
	tref arg = n[1].get();
	auto opt_nso_rr = get_nso_rr_with_defs(arg);
	if (!opt_nso_rr || !opt_nso_rr.value().main) return invalid_argument();
	rr<node> nso_rr = opt_nso_rr.value();
	const auto& main = tau::get(nso_rr.main);
	if (main.is(tau::bf)) return invalid_argument();
	tref normalized = normalizer<node>(nso_rr);
	return is_tau_impl<node>(tau::_T(), normalized) ? tau::_T() : tau::_F();
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::sat_cmd(const tt& n) {
	tref arg = n[1].get();
	auto opt_nso_rr = get_nso_rr_with_defs(arg);
	if (!opt_nso_rr || !opt_nso_rr.value().main) return invalid_argument();
	rr<node> nso_rr = opt_nso_rr.value();
	const auto& main = tau::get(nso_rr.main);
	if (main.is(tau::bf)) return invalid_argument();
	tref normalized = normalizer<node>(nso_rr);
	return is_tau_formula_sat<node>(normalized, 0, true)
		? tau::_T()	: tau::_F();
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::unsat_cmd(const tt& n) {
	return tau::get(sat_cmd(n)).equals_F() ? tau::_T() : tau::_F();
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::def_rr_cmd(const tt& n) {
	auto& defs = definitions<node>::instance();
	size_t idx = defs.add(tau::geth(n[0][0]), tau::geth(n[0][1]));
	std::cout << "[" << idx + 1 << "] " << to_str<node>(defs[idx]) << "\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::def_list_cmd() {
	auto& defs = definitions<node>::instance();
	if (defs.size() == 0) std::cout << "Definitions: empty\n";
	else std::cout << "Definitions:\n";
	for (size_t i = 0; i < defs.size(); i++)
		std::cout << "    [" << i + 1 << "] "
			<< to_str<node>(defs[i]) << "\n";
	std::cout << defs.get_io_context();
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::def_print_cmd(const tt& command) {
	auto num = command | tau::num;
	if (!num) return;
	auto i = num | tt::num;
	const auto& defs = definitions<node>::instance();
	if (i && i <= defs.size()) {
		print<node>(std::cout, defs[i-1]) << "\n";
		return;
	}
	TAU_LOG_ERROR << "Definition [" << i << "] does not exist\n";
	return;
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::def_input_cmd(const tt& command) {
	auto& defs = definitions<node>::instance();
	if (!get_io_def<node>(command | tt::only_child | tt::ref, defs.get_input_defs())) {
		error = true;
		TAU_LOG_ERROR << "Invalid type " << TAU_TO_STR(command.value());
	}
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::def_output_cmd(const tt& command) {
	auto& defs = definitions<node>::instance();
	if (!get_io_def<node>(command | tt::only_child | tt::ref, defs.get_output_defs())){
		error = true;
		TAU_LOG_ERROR << "Invalid type " << TAU_TO_STR(command.value());
	}
}

// make a nso_rr from the given tau source and binder.
template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::make_cli(const std::string& src) {
	// remove ascii char 22 if exists in the input
	std::string filt = src;
	filt.erase(remove_if(filt.begin(), filt.end(), [](unsigned char c) {
		return c == 22;
	}), filt.end());
	tau_parser::result result = tau_parser::instance()
		.parse(filt.c_str(), filt.size(), {
						.start = tau::cli });
	auto fail = [this]() { return error = true, nullptr; };
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(tau_parser::error::info_lvl::INFO_BASIC);
		if (opt.error_quits
			|| msg.find("Syntax Error: Unexpected end")!=0)
		{
			TAU_LOG_ERROR << "[repl] " << msg << "\n";
			return fail();
		}
		return nullptr; // Unexpected eof, continue with reading input
	}
	auto t = result.get_shaped_tree2();
	auto& global_scope = definitions<node>::instance().get_global_scope();
	typename tau::get_options opts = {
		.global_scope = global_scope
	};
	auto bound = tau::get(tau_parser::tree::get(t), opts);
	if (!bound) return fail();
	global_scope = opts.global_scope;
	return bound;
}

inline repl_option get_opt(const std::string& x) {
	if (x.empty())                       return none_opt;
	if (x == "S" || x == "severity"
		|| x == "sev")               return severity_opt;
	if (x == "s" || x == "status")       return status_opt;
	if (x == "c" || x == "colors"
		|| x == "color")             return colors_opt;
	if (x == "V" || x == "charvar")      return charvar_opt;
	if (x == "H" || x == "highlighting"
		|| x == "highlight")         return highlighting_opt;
	if (x == "I" || x == "indenting"
		|| x == "indent")            return indenting_opt;
	if (x == "d" || x == "debug"
		|| x == "dbg")               return debug_opt;
	TAU_LOG_ERROR << "Invalid option: " << x << "\n";
	return invalid_opt;
}

template <NodeType node>
repl_option get_opt(const typename tree<node>::traverser& n) {
	auto o = n | tau_parser::option_name;
	if (!o) return none_opt;
	return get_opt(o | tree<node>::traverser::string);
}

inline std::optional<boost::log::trivial::severity_level>
	str2severity(const std::string& v)
{
	// TODO (LOW) should we add also warning? and what about fatal?
	if (v == "e" || v == "error") return { boost::log::trivial::error };
	if (v == "d" || v == "debug") return { boost::log::trivial::debug };
	if (v == "t" || v == "trace") return { boost::log::trivial::trace };
	if (v == "i" || v == "info")  return { boost::log::trivial::info };
	TAU_LOG_ERROR << "Invalid severity value: " << v
		<< " (only error, info, debug or trace are allowed)\n";
	return {};
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::get_cmd(const tt& n) {
	return get_cmd(get_opt<node>(n));
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::get_cmd(repl_option o) {
	static std::string pbool[] = { "off", "on" };
	static std::map<repl_option, std::function<void()>> printers = {
#ifdef DEBUG
	{ debug_opt, [this]() {
		std::cout << "debug-repl:          " << pbool[opt.debug_repl] << "\n"; } },
#endif // DEBUG
	{ status_opt,       [this]() {
		std::cout << "status:              " << pbool[opt.status] << "\n"; } },
	{ colors_opt,       [this]() {
		std::cout << "colors:              " << pbool[opt.colors] << "\n"; } },
	{ charvar_opt,      [this]() {
		std::cout << "charvar:             " << pbool[opt.charvar] << "\n"; } },
	{ highlighting_opt, []() {
		std::cout << "syntax highlighting: " << pbool[pretty_printer_highlighting] << "\n"; } },
	{ indenting_opt,    []() {
		std::cout << "indenting:           " << pbool[pretty_printer_indenting] << "\n"; } },
	{ severity_opt,     [this]() {
		std::cout << "severity:            " << opt.severity << "\n"; } }};
	if (o == invalid_opt) return;
#ifndef DEBUG
	if (o == debug_opt) {
		TAU_LOG_ERROR << "Debug option not available in release build\n";
		return;
	}
#endif // DEBUG
	if (o == none_opt) { for (auto& [_, v] : printers) v(); return; }
	printers[o]();
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::set_cmd(const tt& n) {
	repl_option o = get_opt<node>(n);
	auto ov = n | tau::option_value;
	if (!ov) { TAU_LOG_ERROR << "Invalid value\n"; return; }
	set_cmd(o, ov | tt::string);
	get_cmd(n);
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::set_cmd(repl_option o, const std::string& v) {
	using namespace boost::log;
	if (o == invalid_opt || o == none_opt) return;
#ifndef DEBUG
	if (o == debug_opt) {
		TAU_LOG_ERROR << "Debug option not available\n";
		return;
	}
#endif // DEBUG
	auto update_bool_value = [&v](bool& opt) {
		if (v == "t" || v == "true" || v == "on" || v == "1"
			|| v == "y" || v == "yes") opt = true;
		else if (v == "f" || v == "false" || v == "off" || v == "0"
			|| v == "n" || v == "no") opt = false;
		else TAU_LOG_ERROR << "Invalid value\n";
		return opt;
	};
	static std::map<repl_option, std::function<void()>> setters = {
#ifdef DEBUG
	{ debug_opt, [&]() {
		update_bool_value(opt.debug_repl); } },
#endif // DEBUG
	{ status_opt,   [&]() {
		update_bool_value(opt.status); } },
	{ colors_opt,   [&]() {
		TC.set(update_bool_value(opt.colors)); } },
	{ charvar_opt,   [&]() {
		update_charvar(update_bool_value(opt.charvar)); } },
	{ highlighting_opt,   [&]() {
		update_bool_value(pretty_printer_highlighting); } },
	{ indenting_opt,   [&]() {
		update_bool_value(pretty_printer_indenting); } },
	{ severity_opt, [&]() {
		auto sev = str2severity(v);
		if (!sev.has_value()) return;
		opt.severity = sev.value();
		logging::set_filter(opt.severity);
	} } };
	setters[o]();
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::update_bool_opt_cmd(const tt& n,
	const std::function<bool(bool&)>& update_fn)
{
	auto o = get_opt<node>(n);
	update_bool_opt_cmd(o, update_fn);
	get_cmd(n);
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::update_bool_opt_cmd(repl_option o,
	const std::function<bool(bool&)>& update_fn)
{
	if (o == invalid_opt || o == none_opt) return;
#ifndef DEBUG
	if (o == debug_opt) {
		TAU_LOG_ERROR << "Debug option not available\n";
		return;
	}
#endif // DEBUG
	switch (o) {
#ifdef DEBUG
	case debug_opt: update_fn(opt.debug_repl); break;
#endif // DEBUG
	case colors_opt:       TC.set(update_fn(opt.colors)); break;
	case charvar_opt:      update_charvar(update_fn(opt.charvar)); break;
	case highlighting_opt: update_fn(pretty_printer_highlighting); break;
	case indenting_opt:    update_fn(pretty_printer_indenting); break;
	case status_opt:       update_fn(opt.status); break;
	default: TAU_LOG_ERROR << "Invalid option\n", error = true; return;
	}
}

template <typename... BAs>
requires BAsPack<BAs...>
bool repl_evaluator<BAs...>::update_charvar(bool value) {
	std::set<std::string> guards{
		(opt.charvar = value) ? "charvar" : "var" };
	tau_parser::instance().get_grammar().set_enabled_productions(guards);
	sbf_parser::instance().get_grammar().set_enabled_productions(guards);
	return value;
}

template <typename... BAs>
requires BAsPack<BAs...>
int repl_evaluator<BAs...>::eval_cmd(const tt& n) {
	auto command = n | tt::only_child;
	auto command_type = command | tt::nt;
#ifdef DEBUG
	if (opt.debug_repl) {
		// std::cout << "command: " << command << "\n";
		command.value_tree().print_tree(std::cout << "tree: ") << "\n";
	}
#endif // DEBUG
	tref result = 0;
	switch (command_type) {
	case tau::quit_cmd:           return std::cout << "Quit.\n", 1;
	case tau::clear_cmd:          if (r) r->clear(); break;
	case tau::help_cmd:           help_cmd(command); break;
	case tau::version_cmd:        version_cmd(); break;
	case tau::get_cmd:            get_cmd(command); break;
	case tau::set_cmd:            set_cmd(command); break;
	case tau::enable_cmd:         update_bool_opt_cmd(command,
					[](bool& b){ return b = true; }); break;
	case tau::disable_cmd:        update_bool_opt_cmd(command,
					[](bool& b){ return b = false; });break;
	case tau::toggle_cmd:         update_bool_opt_cmd(command,
					[](bool& b){ return b = !b; }); break;
	case tau::history_list_cmd:   history_list_cmd(); break;
	case tau::history_print_cmd:  history_print_cmd(command); break;
	case tau::history_store_cmd:  history_store_cmd(command); break;
	// normalization
	case tau::normalize_cmd:      result = normalize_cmd(command); break;
	// execution
	case tau::run_cmd:            run_cmd(command); break;
	case tau::solve_cmd:          solve_cmd(command); break;
	case tau::lgrs_cmd:           lgrs_cmd(command); break;
	// substitution and instantiation
	case tau::subst_cmd:          result = subst_cmd(command); break;
	case tau::inst_cmd:           result = inst_cmd(command); break;
	// formula checks
	case tau::sat_cmd:            result = sat_cmd(command); break;
	case tau::valid_cmd:          result = valid_cmd(command); break;
	case tau::unsat_cmd:          result = unsat_cmd(command); break;
	// normal forms
	case tau::onf_cmd:            result = onf_cmd(command); break;
	case tau::dnf_cmd:            result = dnf_cmd(command); break;
	case tau::cnf_cmd:            result = cnf_cmd(command); break;
	case tau::anf_cmd:            not_implemented_yet(); break;
	case tau::nnf_cmd:            result = nnf_cmd(command); break;
	case tau::pnf_cmd:            not_implemented_yet(); break;
	case tau::mnf_cmd:            result = mnf_cmd(command); break;
	// definition of rec relations to be included during normalization
	case tau::def_rr_cmd:         def_rr_cmd(command); break;
	case tau::def_list_cmd:       def_list_cmd(); break;
	case tau::def_print_cmd:      def_print_cmd(command); break;
	// definitions of i/o streams
	case tau::def_input_cmd:      def_input_cmd(command); break;
	case tau::def_output_cmd:     def_output_cmd(command); break;
	// qelim
	case tau::qelim_cmd:          result = qelim_cmd(command); break;
	case tau::comment:            break;
	// error handling
	default: error = true; std::cout << std::endl;
		TAU_LOG_ERROR << "Unknown command";
	}
#ifdef DEBUG
	if (opt.debug_repl && result) tau::get(result).print_tree(
		std::cout << "result tree: ") << "\n";
#endif // DEBUG
	if (result) history_store(result);
	return 0;
}

template <typename... BAs>
requires BAsPack<BAs...>
repl_evaluator<BAs...>::repl_evaluator(options opt): opt(opt)
{
	TC.set(opt.colors);
	logging::set_filter(opt.severity);
	// Controls how fixpoint information in satisfiability.h should be printed
	if (!opt.repl_running) use_debug_output_in_sat = true;
	if (opt.experimental) std::cout << "\n!!! Experimental features "
		"enabled: new tree API (almost nothing works yet) !!!\n\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
std::string repl_evaluator<BAs...>::prompt() {
	std::stringstream ss;
	if (opt.status) {
		std::stringstream status;
		if (H.size()) status << " " << TC_STATUS_OUTPUT << "%"
			<< H.size() << TC.CLEAR() << TC_STATUS;
		if (opt.severity != boost::log::trivial::info)
			status << " " << to_string(opt.severity);
		if (status.tellp()) ss << TC_STATUS << "["
			<< status.str() << " ]" << TC.CLEAR() << " ";
	}
	ss << (error ? TC_ERROR : TC_PROMPT) << "tau>" << TC.CLEAR() << " ";
	if (r) r->set_prompt(ss.str());
	return ss.str();
}

template <typename... BAs>
requires BAsPack<BAs...>
int repl_evaluator<BAs...>::eval(const std::string& src) {
	error = false;
	auto tau_spec = tt(make_cli(src));
	int quit = 0;
	if (tau_spec) {
		auto commands = tau_spec || tau::cli_command;
		for (const auto& cmd : commands())
			if (quit = eval_cmd(cmd); quit == 1) break;
	} else if (!error) return 2;
	std::cout << "\n", std::cout.flush();
	if (error && opt.error_quits) return quit = 1;
	if (quit == 0) prompt();
	return quit;
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::version_cmd() {
	std::cout << full_version << "\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::help_cmd(const tt& n) const {
	auto arg = n | tau::help_arg | tt::only_child;
	size_t nt = arg ? (arg | tt::nt) : tau::help_sym;
	help(nt);
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::help(size_t nt) const {
	static const std::string bool_options =
		"  <option>               <description>                        <value>\n"
#ifdef DEBUG
		"  debug-repl             show REPL commands             on/off\n"
#endif // DEBUG
		"  status                 show status                          on/off\n"
		"  colors                 use term colors                      on/off\n"
		"  highlighting           syntax highlighting of Tau formulas  on/off\n"
		"  indenting              indenting of Tau formulas            on/off\n";
	static const std::string all_available_options = std::string{} +
		"Available options and values:\n" + bool_options +
		"  severity               severity                             error/info/debug/trace\n";
	static const std::string bool_available_options = std::string{} +
		"Available options and values:\n" + bool_options;
	switch (nt) {
	case tau::help_sym: std::cout
		<< "General commands:\n"
		<< "  help or h               print overview of available commands in Tau repl\n"
		<< "  quit or q               exit the Tau repl\n"
		<< "  version or v            print current version\n"
		<< "  clear                   clear the screen\n"
		<< "\n"

		<< "Run command:\n"
		<< "  run                     execute a Tau formula as a program\n"
		<< "\n"

		<< "Logical procedures:\n"
		<< "  normalize or n          normalize a Tau expression\n"
		<< "  qelim                   eliminate non-temporal quantifiers in a Tau formula\n"
		<< "  sat                     check if a Tau formula is satisfiable\n"
		<< "  unsat                   check if a Tau formula is unsatisfiable\n"
		<< "  valid                   check if a Tau formula is valid\n"
		<< "  solve                   compute a satisfying assignment for the free variables in a Tau formula\n"
		<< "  lgrs                    compute a LGRS for a given equation\n"
		<< "\n"

		<< "Normal form commands:\n"
		<< "  cnf                     convert a Tau expression to conjunctive normal form\n"
		<< "  dnf                     convert a Tau expression to disjunctive normal form\n"
		<< "  mnf                     convert a Tau expression to minterm normal form\n"
		// << "  anf                     convert to algebraic normal form\n"
		<< "  nnf                     convert a Tau expression to negation normal form\n"
		// << "  pnf                     convert to prenex normal form\n"
		<< "  onf                     convert a Tau formula to order normal form\n"
		<< "\n"

		<< "History and definitions:\n"
		<< "  history or hist         show all Tau expressions stored in the repl history\n"
		<< "  definitions or defs     show stored IO variables and function and predicate definitions\n"
		<< "\n"

		<< "Substitution and instantiation command:\n"
		<< "  substitute, subst or s  substitute a Tau expression in a Tau expression by another\n"
		<< "  instantiate, inst or i  instantiate a variable in a Tau expression with a Tau term\n"
		<< "\n"

		<< "Settings commands:\n"
		<< "  get                     show option values\n"
		<< "  set                     set option's value\n"
		<< "  enable                  enable option's value\n"
		<< "  disable                 disable option's value\n"
		<< "  toggle                  toggle option's value\n"
		<< "\n"

		<< "Examples:\n"
		<< "  help or h examples      show examples related to the Tau language syntax\n"
		<< "\n"

		<< "Type \'help <command>\' for more information about a specific command\n";
		break;
	case tau::version_sym: std::cout
		<< "version prints the current Tau version\n";
		break;
	case tau::quit_sym: std::cout
		<< "quit exits the Tau repl\n";
		break;
	case tau::clear_sym: std::cout
		<< "clear clears the terminal screen\n";
		break;
	case tau::get_sym: std::cout
		<< "get                       prints all options and their values\n"
		<< "get <option>              prints the value of the given option\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau::set_sym: std::cout
		<< "set <option> [=] <value> sets option to value\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau::enable_sym: std::cout
		<< "enable <option>           enables option\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau::disable_sym: std::cout
		<< "disable <option>          disables option\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau::toggle_sym: std::cout
		<< "toggle <option>           toggles option value\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau::history_sym: std::cout
		<< "the history command shows all stored Tau expressions in the repl history\n"
		<< "\n"
		<< "  history or hist                 shows all stored Tau expressions\n"
		<< "  history or hist <repl_history>  shows the Tau expression stored at the specified repl history position\n"
		<< "  <rr>, <tau> or <term>           stores a Tau expression in the repl history at the latest position\n"
		<< "\n"
		<< "the repl history can be accessed in the following ways\n\n"
		<< "  %                       to retrieve the Tau expression stored at the latest position\n"
		<< "  %-<number>              to retrieve the Tau expression stored at the latest position minus <number>\n"
		<< "  %<number>               to retrieve the Tau expression stored at position <number>\n\n"
		<< "stored Tau expressions can be used wherever a command expects a Tau expression\n";
		break;
	case tau::normalize_sym: std::cout
		<< "the normalize command normalizes a Tau expression, prints the result and\n"
		<< "saves it into the repl history\n"
		<< "\n"
		<< "usage:\n"
		<< "  normalize <rr>            normalizes the given tau formula with additional predicate and function definitions\n"
		<< "  normalize <tau>           normalizes the given tau formula\n"
		<< "  normalize <term>          normalizes the given term\n"
		<< "  normalize <repl_history>  normalizes the Tau expression stored at the specified repl history position\n";
		break;
	case tau::qelim_sym: std::cout
		<< "the qelim command eliminates all non-temporal quantifiers, prints the result and\n"
		<< "saves it into the repl history\n"
		<< "\n"
		<< "usage:\n"
		<< "  qelim <tau>             eliminates non-temporal quantifiers in the given tau formula\n"
		<< "  qelim <repl_history>    eliminates non-temporal quantifiers in the Tau formula stored at the specified repl history position\n";
		break;
	case tau::run_sym: std::cout
		<< "the run command executes a Tau formula as a program\n"
		<< "\n"
		<< "run automatically chooses a single program from the set of programs satisfying a given Tau formula\n"
		<< "and executes it in such a way that in each step (starting at 0) each input variable reads a value from the specified stream\n"
		<< "and then a value is written into each specified output stream such that the Tau formula being executed is satisfied\n\n"
		<< "usage:\n"
		<< "  run <tau>               execute the given Tau formula\n"
		<< "  run <repl_history>      execute the Tau formula stored at the specified repl history position\n"
		<< "\n";
		break;
	case tau::solve_sym: std::cout
		<< "the solve command computes a single satisfying assignment for the free variables in a Tau formula\n"
		<< "\n"
		<< "usage:\n"
		<< "  solve [options] <tau>           computes a single satisfying assignment for the free variables in the Tau formula\n"
		<< "  solve [options] <repl_history>  computes a single satisfying assignment for the free variables in the Tau formula stored at the specified repl history position"
		<< "\n"
		<< "options:\n"
		<< "  --min, --minimum                   computes a minimum solution of the system of equations\n"
		<< "  --max, --maximum                   computes a maximum solution of the system of equations\n"
		<< "  --<type>                           uses the specified type for the solution (sbf or tau)\n"
		<< "\n";
		break;
	case tau::lgrs_sym: std::cout
		<< "the lgrs command computes a LGRS for an equation\n"
		<< "\n"
		<< "usage:\n"
		<< "  lgrs <tau_eq>           computes a LGRS for a given equation\n"
		<< "  lgrs <repl_history>     computes a LGRS for the equation stored at the specified repl history position\n"
		<< "options:\n"
		<< "  --<type>                uses the specified type for the solution\n"
		<< "\n";
		break;
	case tau::sat_sym: std::cout
		<< "the sat command checks if a Tau formula is satisfiable and if so prints T and else F\n\n"
		<< "a tau formula is satisfiable if there exists a variable assignment to non-temporal variables\n"
		<< "such that for all possible inputs there exist time compatible outputs at each point in time\n"
		<< "\n"
		<< "usage:\n"
		<< "  sat <rr>                checks the given tau formula with additional predicate and function definitions for satisfiability\n"
		<< "  sat <tau>               checks the given tau formula for satisfiability\n"
		<< "  sat <repl_history>      checks the Tau formula stored at the specified repl history position for satisfiability\n";
		break;
	case tau::valid_sym: std::cout
		<< "the valid command checks if a Tau formula is logically equivalent to T and if so prints T and else F\n"
		<< "\n"
		<< "usage:\n"
		<< "  valid <rr>              checks the given tau formula with additional predicate and function definitions for validity\n"
		<< "  valid <tau>             checks the given tau formula for validity\n"
		<< "  valid <repl_history>    checks the Tau formula stored at the specified repl history position for validity\n";
		break;
		break;
	case tau::unsat_sym: std::cout
		<< "the unsat command checks if a Tau formula is unsatisfiable and if so prints T and else F\n\n"
		<< "a tau formula is unsatisfiable if for every variable assignment to non-temporal variables\n"
		<< "there exist inputs such that there are no time compatible outputs at some point in time\n"
		<< "\n"
		<< "usage:\n"
		<< "  unsat <rr>              checks the given tau formula with additional predicate and function definitions for unsatisfiability\n"
		<< "  unsat <tau>             checks the given tau formula for unsatisfiability\n"
		<< "  unsat <repl_history>    checks the Tau formula stored at the specified repl history position for unsatisfiability\n";
		break;
	case tau::dnf_sym: std::cout
		<< "dnf converts a Tau expression to disjunctive normal form (DNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  dnf <term>              converts the given term to DNF\n"
		<< "  dnf <tau>               converts the given Tau formula to DNF\n"
		<< "  dnf <repl_history>      converts the Tau expression stored at the specified repl history position to DNF\n";
		break;
	case tau::cnf_sym: std::cout
		<< "cnf converts a Tau expression to conjunctive normal form (CNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  cnf <term>              converts the given term to CNF\n"
		<< "  cnf <tau>               converts the given Tau formula to CNF\n"
		<< "  cnf <repl_history>      converts the Tau expression stored at the specified repl history position to CNF\n";
		break;
	//case tau::anf_sym: std::cout
	//	<< "anf command converts a boolean formula or a well formed formula to algebraic normal form\n"
	//	<< "\n"
	//	<< "usage:\n"
	//	<< "  anf <term>              converts the given term to ANF\n"
	//	<< "  anf <tau>               converts the given tau to ANF\n"
	//	<< "  anf <history>           converts the history with the given id to ANF\n";
	//	break;
	case tau::nnf_sym: std::cout
		<< "nnf converts a Tau expression to negation normal form (NNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  nnf <term>              converts the given term to NNF\n"
		<< "  nnf <tau>               converts the given tau formula to NNF\n"
		<< "  nnf <repl_history>      converts the Tau expression stored at the specified repl history position to NNF\n";
		break;
	//case tau::pnf_sym: std::cout
	//	<< "pnf command converts a boolean formula or a well formed formula to prenex normal form\n"
	//	<< "\n"
	//	<< "usage:\n"
	//	<< "  pnf <term>              converts the given term to PNF\n"
	//	<< "  pnf <tau>               converts the given term to PNF\n"
	//	<< "  pnf <history>           converts the history with the given id to PNF\n";
	//	break;
	case tau::mnf_sym: std::cout
		<< "mnf converts a Tau expression to minterm normal form (MNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  mnf <term>              converts the given term to MNF\n"
		<< "  mnf <tau>               converts the given tau formula to MNF\n"
		<< "  mnf <repl_history>      converts the Tau expression stored at the specified repl history position to MNF\n";
		break;
	case tau::onf_sym: std::cout
		<< "onf converts a tau formula to order normal form (ONF) on the specified variable\n"
		<< "\n"
		<< "usage:\n"
		<< "  onf <var> <tau>           converts the given tau formula to ONF using <var>\n"
		<< "  onf <var> <repl_history>  converts the Tau formula stored at the specified repl history position to ONF using <var>\n";
		break;
	case tau::subst_sym: std::cout
		<< "the substitute command substitutes a Tau expression in a Tau expression by another Tau expression\n"
		<< "\n"
		<< "usage:\n"
		<< "  substitute <input> '[' <match> / <replace> ']'\n"
		<< "\n"
		<< "where:\n"
		<< "  <input> is the Tau expression in which to replace\n"
		<< "  <match> is the Tau expression to be replaced in <input>\n"
		<< "  <replace> is the Tau expression that is replaced for <match>\n"
		<< "\n"
		<< "  Note that if <input> is of type term, <match> and <replace> must be of type term\n"
		<< "  In general <match> and <replace> must be of the same type, so either both term or tau\n"
		<< "\n";
		break;
	case tau::inst_sym: std::cout
		<< "the instantiate command instantiates a variable in a Tau formula with the specified term\n"
		<< "\n"
		<< "usage:\n"
		<< "  instantiate <input> '[' <variable> / <value> ']'\n"
		<< "\n"
		<< "where:\n"
		<< "  <input> is the Tau term to instantiate in\n"
		<< "  <variable> is the variable to be instantiated\n"
		<< "  <value> is the Tau term to instantiate with\n"
		<< "\n";
		break;
	case tau::def_sym: std::cout
		<< "the definitions command shows stored input/output stream variables and function and predicate definitions\n"
		<< "\n"
		<< "usage:\n"
		<< "  <tau_rec_relation>      defines a tau predicate\n"
		<< "  <term_rec_relation>     defines a tau function\n"
		<< "  <def_input_cmd>         defines an input stream variable\n"
		<< "  <def_output_cmd>        defines an output stream variable\n"
		<< "  definitions             lists all definitions present in repl\n"
		<< "  definitions <number>    prints predicate or function at specified position\n"
		<< "\n"
		<< "Examples defining stream variables and functions can be found by typing \'help examples\'\n";
		break;
	case tau::examples_sym: std::cout
		<< "examples\n"
		<< "\n"
		<< "  # defining an input stream variable\n"
		<< "  sbf i1 = console\n"
		<< "  tau i2 = ifile(\"inputs.in\")\n"
		<< "\n"
		<< "  # defining an output stream variable\n"
		<< "  sbf o1 = console\n"
		<< "  tau o2 = ofile(\"outputs.out\")\n"
		<< "\n"
		<< "  # defining functions\n"
		<< "  (Tau term function)    rr1(x,y,z) := (x & y) | z\n"
		<< "  (Tau formula function) rr2(x,y,z) := ( x = 0 || y = 0) && z = 0\n\n"
		<< "\n";
		break;
	}
}

// undef terminal color macros valid only for this file
#undef TC_STATUS
#undef TC_STATUS_OUTPUT
#undef TC_ERROR
#undef TC_PROMPT
#undef TC_OUTPUT

} // namespace idni::tau_lang
