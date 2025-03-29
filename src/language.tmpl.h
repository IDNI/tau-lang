// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <optional>

#include "nso_rr.h"
#include "queries.h"

namespace idni::tau_lang {


template <typename... BAs>
tau_depreciating<BAs...> trim(const tau_depreciating<BAs...>& n) {
	return n->child[0];
}

template <typename... BAs>
tau_depreciating<BAs...> trim2(const tau_depreciating<BAs...>& n) {
	return n->child[0]->child[0];
}

template <typename... BAs>
tau_depreciating<BAs...> wrap(tau_parser::nonterminal nt, const std::vector<tau_depreciating<BAs...>>& nn)
{
	return make_node<tau_sym<BAs...>>(
		tau_parser::instance().literal(nt), nn);
}

template <typename... BAs>
tau_depreciating<BAs...> wrap(tau_parser::nonterminal nt,
	const std::initializer_list<tau_depreciating<BAs...>> ch)
{
	return wrap(nt, std::vector<tau_depreciating<BAs...>>(ch));
}

template <typename... BAs>
tau_depreciating<BAs...> wrap(tau_parser::nonterminal nt, const tau_depreciating<BAs...>& n) {
	return wrap(nt, { n });
}

template <typename... BAs>
tau_depreciating<BAs...> wrap(tau_parser::nonterminal nt, const tau_depreciating<BAs...>& c1,
	const tau_depreciating<BAs...>& c2)
{
	return wrap(nt, { c1, c2 });
}

template <typename... BAs>
tau_depreciating<BAs...> wrap(tau_parser::nonterminal nt, const std::string& terminals) {
	std::vector<tau_depreciating<BAs...>> children;
	for (const auto& c : terminals)
		children.emplace_back(rewriter::make_node<tau_sym<BAs...>>(
			tau_source_sym(c), {}));
	return wrap(nt, children);
}

//
// functions traversing the nodes according to the specified non terminals and
// extracting the required information from them
//
//

// returns the extracted component/information of the specified node if possible,
// the component/information is extracted using the given extractor.
template <size_t... nt, typename extractor_t, typename... BAs>
auto get_optional(const extractor_t& extractor, const tau_depreciating<BAs...>& n) {
	return get_node<nt..., BAs...>(n).and_then(extractor);
}

// returns the extracted components/informations of the specified nodes, the
// component/information is extracted using the given extractor.
template <size_t... nt, typename extractor_t, typename... BAs>
auto get_optionals(const extractor_t& extractor, const tau_depreciating<BAs...>& n) {
	return get_nodes<nt..., BAs...>(n)
		| std::ranges::views::transform(extractor)
		| std::ranges::views::filter(
				&std::optional<tau_sym<BAs...>>::has_value);
}

// converts a tau_source_sym to a tau_sym, it is used to convert from
// tau_source to tau_node...
template <typename... BAs>
struct tauify {

	tau_sym<BAs...> operator()(const tau_source_sym& n) const {
		// TODO this is simple retype of bf_and_nosep_1st_op to bf
		// this is temporary until parser features retyping
		if (n.nt()) switch (n.n()) {
		case tau_parser::bf_and_nosep_1st_oprnd:
		case tau_parser::bf_and_nosep_2nd_oprnd:
		case tau_parser::bf_neg_oprnd:
			return tau_sym<BAs...>(tau_parser::instance()
						.literal(tau_parser::bf));
		case tau_parser::bf_and_nosep:
			return tau_sym<BAs...>(tau_parser::instance()
						.literal(tau_parser::bf_and));
		}
		return tau_sym<BAs...>(n);
	}
};

// extracts terminal from sp_tau_source_node
extern std::function<std::optional<char>(const sp_tau_source_node& n)>
	tau_source_terminal_extractor;

template <typename... BAs>
using tau_source_terminal_extractor_t = decltype(tau_source_terminal_extractor);

// bind the given, using a binder, the constants of the a given tau<...>.
template<typename binder_t, typename... BAs>
struct bind_transformer {

	bind_transformer(binder_t& binder) : binder(binder) {}

	tau_depreciating<BAs...> operator()(const tau_depreciating<BAs...>& n) {
		if (error) return nullptr;
		if (auto it = changes.find(n); it != changes.end())
			return it->second;
		if (is_non_terminal<tau_parser::bf_constant, BAs...>(n)) {
			auto nb = binder.bind(n);
			if (binder.error) return error = true, nullptr;
			return changes[n] = nb;
		}
		// IDEA maybe we could use the replace transform instead of having the following code
		bool changed = false;
		std::vector<tau_depreciating<BAs...>> child;
		for (auto& c : n->child)
			if (changes.contains(c))
				changed = true, child.push_back(changes[c]);
			else child.push_back(c);
		auto nn = make_node<tau_sym<BAs...>>(n->value, child);
		if (changed) changes[n] = nn;
		return nn;
	}

	std::map<tau_depreciating<BAs...>, tau_depreciating<BAs...>> changes;
	binder_t binder;
	bool error = false;
};

// is not a whitespace predicate
template <typename... BAs>
static const auto not_whitespace_predicate = [](const tau_depreciating<BAs...>& n) {
	return n->value.index() != 0
		|| !get<0>(n->value).nt()
		|| (get<0>(n->value).n() != tau_parser::_ &&
			get<0>(n->value).n() != tau_parser::__);
};

template <typename... BAs>
using not_whitespace_predicate_t = decltype(not_whitespace_predicate<BAs...>);

// binds the constants of a given binding using the label specified
// in the code and according to a map from labels to constants in the BAs...
template <typename... BAs>
struct name_binder {

	name_binder(const bindings<BAs...>& bs) : bs(bs) {}

	tau_depreciating<BAs...> bind(const tau_depreciating<BAs...>& n) {
		auto binding = n | tau_parser::constant | tau_parser::binding;
		if (!binding || (n | tau_parser::type).has_value()) return n;
		auto bn = make_string<
				tau_node_terminal_extractor_t<BAs...>,
				tau_depreciating<BAs...>>(
			tau_node_terminal_extractor<BAs...>, binding.value());
		if (auto s = bs.find(bn); s != bs.end()) {
			tau_sym<BAs...> ts = s->second;
			return wrap(tau_parser::bf_constant,
				wrap(tau_parser::constant,
					rewriter::make_node<tau_sym<BAs...>>(
								ts, {})));
		}
		return error = true, n;
	}

	bool error = false;
	const bindings<BAs...>& bs;
};

// binds the constants of a given binding using the multi-factory for the types
// supported.
template <typename... BAs>
struct factory_binder {

	tau_depreciating<BAs...> bind(const tau_depreciating<BAs...>& n) {
		if (error) return nullptr;
		auto binding = n | tau_parser::constant | tau_parser::binding;
		if (!binding) return n; // not a binding (capture?)
		auto type = find_top(n, is_non_terminal<tau_parser::type, BAs...>);
		if (type) {
			// the factory take two arguments, the first is the type and the
			// second is the node representing the constant.
			std::string type_name = make_string<
				tau_node_terminal_extractor_t<BAs...>,
				tau_depreciating<BAs...>>(
					tau_node_terminal_extractor<BAs...>,
					type.value());
			auto nn = nso_factory<BAs...>::instance().binding(binding.value(), type_name);
			if (!nn) return error = true, nullptr;
			if (nn != binding.value())
				return wrap(tau_parser::bf_constant,
					wrap(tau_parser::constant, nn), type.value());
			return n;
		}
		auto nn = nso_factory<BAs...>::instance().binding(binding.value(), "");
		if (!nn) return error = true, nullptr;
		return wrap(tau_parser::bf_constant,
			wrap(tau_parser::constant, nn));
	}

	bool error = false;
};

// creates a specific rule from a generic rule
// TODO (LOW) should depend in node_t instead of BAs...
template <typename... BAs>
rewriter::rule<tau_depreciating<BAs...>> make_rule(tau_parser::nonterminal rule_t,
	tau_parser::nonterminal matcher_t, tau_parser::nonterminal body_t,
	const tau_depreciating<BAs...>& rule)
{
	auto matcher = rule | rule_t | matcher_t | only_child_extractor<BAs...>
		| optional_value_extractor<tau_depreciating<BAs...>>;
	auto body = rule | rule_t | body_t | only_child_extractor<BAs...>
		| optional_value_extractor<tau_depreciating<BAs...>>;
	return { matcher, body };
}

// creates a specific rule from a generic rule
// TODO (LOW) should depend in node_t instead of BAs...
template <typename... BAs>
rewriter::rule<tau_depreciating<BAs...>> make_rule(const tau_depreciating<BAs...>& rule) {
	auto type = only_child_extractor<BAs...>(rule)
		| non_terminal_extractor<BAs...>
		| optional_value_extractor<size_t>;
	switch (type) {
	case tau_parser::bf_rule: return make_rule<BAs...>(tau_parser::bf_rule,
		tau_parser::bf_matcher, tau_parser::bf_body, rule); break;
	case tau_parser::wff_rule:return make_rule<BAs...>(tau_parser::wff_rule,
		tau_parser::wff_matcher, tau_parser::wff_body, rule); break;
	default: assert(false); return {};
	};
}

// create a set of rules from a given tau source.
// TODO (LOW) should depend in node_t instead of BAs...
template <typename... BAs>
rules<tau_depreciating<BAs...>> make_rules(tau_depreciating<BAs...>& tau_source) {
	rules<tau_depreciating<BAs...>> rs;
	// TODO (LOW) change call to select by operator|| and operator|
	for (auto& r: select_top(tau_source,
		is_non_terminal<tau_parser::rule, BAs...>))
			rs.push_back(make_rule<BAs...>(r));
	return rs;
}

// create a set of relations from a given tau source.
// TODO (LOW) should depend in node_t instead of BAs...
template <typename... BAs>
rec_relations<tau_depreciating<BAs...>> make_rec_relations(
	const tau_depreciating<BAs...>& tau_source)
{
	rec_relations<tau_depreciating<BAs...>> rs;
	// TODO (LOW) change call to select by operator|| and operator|
	for (auto& r: select_top(tau_source,
		is_non_terminal<tau_parser::rec_relation, BAs...>))
			rs.emplace_back(r->child[0], r->child[1]);
	return rs;
}

// make a tau source from the given source code string.
sp_tau_source_node make_tau_source(const std::string& source,
	idni::parser<>::parse_options options);

// make a tau source from the given source code stream.
sp_tau_source_node make_tau_source(std::istream& is,
	idni::parser<>::parse_options options);

// make a tau source from the given source code stream.
sp_tau_source_node make_tau_source_from_file(const std::string& filename,
	idni::parser<>::parse_options options);

template <typename...BAs>
tau_depreciating<BAs...> process_digits(const tau_depreciating<BAs...>& tau_source) {
	std::map<tau_depreciating<BAs...>, tau_depreciating<BAs...>> changes;
	for(auto& n: select_top(tau_source,
		is_non_terminal<tau_parser::digits, BAs...>))
	{
		auto offset = make_string<
				tau_node_terminal_extractor_t<BAs...>,
				tau_depreciating<BAs...>>(
			tau_node_terminal_extractor<BAs...>,  n);
		auto num = std::stoul(offset);
		auto nn = rewriter::make_node<tau_sym<BAs...>>(tau_sym<BAs...>(num), {});
		changes[n] = nn;
	}
	return replace<tau_depreciating<BAs...>>(tau_source, changes);
}

// } // include for ptree<BAs...>()
// #include "debug_helpers.h"
// namespace idni::tau_lang {

template <typename... BAs>
struct quantifier_vars_transformer {
	using p = tau_parser;
	using node = tau_depreciating<BAs...>;
	static constexpr std::array<p::nonterminal, 2>
					quant_nts = { p::wff_ex, p::wff_all };
	node operator()(const node& n) {
		if (auto it = changes.find(n); it != changes.end())
			return it->second;
		// if the node is any of quant_nts (all, ex, ball, bex)
		for (size_t q = 0; q != 2; ++q)
			if (is_non_terminal<BAs...>(quant_nts[q], n))
		{
			// ptree<BAs...>(std::cout << "QUANT: ", n) << "\n";
			// use nonterminals according to quantifier's q
			const p::nonterminal& quant_nt = quant_nts[q];
			// travers into q_vars node and access all var/capture children
			auto q_vars = n | p::q_vars
				| optional_value_extractor<node>;
			const std::vector<node>& vars = q_vars->child;
			//  traverse to quantifier wff expression
			node nn = n | p::wff | only_child_extractor<BAs...>
				| optional_value_extractor<node>;
			if (auto it = changes.find(nn); it != changes.end())
				nn = it->second;
			if (vars.size() == 0) continue; // no variable/capture
			// for each variable
			for (size_t vi = 0; vi != vars.size(); ++vi) {
				// get var (in a reverse order of iteration)
				auto& var = vars[vars.size() - 1 - vi];
				// new expression node
				auto expr = wrap(p::wff, nn);
				// create a new quantifier node with var and new children
				nn = wrap(quant_nt, { var, expr });
			}
			changes[n] = nn;
			return nn;
		}

		bool changed = false;
		std::vector<tau_depreciating<BAs...>> child;
		for (const node& c : n->child)
			if (changes.contains(c)) changed = true,
						child.push_back(changes[c]);
			else child.push_back(c);
		node nn = make_node<tau_sym<BAs...>>(n->value, child);
		if (changed) changes[n] = nn;
		return nn;
	}
	std::map<node, node> changes;
};

template <typename...BAs>
tau_depreciating<BAs...> process_quantifier_vars(const tau_depreciating<BAs...>& tau_code) {
	using node = tau_depreciating<BAs...>;
	quantifier_vars_transformer<BAs...> transformer;
	return rewriter::post_order_traverser<quantifier_vars_transformer<BAs...>,
		rewriter::all_t, node>(transformer, rewriter::all)(tau_code);
}

template <typename...BAs>
tau_depreciating<BAs...> process_offset_variables(const tau_depreciating<BAs...>& tau_code) {
	using p = tau_parser;
	using node = tau_depreciating<BAs...>;
	std::map<node, node> changes;
	for (const auto& offsets :
		select_all(tau_code, is_non_terminal<p::offsets, BAs...>))
	{
		for (const auto& var : select_all(offsets,
			is_non_terminal<p::variable, BAs...>))
				changes[var] = wrap(p::capture, var->child);
	}
	if (changes.size()) return replace(tau_code, changes);
	return tau_code;
}

template <typename...BAs>
tau_depreciating<BAs...> process_defs_input_variables(const tau_depreciating<BAs...>& tau_code) {
	using p = tau_parser;
	using node = tau_depreciating<BAs...>;
	std::map<node, node> changes;
	for (const auto& def :
		select_all(tau_code, is_non_terminal<p::rec_relation, BAs...>))
	{
		for (const auto& ref_arg : select_all(def->child[0],
			is_non_terminal<p::ref_arg, BAs...>))
				for (const auto& var : select_all(ref_arg,
					is_non_terminal<p::variable, BAs...>))
		{
			changes[var] = wrap(p::capture, var->child);
		}
	}
	if (changes.size()) return replace(tau_code, changes);
	return tau_code;
}

template <typename... BAs>
tau_depreciating<BAs...> infer_constant_types(const tau_depreciating<BAs...>& code) {
	BOOST_LOG_TRIVIAL(trace)
		<< "(T) infer constant types: " << code;
	using node = tau_depreciating<BAs...>;
	std::map<node, node> changes;
	auto extract_type = [](const node& n) {
		return make_string<tau_node_terminal_extractor_t<BAs...>,
			node>(tau_node_terminal_extractor<BAs...>, n);
	};
	auto type_mismatch = [&](const std::string& expected,
		const std::string& got)
	{
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) Type mismatch. Constant is expected to be "
			<< expected << " but is " << got;
		return false;
	};
	auto unsupported_type = [&](const std::string& type)
	{
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) Unsupported type: " << type << "\n";
		return false;
	};
	auto infer_over_scope = [&](const node& nd) {
		std::string type = "";
		BOOST_LOG_TRIVIAL(trace)
			<< "(T) infer over scope: " << nd;
		// get first appearing type information in an expression
		auto bfcs = select_all(nd,
			is_non_terminal<tau_parser::bf_constant, BAs...>);
		if (bfcs.size() == 0) return true;
		for (auto& bfc : bfcs) {
			auto type_opt = bfc | tau_parser::type;
			if (type_opt) {
				type = extract_type(type_opt.value());
				break;
			}
		}
		BOOST_LOG_TRIVIAL(trace)
			<< "(T) first appearing constant type: " << type;
		if (type.size() == 0) type = nso_factory<BAs...>::instance().default_type(); // default type
		// pass the type to all constants and check for mismatch
		for (auto& c : select_all(nd, is_non_terminal<
				tau_parser::bf_constant, BAs...>))
		{
			auto type_nd = c | tau_parser::type;
			if (type_nd) {
				auto got = extract_type(type_nd.value());
				bool found = false;
				for (auto t: nso_factory<BAs...>::instance().types())
					if (got == t) { found = true; break; }
				if (!found) return unsupported_type(got);
				if (!(type.size() == 0 && got == nso_factory<BAs...>::instance().default_type())
					&& got != type)
				{
					BOOST_LOG_TRIVIAL(trace)
						<< "(T) Type mismatch: " << type << " got: " << got << " for: " << c;
					return type_mismatch(got,type);
				}
			} else changes.emplace(c, rewriter::make_node<
				tau_sym<BAs...>>(c->value, {c->child[0],
					wrap<BAs...>(tau_parser::type,
							type)}));
		}
		return true;
	};
	static auto is_constant_type_scope_node = [](const node& n) {
		static std::vector<size_t> scope_nodes{
			tau_parser::bf_interval,
			tau_parser::bf_eq,
			tau_parser::bf_neq,
			tau_parser::bf_less_equal,
			tau_parser::bf_nleq,
			tau_parser::bf_greater,
			tau_parser::bf_ngreater,
			tau_parser::bf_greater_equal,
			tau_parser::bf_ngeq,
			tau_parser::bf_less,
			tau_parser::bf_nless,
			tau_parser::bf
		};
		if (!is_non_terminal_node<BAs...>(n)) return false;
		auto nt = n | non_terminal_extractor<BAs...>
			| optional_value_extractor<size_t>;
		return find(scope_nodes.begin(), scope_nodes.end(), nt)
							!= scope_nodes.end();
	};
	for (auto& nd : select_top(code, is_constant_type_scope_node))
		if (!infer_over_scope(nd)) return 0;
	return replace<node>(code, changes);
}

// TODO (LOW) refactor and clean this structure
template <typename... BAs>
struct free_vars_collector {

	free_vars_collector(std::set<tau_depreciating<BAs...>>& free_vars) : free_vars(free_vars) {}

	tau_depreciating<BAs...> operator()(const tau_depreciating<BAs...>& n) {
		if (is_quantifier<BAs...>(n)) {
			// IDEA using quantified_variable => variable | capture would simplify the code
			auto var = find_top(n, is_var_or_capture<BAs...>);
			if (var.has_value()) {
				if (auto it = free_vars.find(var.value()); it != free_vars.end()) {
					free_vars.erase(it);
					BOOST_LOG_TRIVIAL(trace) << "(I) -- removing quantified var: " << var.value();
				}
			}
		}
		if (is_var_or_capture<BAs...>(n)) {
			if (auto offset_child = n
					| tau_parser::io_var | only_child_extractor<BAs...> | tau_parser::offset
					| only_child_extractor<BAs...>; offset_child) {
				if (is_var_or_capture<BAs...>(offset_child.value())) {
					auto var = offset_child.value();
					if (auto it = free_vars.find(var); it != free_vars.end()) {
						free_vars.erase(it);
						BOOST_LOG_TRIVIAL(trace) << "(I) -- removing var: " << var;
					}
				} else if (is_non_terminal(tau_parser::shift, offset_child.value())) {
					// shift variable in io_var
					auto var = trim(offset_child.value());
					assert(is_var_or_capture<BAs...>(var));
					if (auto it = free_vars.find(var); it != free_vars.end()) {
						free_vars.erase(it);
						BOOST_LOG_TRIVIAL(trace) << "(I) -- removing var: " << var;
					}
				}
			}
			free_vars.insert(n);
			BOOST_LOG_TRIVIAL(trace) << "(I) -- inserting var: " << n;
		}
		return n;
	}

	std::set<tau_depreciating<BAs...>>& free_vars;
};

template <typename... BAs>
auto get_free_vars_from_nso(const tau_depreciating<BAs...>& n) {
	BOOST_LOG_TRIVIAL(trace) << "(I) -- Begin get_free_vars_from_nso of " << n;
	std::set<tau_depreciating<BAs...>> free_vars;
	free_vars_collector<BAs...> collector(free_vars);
	rewriter::post_order_traverser<
			free_vars_collector<BAs...>,
			rewriter::all_t,
			tau_depreciating<BAs...>>(collector, rewriter::all)(n);
	BOOST_LOG_TRIVIAL(trace) << "(I) -- End get_free_vars_from_nso";
	return free_vars;
}

// TODO (MEDIUM) unify this code with get_tau_nso_clause and get_tau_nso_literals
template<typename ...BAs>
void get_leaves(const tau_depreciating<BAs...>& n, tau_parser::nonterminal branch,
	std::vector<tau_depreciating<BAs...>>& leaves)
{
	auto add_leave = [&branch, &leaves](const auto& n) {
		if (is_non_terminal(branch, n)) return true;
		if (is_child_non_terminal(branch, n)) return true;
		return leaves.push_back(n), false;
	};
	rewriter::pre_order(n).visit(add_leave);
}

template<typename ...BAs>
std::vector<tau_depreciating<BAs...>> get_leaves(const tau_depreciating<BAs...>& n,
	tau_parser::nonterminal branch)
{
	std::vector<tau_depreciating<BAs...>> leaves;
	get_leaves(n, branch, leaves);
	return leaves;
}

template<typename ...BAs>
std::vector<tau_depreciating<BAs...>> get_dnf_wff_clauses(const tau_depreciating<BAs...>& n) {
	return get_leaves(n, tau_parser::wff_or);
}

template<typename ...BAs>
std::vector<tau_depreciating<BAs...>> get_dnf_bf_clauses(const tau_depreciating<BAs...>& n) {
	return get_leaves(n, tau_parser::bf_or);
}

template<typename ...BAs>
std::vector<tau_depreciating<BAs...>> get_cnf_wff_clauses(const tau_depreciating<BAs...>& n) {
	return get_leaves(n, tau_parser::wff_and);
}

template<typename ...BAs>
std::vector<tau_depreciating<BAs...>> get_cnf_bf_clauses(const tau_depreciating<BAs...>& n) {
	return get_leaves(n, tau_parser::bf_and);
}

// A formula has a temporal variable if either it contains an io_var with a variable or capture
// or it contains a flag
template <typename... BAs>
bool has_temp_var (const tau_depreciating<BAs...>& fm) {
	auto io_vars = select_top(fm, is_non_terminal<tau_parser::io_var, BAs...>);
	if (io_vars.empty()) return find_top(fm, is_non_terminal<tau_parser::constraint, BAs...>).has_value();
	// any input/output stream is a temporal variable, also constant positions
	else return true;
}

// Check that no non-temporal quantified variable appears nested in the scope of
// temporal quantification
template <typename... BAs>
bool invalid_nesting_of_quants (const tau_depreciating<BAs...>& fm) {
	auto non_temp_quants = select_all(fm, is_quantifier<BAs...>);
	for (const auto& ntq : non_temp_quants) {
		auto ntq_var = trim(ntq);
		auto temp_quants = select_all(ntq, is_temporal_quantifier<BAs...>);
		for (const auto& tq : temp_quants) {
			// Check that the non-temp quantified variable doesn't appear free
			if (get_free_vars_from_nso(tq).contains(ntq_var)) {
				BOOST_LOG_TRIVIAL(error) << "(Error) Non-temporal quantifier cannot capture variable appearing in scoped temporal subformula: \n"
				<< "Variable \"" << ntq_var << "\" is captured in \"" << tq << "\"";
				return true;
			}
		}
	}
	return false;
}

template <typename... BAs>
bool invalid_nesting_of_temp_quants (const tau_depreciating<BAs...>& fm) {
	auto temp_statements = select_top(fm, is_temporal_quantifier<BAs...>);
	// Check that in no temp_statement another temporal statement is found
	for (const auto& temp_st : temp_statements) {
		if(auto n = find_top(trim(temp_st), is_temporal_quantifier<BAs...>); n.has_value()) {
			BOOST_LOG_TRIVIAL(error) << "(Error) Nesting of temporal quantifiers is currently not allowed: \n"
			<< "Found \"" << n.value() << "\" in \"" << temp_st << "\"";
			return true;
		}
	}
	return false;
}

template <typename... BAs>
bool has_open_tau_fm_in_constant (const tau_depreciating<BAs...>& fm) {
	auto _closed = [](const auto& n) -> bool {
		return is_closed(n);
	};
	auto consts = select_top(fm, is_child_non_terminal<tau_parser::bf_constant, BAs...>);
	for (const auto& c : consts) {
		auto ba_const = c
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		if (!std::visit(_closed, ba_const)) {
			BOOST_LOG_TRIVIAL(error) << "(Error) A Tau formula constant must be closed: " << ba_const;
			return true;
		}
	}
	return false;
}

// Check that no recurrence relation offset is negative
template <typename... BAs>
bool has_negative_offset(const tau_depreciating<BAs...>& fm) {
	using p = tau_parser;
	auto terms = [](const auto& n) {
		return is_non_terminal(p::ref, n) ||
			is_non_terminal(p::bf_ref, n) ||
				is_non_terminal(p::wff_ref, n);
	};
	for (const auto& ref : select_top(fm, terms)) {
		auto offsets = ref | p::offsets;
		if (!offsets.has_value()) continue;
		for (const auto& i : select_top(offsets.value(), is_non_terminal<p::integer, BAs...>)) {
			// Check that each integer is positive
			if (int_extractor<BAs...>(i) < 0) {
				BOOST_LOG_TRIVIAL(error) << "(Error) Index in recurrence relation is negative " << ref;
				return true;
			}
		}
	}
	return false;
}

// This function is used to check for semantic errors in formulas since those
// cannot be captured by the grammar
template <typename... BAs>
bool has_semantic_error (const tau_depreciating<BAs...>& fm) {
	bool error = invalid_nesting_of_quants(fm)
			|| has_open_tau_fm_in_constant(fm)
			|| invalid_nesting_of_temp_quants(fm)
			|| has_negative_offset(fm);
	return error;
}

// create tau code from tau source
template <typename... BAs>
// TODO (LOW) should depend on node_t instead of BAs...
tau_depreciating<BAs...> make_tau_code(sp_tau_source_node& tau_source) {
	if (!tau_source) return 0;
	tauify<BAs...> tf;
	rewriter::map_transformer<tauify<BAs...>,
		sp_tau_source_node, tau_depreciating<BAs...>> transform(tf);
	auto tau_code = rewriter::post_order_traverser<
				rewriter::map_transformer<tauify<BAs...>,
				sp_tau_source_node, tau_depreciating<BAs...>>,
			rewriter::all_t,
			rewriter::sp_node<tau_source_sym>,
			tau_depreciating<BAs...>>(
		transform, rewriter::all)(tau_source);
	if (!tau_code) return nullptr;
	return infer_constant_types(          // transforms ref to bf_ref/wff_ref
		process_defs_input_variables( // transforms input variables to captures
		process_offset_variables(     // transforms offset variables to captures
		process_quantifier_vars(      // transforms ex x,y to ex x ex y
		process_digits(tau_code)))));
}

// make a library from the given tau source.
// TODO (LOW) should depend on node_t instead of BAs...
template <typename... BAs>
library<tau_depreciating<BAs...>> make_library(sp_tau_source_node& tau_source) {
	auto lib = make_tau_code<BAs...>(tau_source);
	return make_rules(lib);
}

// make a library from the given tau source string.
// TODO (LOW) should depend on node_t instead of BAs...
template <typename... BAs>
library<tau_depreciating<BAs...>> make_library(const std::string& source) {
	auto tau_source = make_tau_source(source, {
						.start = tau_parser::library });
	return make_library<BAs...>(tau_source);
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
tau_depreciating<BAs...> bind_tau_code_using_binder(const tau_depreciating<BAs...>& code,
	binder_t& binder) {
	bind_transformer<binder_t, BAs...> bs(binder);
	auto res = rewriter::post_order_traverser<
			bind_transformer<binder_t, BAs...>,
			rewriter::all_t,
			tau_depreciating<BAs...>>(bs, rewriter::all)(code);
	// Check for errors which cannot be captured by the grammar
	if (bs.error) return nullptr;
	if (has_semantic_error(res)) return {};
	else return res;
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
tau_depreciating<BAs...> bind_tau_code_using_bindings(tau_depreciating<BAs...>& code,
	const bindings<BAs...>& bindings)
{
	name_binder<BAs...> nb(bindings);
	return bind_tau_code_using_binder<
			name_binder<BAs...>, BAs...>(code, nb);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
tau_depreciating<BAs...> bind_tau_code_using_factory(const tau_depreciating<BAs...>& code)
{
	factory_binder<BAs...> fb;
	return bind_tau_code_using_binder<factory_binder<BAs...>, BAs...>(code, fb);
}

// make a nso_rr from the given tau code
template <typename... BAs>
std::optional<rr<tau_depreciating<BAs...>>> make_nso_rr_from_binded_code(
	const tau_depreciating<BAs...>& code)
{
	if (is_non_terminal(tau_parser::bf, code)
		|| is_non_terminal(tau_parser::ref, code))
			return { { {}, code } };

	if (is_non_terminal(tau_parser::rec_relations, code))
		return { { make_rec_relations<BAs...>(code), {} } };

	auto main = (is_non_terminal(tau_parser::tau_constant_source, code)
		|| is_non_terminal(tau_parser::rr, code)
			? code | tau_parser::main
			: code | tau_parser::rr | tau_parser::main)
				| tau_parser::wff
				| optional_value_extractor<tau_depreciating<BAs...>>;
	auto rules = make_rec_relations<BAs...>(code);
	return infer_ref_types<BAs...>(rr<tau_depreciating<BAs...>>{ rules, main });
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
std::optional<rr<tau_depreciating<BAs...>>> make_nso_rr_using_binder(const tau_depreciating<BAs...>& code,
	binder_t& binder)
{
	auto binded = bind_tau_code_using_binder<binder_t, BAs...>(code, binder);
	if (!binded) return {};
	return make_nso_rr_from_binded_code<BAs...>(binded);
}

template<typename binder_t, typename... BAs>
tau_depreciating<BAs...> make_nso_using_binder(const tau_depreciating<BAs...>& code,
	binder_t& binder)
{
	return bind_tau_code_using_binder<binder_t, BAs...>(code, binder);
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
std::optional<rr<tau_depreciating<BAs...>>> make_nso_rr_using_binder(
	sp_tau_source_node& source, binder_t& binder)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return {};
	return make_nso_rr_using_binder<binder_t, BAs...>(code, binder);
}

template<typename binder_t, typename... BAs>
tau_depreciating<BAs...> make_nso_using_binder(sp_tau_source_node& source,
	binder_t& binder)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return nullptr;
	return make_nso_using_binder<binder_t, BAs...>(code, binder);
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
std::optional<rr<tau_depreciating<BAs...>>> make_nso_rr_using_binder(std::string& input,
	binder_t& binder)
{
	auto source = make_tau_source(input, { .start = tau_parser::rr });
	return !source ? std::optional<rr<tau_depreciating<BAs...>>>{}
		: std::optional<rr<tau_depreciating<BAs...>>>{
			make_nso_rr_using_binder<binder_t, BAs...>(
				source, binder) };
}

template<typename binder_t, typename... BAs>
std::optional<tau_depreciating<BAs...>> make_nso_using_binder(std::string& input,
	binder_t& binder,
	idni::parser<>::parse_options options = { .start = tau_parser::wff })
{
	auto source = make_tau_source(input, options);
	return !source ? std::optional<tau_depreciating<BAs...>>{}
		: std::optional<tau_depreciating<BAs...>>{
			make_nso_using_binder<binder_t, BAs...>(
				source, binder) };
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau_depreciating<BAs...>>> make_nso_rr_using_bindings(
	const tau_depreciating<BAs...>& code, const bindings<BAs...>& bindings)
{
	name_binder<BAs...> nb(bindings);
	return make_nso_rr_using_binder<name_binder<BAs...>, BAs...>(code, nb);
}

template <typename... BAs>
tau_depreciating<BAs...> make_nso_using_bindings(const tau_depreciating<BAs...>& code,
	const bindings<BAs...>& bindings)
{
	name_binder<BAs...> nb(bindings);
	return make_nso_using_binder<name_binder<BAs...>, BAs...>(code, nb);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau_depreciating<BAs...>>> make_nso_rr_using_bindings(
	sp_tau_source_node& source, const bindings<BAs...>& bindings)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return {};
	return make_nso_rr_using_bindings<BAs...>(code, bindings);
}

template <typename... BAs>
std::optional<tau_depreciating<BAs...>> make_nso_using_bindings(sp_tau_source_node& source,
	const bindings<BAs...>& bindings)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return {};
	return make_nso_using_bindings<BAs...>(code, bindings);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau_depreciating<BAs...>>> make_nso_rr_using_bindings(
	const std::string& input, const bindings<BAs...>& bindings)
{
	auto source = make_tau_source(input, { .start = tau_parser::rr });
	if (!source) return std::optional<rr<tau_depreciating<BAs...>>>{};
	return make_nso_rr_using_bindings<BAs...>(source, bindings);
}
// make a nso from the given tau source and bindings.
template <typename... BAs>
std::optional<tau_depreciating<BAs...>> make_nso_using_bindings(
	const std::string& input, const bindings<BAs...>& bindings,
	idni::parser<>::parse_options options)
{
	auto source = make_tau_source(input, options);
	if (!source) return std::optional<tau_depreciating<BAs...>>{};
	return make_nso_using_bindings<BAs...>(source, bindings);
}

template <typename... BAs>
std::optional<rr<tau_depreciating<BAs...>>> make_nso_rr_using_factory(
	const tau_depreciating<BAs...>& code)
{
	factory_binder<BAs...> fb;
	return make_nso_rr_using_binder<
			factory_binder<BAs...>, BAs...>(code,fb);
}

template <typename... BAs>
tau_depreciating<BAs...> make_nso_using_factory(const tau_depreciating<BAs...>& code) {
	factory_binder<BAs...> fb;
	return make_nso_using_binder<
			factory_binder<BAs...>, BAs...>(code,fb);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau_depreciating<BAs...>>> make_nso_rr_using_factory(
	sp_tau_source_node& source)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return std::optional<rr<tau_depreciating<BAs...>>>{};
	return make_nso_rr_using_factory<BAs...>(code);
}

template <typename... BAs>
std::optional<tau_depreciating<BAs...>> make_nso_using_factory(
	sp_tau_source_node& source)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return std::optional<tau_depreciating<BAs...>>{};
	return make_nso_using_factory<BAs...>(code);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau_depreciating<BAs...>>> make_nso_rr_using_factory(
	const std::string& input)
{
	auto source = make_tau_source(input, { .start = tau_parser::rr });
	if (!source) return std::optional<rr<tau_depreciating<BAs...>>>{};
	return make_nso_rr_using_factory<BAs...>(source);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<tau_depreciating<BAs...>> make_nso_using_factory(
	const std::string& input,
	idni::parser<>::parse_options options)
{
	auto source = make_tau_source(input, options);
	if (!source) return std::optional<tau_depreciating<BAs...>>{};
	return make_nso_using_factory<BAs...>(source);
}

//------------------------------------------------------------------------------
// rec relations type checking and inference

// rr name-id dict
static inline std::vector<std::string> rr_v{ "dummy" };
static inline std::map<std::string, size_t> rr_m{};
inline size_t rr_dict(const std::string& s) {
	if (auto it = rr_m.find(s); it != rr_m.end()) return it->second;
	return rr_m.emplace(s, rr_v.size()), rr_v.push_back(s), rr_v.size() - 1;
};
inline const std::string& rr_dict(size_t i) { return rr_v[i]; };

// rel's signature = name id (rr_dict), offset arity and argument arity
struct rr_sig {
	size_t name = 0, offset_arity = 0, arg_arity = 0;
	auto operator<=>(const rr_sig&) const = default;
};

} // namespace idni::tau_lang
template<>
struct std::hash<idni::tau_lang::rr_sig> {
	size_t operator()(const idni::tau_lang::rr_sig& s) const noexcept {
		size_t seed = 0;
		idni::hash_combine(seed, s.name);
		idni::hash_combine(seed, s.offset_arity);
		idni::hash_combine(seed, s.arg_arity);
		return seed;
	}
};
namespace idni::tau_lang {

static auto sig2str = [](const rr_sig& s) {
	std::stringstream ss;
	return (ss << rr_dict(s.name) << "[" << s.offset_arity
		<< "]/" << s.arg_arity), ss.str();
};

static auto type2str = [](const tau_parser::nonterminal& t) {
	return tau_parser::instance().name(t);
};

template <typename... BAs>
rr_sig get_rr_sig(const tau_depreciating<BAs...>& n) {
	auto ref = n; // traverse to ref if n is bf_ref or wff_ref
	if (auto ref_as_child = n | tau_parser::ref; ref_as_child)
		ref = ref_as_child.value();
	return { rr_dict(make_string(tau_node_terminal_extractor<BAs...>,
					(ref | tau_parser::sym).value())),
		(ref | tau_parser::offsets || tau_parser::offset).size(),
		(ref | tau_parser::ref_args || tau_parser::ref_arg).size() };
};

// manages information about refs resolved/unresolved types and fp calls
template <typename... BAs>
struct rr_types {
	friend std::optional<rr<tau_depreciating<BAs...>>> infer_ref_types<BAs...>(
		const rr<tau_depreciating<BAs...>>&, rr_types<BAs...>&);
	rr_types(const rr<tau_depreciating<BAs...>>& nso_rr) { get_ref_types(nso_rr); }
	// returns false if any error or unresolved ref
	bool ok() const { return errors_.empty() && unresolved().empty(); }
	// returns set of errors
	const std::set<std::string>& errors() const { return errors_; }
	// returns set of unresolved refs
	std::set<rr_sig> unresolved() const {
		std::set<rr_sig> unres(todo_);
		std::erase_if(unres, [this](const rr_sig& x) {
			return fpcalls_.contains(x);
		});
		return unres;
	}
	// returns known type of a ref, or no value
	std::optional<tau_parser::nonterminal> type(const rr_sig& sig) const {
		rr_sig s(sig);
		auto fpopt = fpcall(s);
		if (fpopt) s = fpopt.value();
		if (auto it = types_.find(s); it != types_.end()) {
			BOOST_LOG_TRIVIAL(trace)
				<< "(I) -- Looking for type of " << sig2str(s)
				<< " found " << type2str(it->second);
			return { it->second };
		}
		BOOST_LOG_TRIVIAL(trace) << "(I) -- Looking for type of "
						<< sig2str(sig) << " failed";
		return {};
	}
	// returns ref to calculate fp by provided by fp call sig, or no value
	std::optional<rr_sig> fpcall(const rr_sig& fp_sig) const {
		if (auto it = fpcalls_.find(fp_sig); it != fpcalls_.end())
			return { it->second };
		return {};
	}
	// std::ostream& print(std::ostream& os) const {
	// 	os << "Types:\n";
	// 	for (const auto& [sig, t] : types_)
	// 		os << "\t" << sig2str(sig) << " : " << type2str(t) << "\n";
	// 	if (auto unres = unresolved(); !unres.empty()) {
	// 		os << "Unresolved:\n";
	// 		for (const auto& sig : unres)
	// 			os << "\t" << sig2str(sig) << "\n";
	// 	}
	// 	if (errors_.size()) {
	// 		os << "Errors:\n";
	// 		for (const auto& err : errors_)
	// 			os << "\t" << err << "\n";
	// 	}
	// 	return os;
	// }
private:
	void done(const rr_sig& sig) {
		BOOST_LOG_TRIVIAL(trace)
			<< "(I) -- ref type done " << sig2str(sig);
		todo_.erase(sig), done_.insert(sig);
	}
	void todo(const rr_sig& sig) {
		if (done_.contains(sig) || todo_.contains(sig)) return;
		BOOST_LOG_TRIVIAL(trace)
			<< "(I) -- ref type todo " << sig2str(sig);
		todo_.insert(sig);
	}
	void add_fpcall(const rr_sig& sig) {
		// TODO (LOW) decide how to call fp calculation for various
		// offset arity rels with otherwise same signature.
		// We currently call the rel with the least offset arity.
		// Should we provide a way how to specify exact relation to call? 
		rr_sig fp_sig(sig);
		fp_sig.offset_arity = 0;
		if (auto fp_exists = fpcall(fp_sig); fp_exists) {
			if (sig.offset_arity < fp_exists.value().offset_arity)
				fpcalls_[fp_sig] = sig;
		} else fpcalls_.emplace(fp_sig, sig);
	}
	// add sig with type t, and if it's already typed, check it equals to t
	bool add(const tau_depreciating<BAs...>& n, const tau_parser::nonterminal& t) {
		auto ref = n;
		if (auto ref_as_child = n | tau_parser::ref; ref_as_child)
			ref = ref_as_child.value();
		auto sig = get_rr_sig(ref);
		if (auto fp_sig = fpcall(sig); fp_sig) { // if fp_call
			BOOST_LOG_TRIVIAL(trace) << "(I) -- FP call "
				<< sig2str(fp_sig.value()) << " for "
				<< sig2str(sig) << "() : " << type2str(t);
			sig = fp_sig.value(); // use actual relation's sig
		}
		tau_parser::nonterminal new_type = t;
		auto it = types_.find(sig);
		if (it != types_.end()) {
			auto& rt = it->second;
			std::stringstream err;
			if (rt != t) err << "Type mismatch. ";
			if (err.tellp()) return
				err << sig2str(sig) << "() : "
					<< type2str(new_type) << " declared as "
					<< type2str(rt),
				errors_.insert(err.str()), false;
		} else {
			types_[sig] = new_type, done(sig);
			BOOST_LOG_TRIVIAL(trace) << "(I) -- Found type of "
				<< sig2str(sig) << "() : "
				<< type2str(types_[sig]);
			return true;
		}
		return false;
	};
	bool get_types(const tau_depreciating<BAs...>& n, bool def = false) {
		for (const auto& ref : select_all(n, // collect all refs to do
				is_non_terminal<tau_parser::ref, BAs...>))
		{
			auto sig = get_rr_sig(ref);
			todo(sig);
			if (def	&& sig.offset_arity > 0) add_fpcall(sig);
		}
		for (const auto& ref : select_all(n, // collect all wff typed refs
				is_non_terminal<tau_parser::wff_ref, BAs...>))
			add(ref, tau_parser::wff);
		for (const auto& ref : select_all(n, // collect all bf typed refs
				is_non_terminal<tau_parser::bf_ref, BAs...>))
			add(ref, tau_parser::bf);
		return errors_.empty();
	}
	bool get_ref_types(const rr<tau_depreciating<BAs...>>& nso_rr) {
		// get types from relations' heads if any
		for (const auto& r : nso_rr.rec_relations)
			get_types(r.first, true); // true since these are defs
		// from relations' bodies
		for (const auto& r : nso_rr.rec_relations) get_types(r.second);
		if (nso_rr.main) get_types(nso_rr.main); // from main if any
		return errors_.empty();
	}
	std::unordered_map<rr_sig, rr_sig> fpcalls_;
	std::unordered_map<rr_sig, tau_parser::nonterminal> types_;
	std::set<rr_sig> done_, todo_;
	std::set<std::string> errors_;
};

template <typename... BAs>
std::optional<rr<tau_depreciating<BAs...>>> infer_ref_types(const rr<tau_depreciating<BAs...>>& nso_rr,
	rr_types<BAs...>& ts)
{
	BOOST_LOG_TRIVIAL(trace) << "(I) -- Begin type inferrence"; // << ": " << nso_rr;
	// for (auto& r : nso_rr.rec_relations)
	// 	ptree<BAs...>(std::cout << "rule left: ", r.first) << "\n",
	// 	ptree<BAs...>(std::cout << "rule right: ", r.second) << "\n";
	// ptree<BAs...>(std::cout << "main: ", nso_rr.main) << "\n";
	static auto get_nt_type = [](const tau_depreciating<BAs...>& r) {
		size_t n = r | non_terminal_extractor<BAs...>
			| optional_value_extractor<size_t>;
		return static_cast<tau_parser::nonterminal>(n);
	};
	static auto update_ref = [](tau_depreciating<BAs...>& r,
		const tau_parser::nonterminal& t)
	{
		//ptree<BAs...>(std::cout << "updating ref: ", r) << "\n";
		r = wrap(t, wrap(t == tau_parser::wff
			? tau_parser::wff_ref : tau_parser::bf_ref, r));
		//ptree<BAs...>(std::cout << "updated ref: ", r) << "\n";
	};
	rr<tau_depreciating<BAs...>> nn = nso_rr;
	// inference loop
	bool changed;
	do {
		changed = false;
		for (auto& r : nn.rec_relations) {
			// check type of the right side
			auto t = get_nt_type(r.second);
			BOOST_LOG_TRIVIAL(trace) << "(T) " << r.second
						<< " is " << (type2str(t));
			if (t == tau_parser::ref) {
				// right side is unresolved ref
				if (auto topt = ts.type(get_rr_sig(r.second));
					topt.has_value())
				{
					t = topt.value();
					BOOST_LOG_TRIVIAL(trace)
						<< "(T) updating right side: "
						<< r.second;
					update_ref(r.second, t);
					changed = true;
				}
			}
			// update left side if right side is known
			if (t == tau_parser::bf || t == tau_parser::wff) {
				if (get_nt_type(r.first) == tau_parser::ref) {
					// left side is unresolved ref
					BOOST_LOG_TRIVIAL(trace)
						<< "(T) updating left side: "
						<< r.first;
					ts.add(r.first, t);
					update_ref(r.first, t);
					changed = true;
				}
			}
			// infer capture's type from the left side if known
			if (t == tau_parser::capture) {
				t = get_nt_type(r.first);
				// left side is an unresolved ref
				if (t == tau_parser::ref) {
					auto topt = ts.type(get_rr_sig(r.first));
					if (topt.has_value()) { // if we know
						t = topt.value(); // update
						BOOST_LOG_TRIVIAL(trace)
							<< "(T) updating left "
							"side: " << r.first;
						ts.done(get_rr_sig(r.first));
						update_ref(r.first, t);
						changed = true;
					}
				}
				// left side is bf or wff, update capture
				if (t == tau_parser::bf
					|| t == tau_parser::wff)
				{
					BOOST_LOG_TRIVIAL(trace)
						<< "(T) updating capture: "
						<< r.second;
					//ptree<BAs...>(std::cout << "updating ref: ", r) << "\n";
					r.second = wrap(t, r.second);
					//ptree<BAs...>(std::cout << "updated ref: ", r) << "\n";
				}
			}
		}
	} while (changed); // inference fixed point

	// infer main if unresolved ref
	if (nn.main) {
		auto t = get_nt_type(nn.main);
		BOOST_LOG_TRIVIAL(trace) << "(T) main " << nn.main
						<< " is " << (type2str(t));
		if (t == tau_parser::ref) {
			// main is an unresolved ref
			if (auto topt = ts.type(get_rr_sig(nn.main)); topt) {
				t = topt.value();
				BOOST_LOG_TRIVIAL(trace)
					<< "(T) updating main: " << nn.main;
				update_ref(nn.main, t);
			}
		}
	}

	for (const auto& err : ts.errors())
		BOOST_LOG_TRIVIAL(error) << "(Error) " << err;
	if (ts.errors().size()) return {};

	if (const auto& unresolved = ts.unresolved(); unresolved.size()) {
		std::stringstream ss;
		for (auto& sig : unresolved) ss << " " << sig2str(sig);
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) Unknown type for:" << ss.str();
		return {};
	}
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End type inferrence"; // << ": " << nn;
	return { nn };
}

template <typename... BAs>
std::optional<rr<tau_depreciating<BAs...>>> infer_ref_types(const rr<tau_depreciating<BAs...>>& nso_rr) {
	rr_types<BAs...> ts(nso_rr);
	return infer_ref_types(nso_rr, ts);
}

} // namespace tau
