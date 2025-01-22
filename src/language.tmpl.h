// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <optional>

#include "nso_rr.h"
#include "queries.h"

namespace idni::tau_lang {


template <typename... BAs>
tau<BAs...> trim(const tau<BAs...>& n) {
	return n->child[0];
}

template <typename... BAs>
tau<BAs...> trim2(const tau<BAs...>& n) {
	return n->child[0]->child[0];
}

template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const std::vector<tau<BAs...>>& nn)
{
	return make_node<tau_sym<BAs...>>(
		tau_parser::instance().literal(nt), nn);
}

template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt,
	const std::initializer_list<tau<BAs...>> ch)
{
	return wrap(nt, std::vector<tau<BAs...>>(ch));
}

template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const tau<BAs...>& n) {
	return wrap(nt, { n });
}

template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const tau<BAs...>& c1,
	const tau<BAs...>& c2)
{
	return wrap(nt, { c1, c2 });
}

template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const std::string& terminals) {
	std::vector<tau<BAs...>> children;
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
auto get_optional(const extractor_t& extractor, const tau<BAs...>& n) {
	return get_node<nt..., BAs...>(n).and_then(extractor);
}

// returns the extracted components/informations of the specified nodes, the
// component/information is extracted using the given extractor.
template <size_t... nt, typename extractor_t, typename... BAs>
auto get_optionals(const extractor_t& extractor, const tau<BAs...>& n) {
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

	tau<BAs...> operator()(const tau<BAs...>& n) {
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
		std::vector<tau<BAs...>> child;
		for (auto& c : n->child)
			if (changes.contains(c))
				changed = true, child.push_back(changes[c]);
			else child.push_back(c);
		auto nn = make_node<tau_sym<BAs...>>(n->value, child);
		if (changed) changes[n] = nn;
		return nn;
	}

	std::map<tau<BAs...>, tau<BAs...>> changes;
	binder_t binder;
	bool error = false;
};

// is not a whitespace predicate
template <typename... BAs>
static const auto not_whitespace_predicate = [](const tau<BAs...>& n) {
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

	tau<BAs...> bind(const tau<BAs...>& n) {
		auto binding = n | tau_parser::constant | tau_parser::binding;
		if (!binding || (n | tau_parser::type).has_value()) return n;
		auto bn = make_string<
				tau_node_terminal_extractor_t<BAs...>,
				tau<BAs...>>(
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

	tau<BAs...> bind(const tau<BAs...>& n) {
		if (error) return nullptr;
		auto binding = n | tau_parser::constant | tau_parser::binding;
		if (!binding) return n; // not a binding (capture?)
		auto type = find_top(n, is_non_terminal<tau_parser::type, BAs...>);
		if (type) {
			// the factory take two arguments, the first is the type and the
			// second is the node representing the constant.
			std::string type_name = make_string<
				tau_node_terminal_extractor_t<BAs...>,
				tau<BAs...>>(
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
rewriter::rule<tau<BAs...>> make_rule(tau_parser::nonterminal rule_t,
	tau_parser::nonterminal matcher_t, tau_parser::nonterminal body_t,
	const tau<BAs...>& rule)
{
	auto matcher = rule | rule_t | matcher_t | only_child_extractor<BAs...>
		| optional_value_extractor<tau<BAs...>>;
	auto body = rule | rule_t | body_t | only_child_extractor<BAs...>
		| optional_value_extractor<tau<BAs...>>;
	return { matcher, body };
}

// creates a specific rule from a generic rule
// TODO (LOW) should depend in node_t instead of BAs...
template <typename... BAs>
rewriter::rule<tau<BAs...>> make_rule(const tau<BAs...>& rule) {
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
rules<tau<BAs...>> make_rules(tau<BAs...>& tau_source) {
	rules<tau<BAs...>> rs;
	// TODO (LOW) change call to select by operator|| and operator|
	for (auto& r: select_top(tau_source,
		is_non_terminal<tau_parser::rule, BAs...>))
			rs.push_back(make_rule<BAs...>(r));
	return rs;
}

// create a set of relations from a given tau source.
// TODO (LOW) should depend in node_t instead of BAs...
template <typename... BAs>
rec_relations<tau<BAs...>> make_rec_relations(
	const tau<BAs...>& tau_source)
{
	rec_relations<tau<BAs...>> rs;
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
tau<BAs...> process_digits(const tau<BAs...>& tau_source) {
	std::map<tau<BAs...>, tau<BAs...>> changes;
	for(auto& n: select_top(tau_source,
		is_non_terminal<tau_parser::digits, BAs...>))
	{
		auto offset = make_string<
				tau_node_terminal_extractor_t<BAs...>,
				tau<BAs...>>(
			tau_node_terminal_extractor<BAs...>,  n);
		auto num = std::stoul(offset);
		auto nn = rewriter::make_node<tau_sym<BAs...>>(tau_sym<BAs...>(num), {});
		changes[n] = nn;
	}
	return replace<tau<BAs...>>(tau_source, changes);
}

// } // include for ptree<BAs...>()
// #include "debug_helpers.h"
// namespace idni::tau_lang {

template <typename... BAs>
struct quantifier_vars_transformer {
	using p = tau_parser;
	using node = tau<BAs...>;
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
		std::vector<tau<BAs...>> child;
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
tau<BAs...> process_quantifier_vars(const tau<BAs...>& tau_code) {
	using node = tau<BAs...>;
	quantifier_vars_transformer<BAs...> transformer;
	return rewriter::post_order_traverser<quantifier_vars_transformer<BAs...>,
		rewriter::all_t, node>(transformer, rewriter::all)(tau_code);
}

template <typename...BAs>
tau<BAs...> process_offset_variables(const tau<BAs...>& tau_code) {
	using p = tau_parser;
	using node = tau<BAs...>;
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
tau<BAs...> process_defs_input_variables(const tau<BAs...>& tau_code) {
	using p = tau_parser;
	using node = tau<BAs...>;
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
tau<BAs...> infer_constant_types(const tau<BAs...>& code) {
	BOOST_LOG_TRIVIAL(trace)
		<< "(T) infer constant types: " << code;
	using node = tau<BAs...>;
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
						<< "(T) type mismatch: " << type << " got: " << got << " for: " << c;
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
			tau_parser::bf_neq,
			tau_parser::bf_eq,
			tau_parser::bf_nleq,
			tau_parser::bf_greater,
			tau_parser::bf_less_equal,
			tau_parser::bf_less,
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

	free_vars_collector(std::set<tau<BAs...>>& free_vars) : free_vars(free_vars) {}

	tau<BAs...> operator()(const tau<BAs...>& n) {
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
			if (auto check = n
					| tau_parser::io_var | only_child_extractor<BAs...> | tau_parser::offset
					| only_child_extractor<BAs...>;
					check.has_value() && is_var_or_capture<BAs...>(check.value())) {
				auto var = check.value();
				if (auto it = free_vars.find(var); it != free_vars.end()) {
					free_vars.erase(it);
					BOOST_LOG_TRIVIAL(trace) << "(I) -- removing var: " << var;
				}
			}
			free_vars.insert(n);
			BOOST_LOG_TRIVIAL(trace) << "(I) -- inserting var: " << n;
		}
		return n;
	}

	std::set<tau<BAs...>>& free_vars;
};

template <typename... BAs>
auto get_free_vars_from_nso(const tau<BAs...>& n) {
	BOOST_LOG_TRIVIAL(trace) << "(I) -- Begin get_free_vars_from_nso of " << n;
	std::set<tau<BAs...>> free_vars;
	free_vars_collector<BAs...> collector(free_vars);
	rewriter::post_order_traverser<
			free_vars_collector<BAs...>,
			rewriter::all_t,
			tau<BAs...>>(collector, rewriter::all)(n);
	BOOST_LOG_TRIVIAL(trace) << "(I) -- End get_free_vars_from_nso";
	return free_vars;
}

// TODO (MEDIUM) unify this code with get_tau_nso_clause and get_tau_nso_literals
template<typename ...BAs>
void get_leaves(const tau<BAs...>& n, tau_parser::nonterminal branch,
	tau_parser::nonterminal skip, std::vector<tau<BAs...>>& leaves)
{
    if (is_child_non_terminal(branch, n)) {
	    for (const auto& c : trim(n)->child)
	    	if (is_non_terminal(skip, c))
	    		get_leaves(c, branch, skip, leaves);
    } else {
        leaves.push_back(n);
        BOOST_LOG_TRIVIAL(trace) << "(I) get_leaves: found clause: " << n;
    }
}

template<typename ...BAs>
std::vector<tau<BAs...>> get_leaves(const tau<BAs...>& n,
	tau_parser::nonterminal branch, tau_parser::nonterminal skip)
{
	std::vector<tau<BAs...>> leaves;
	get_leaves(n, branch, skip, leaves);
	return leaves;
}

template<typename ...BAs>
std::vector<tau<BAs...>> get_dnf_wff_clauses(const tau<BAs...>& n) {
	return get_leaves(n, tau_parser::wff_or, tau_parser::wff);
}

template<typename ...BAs>
std::vector<tau<BAs...>> get_dnf_bf_clauses(const tau<BAs...>& n) {
	return get_leaves(n, tau_parser::bf_or, tau_parser::bf);
}

template<typename ...BAs>
std::vector<tau<BAs...>> get_cnf_wff_clauses(const tau<BAs...>& n) {
	return get_leaves(n, tau_parser::wff_and, tau_parser::wff);
}

template<typename ...BAs>
std::vector<tau<BAs...>> get_cnf_bf_clauses(const tau<BAs...>& n) {
	return get_leaves(n, tau_parser::bf_and, tau_parser::bf);
}

// A formula has a temporal variable if either it contains an io_var with a variable or capture
// or it contains a flag
template <typename... BAs>
bool has_temp_var (const tau<BAs...>& fm) {
	auto io_vars = select_top(fm, is_non_terminal<tau_parser::io_var, BAs...>);
	if (io_vars.empty()) return find_top(fm, is_non_terminal<tau_parser::constraint, BAs...>).has_value();
	// any input/output stream is a temporal variable, also constant positions
	else return true;
}

// Check that no non-temporal quantified variable appears nested in the scope of
// temporal quantification
template <typename... BAs>
bool invalid_nesting_of_quants (const tau<BAs...>& fm) {
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
bool invalid_nesting_of_temp_quants (const tau<BAs...>& fm) {
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
bool has_open_tau_fm_in_constant (const tau<BAs...>& fm) {
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

// This function is used to check for semantic errors in formulas since those
// cannot be captured by the grammar
template <typename... BAs>
bool has_semantic_error (const tau<BAs...>& fm) {
	bool error = invalid_nesting_of_quants(fm)
			|| has_open_tau_fm_in_constant(fm)
			|| invalid_nesting_of_temp_quants(fm);
	return error;
}

// create tau code from tau source
template <typename... BAs>
// TODO (LOW) should depend on node_t instead of BAs...
tau<BAs...> make_tau_code(sp_tau_source_node& tau_source) {
	if (!tau_source) return 0;
	tauify<BAs...> tf;
	rewriter::map_transformer<tauify<BAs...>,
		sp_tau_source_node, tau<BAs...>> transform(tf);
	auto tau_code = rewriter::post_order_traverser<
				rewriter::map_transformer<tauify<BAs...>,
				sp_tau_source_node, tau<BAs...>>,
			rewriter::all_t,
			rewriter::sp_node<tau_source_sym>,
			tau<BAs...>>(
		transform, rewriter::all)(tau_source);
	if (!tau_code) return nullptr;
	return infer_constant_types(
		process_defs_input_variables(
		process_offset_variables(
		process_quantifier_vars(
		process_digits(tau_code)))));
}

// make a library from the given tau source.
// TODO (LOW) should depend on node_t instead of BAs...
template <typename... BAs>
library<tau<BAs...>> make_library(sp_tau_source_node& tau_source) {
	auto lib = make_tau_code<BAs...>(tau_source);
	return make_rules(lib);
}

// make a library from the given tau source string.
// TODO (LOW) should depend on node_t instead of BAs...
template <typename... BAs>
library<tau<BAs...>> make_library(const std::string& source) {
	auto tau_source = make_tau_source(source, {
						.start = tau_parser::library });
	return make_library<BAs...>(tau_source);
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
tau<BAs...> bind_tau_code_using_binder(const tau<BAs...>& code,
	binder_t& binder)
{
	bind_transformer<binder_t, BAs...> bs(binder);
	auto res = rewriter::post_order_traverser<
			bind_transformer<binder_t, BAs...>,
			rewriter::all_t,
			tau<BAs...>>(bs, rewriter::all)(code);
	// Check for errors which cannot be captured by the grammar
	if (bs.error) return nullptr;
	if (has_semantic_error(res)) return {};
	else return res;
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
tau<BAs...> bind_tau_code_using_bindings(tau<BAs...>& code,
	const bindings<BAs...>& bindings)
{
	name_binder<BAs...> nb(bindings);
	return bind_tau_code_using_binder<
			name_binder<BAs...>, BAs...>(code, nb);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
tau<BAs...> bind_tau_code_using_factory(const tau<BAs...>& code)
{
	factory_binder<BAs...> fb;
	return bind_tau_code_using_binder<factory_binder<BAs...>, BAs...>(code, fb);
}

// make a nso_rr from the given tau code
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_from_binded_code(
	const tau<BAs...>& code)
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
				| optional_value_extractor<tau<BAs...>>;
	auto rules = make_rec_relations<BAs...>(code);
	return infer_ref_types<BAs...>(rr<tau<BAs...>>{ rules, main });
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_binder(const tau<BAs...>& code,
	binder_t& binder)
{
	auto binded = bind_tau_code_using_binder<binder_t, BAs...>(code, binder);
	if (!binded) return {};
	return make_nso_rr_from_binded_code<BAs...>(binded);
}

template<typename binder_t, typename... BAs>
tau<BAs...> make_nso_using_binder(const tau<BAs...>& code,
	binder_t& binder)
{
	return bind_tau_code_using_binder<binder_t, BAs...>(code, binder);
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_binder(
	sp_tau_source_node& source, binder_t& binder)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return {};
	return make_nso_rr_using_binder<binder_t, BAs...>(code, binder);
}

template<typename binder_t, typename... BAs>
tau<BAs...> make_nso_using_binder(sp_tau_source_node& source,
	binder_t& binder)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return nullptr;
	return make_nso_using_binder<binder_t, BAs...>(code, binder);
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_binder(std::string& input,
	binder_t& binder)
{
	auto source = make_tau_source(input, { .start = tau_parser::rr });
	return !source ? std::optional<rr<tau<BAs...>>>{}
		: std::optional<rr<tau<BAs...>>>{
			make_nso_rr_using_binder<binder_t, BAs...>(
				source, binder) };
}

template<typename binder_t, typename... BAs>
std::optional<tau<BAs...>> make_nso_using_binder(std::string& input,
	binder_t& binder,
	idni::parser<>::parse_options options = { .start = tau_parser::wff })
{
	auto source = make_tau_source(input, options);
	return !source ? std::optional<tau<BAs...>>{}
		: std::optional<tau<BAs...>>{
			make_nso_using_binder<binder_t, BAs...>(
				source, binder) };
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_bindings(
	const tau<BAs...>& code, const bindings<BAs...>& bindings)
{
	name_binder<BAs...> nb(bindings);
	return make_nso_rr_using_binder<name_binder<BAs...>, BAs...>(code, nb);
}

template <typename... BAs>
tau<BAs...> make_nso_using_bindings(const tau<BAs...>& code,
	const bindings<BAs...>& bindings)
{
	name_binder<BAs...> nb(bindings);
	return make_nso_using_binder<name_binder<BAs...>, BAs...>(code, nb);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_bindings(
	sp_tau_source_node& source, const bindings<BAs...>& bindings)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return {};
	return make_nso_rr_using_bindings<BAs...>(code, bindings);
}

template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_bindings(sp_tau_source_node& source,
	const bindings<BAs...>& bindings)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return {};
	return make_nso_using_bindings<BAs...>(code, bindings);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_bindings(
	const std::string& input, const bindings<BAs...>& bindings)
{
	auto source = make_tau_source(input, { .start = tau_parser::rr });
	if (!source) return std::optional<rr<tau<BAs...>>>{};
	return make_nso_rr_using_bindings<BAs...>(source, bindings);
}
// make a nso from the given tau source and bindings.
template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_bindings(
	const std::string& input, const bindings<BAs...>& bindings,
	idni::parser<>::parse_options options)
{
	auto source = make_tau_source(input, options);
	if (!source) return std::optional<tau<BAs...>>{};
	return make_nso_using_bindings<BAs...>(source, bindings);
}

template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_factory(
	const tau<BAs...>& code)
{
	factory_binder<BAs...> fb;
	return make_nso_rr_using_binder<
			factory_binder<BAs...>, BAs...>(code,fb);
}

template <typename... BAs>
tau<BAs...> make_nso_using_factory(const tau<BAs...>& code) {
	factory_binder<BAs...> fb;
	return make_nso_using_binder<
			factory_binder<BAs...>, BAs...>(code,fb);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_factory(
	sp_tau_source_node& source)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return std::optional<rr<tau<BAs...>>>{};
	return make_nso_rr_using_factory<BAs...>(code);
}

template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_factory(
	sp_tau_source_node& source)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return std::optional<tau<BAs...>>{};
	return make_nso_using_factory<BAs...>(code);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_factory(
	const std::string& input)
{
	auto source = make_tau_source(input, { .start = tau_parser::rr });
	if (!source) return std::optional<rr<tau<BAs...>>>{};
	return make_nso_rr_using_factory<BAs...>(source);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_factory(
	const std::string& input,
	idni::parser<>::parse_options options)
{
	auto source = make_tau_source(input, options);
	if (!source) return std::optional<tau<BAs...>>{};
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

// pair of name id (rr_dict) and argument arity
using rr_sig = std::pair<size_t, size_t>;

struct rr_type {
	tau_parser::nonterminal type;
	size_t offset_arity = 0;
	bool fp = false;
};

struct rr_types {
	std::unordered_map<rr_sig, rr_type> types;
	std::set<std::string> errors;
};

static auto sig2str = [](const rr_sig& s) {
	std::stringstream ss;
	return (ss << rr_dict(s.first) << "/" << s.second), ss.str();
};

static auto type2str = [](const tau_parser::nonterminal& t) {
	return tau_parser::instance().name(t);
};

inline std::ostream& print_rr_type(std::ostream& os, const rr_type& t) {
	os << type2str(t.type) << "[";
	if (t.fp) os << "*"; else os << t.offset_arity;
	return os << "]";
}

inline std::string rr_type2str(const rr_type& t) {
	std::stringstream ss;
	print_rr_type(ss, t);
	return ss.str();
}

template <typename... BAs>
rr_sig get_rr_sig(const tau<BAs...>& n) {
	auto ref = n;
	if (auto ref_as_child = n | tau_parser::ref; ref_as_child)
		ref = ref_as_child.value();
	auto args = ref | tau_parser::ref_args || tau_parser::ref_arg;
	return { rr_dict(make_string(tau_node_terminal_extractor<BAs...>,
		(ref | tau_parser::sym).value())), args.size() };
};

template <typename... BAs>
rr_sig get_ref_type(bool& success, rr_types& ts,
	const tau<BAs...>& n, const tau_parser::nonterminal& t,
	bool possible_fp = false)
{
	auto ref = n;
	if (auto ref_as_child = n | tau_parser::ref; ref_as_child)
		ref = ref_as_child.value();
	auto sig = get_rr_sig(ref);
	auto oarity = (ref | tau_parser::offsets || tau_parser::offset).size();
	rr_type new_type(t, oarity, possible_fp && oarity == 0);
	auto it = ts.types.find(sig);
	if (it != ts.types.end()) {
		auto& rt = it->second;
		std::stringstream err;
		if (rt.type != t) err << "Type mismatch. ";
		if (rt.fp && oarity) {
			// found a real offset arity for a uncomplete type taken
			// from main with a fp call => update offset arity
			ts.types[sig].offset_arity = oarity;
			ts.types[sig].fp = false;
			BOOST_LOG_TRIVIAL(trace) << "(I) -- Updated type of "
				<< sig2str(sig) << "() : "
				<< rr_type2str(ts.types[sig]);
		}
		if (!rt.fp && !new_type.fp && rt.offset_arity != oarity)
			err << "Offset arity mismatch. ";
		if (err.tellp()) return
			err << sig2str(sig) << "() : " << rr_type2str(new_type)
				<< " declared as " << rr_type2str(rt),
			ts.errors.insert(err.str()),
			success = false, rr_sig{ 0, 0 };
	} else {
		ts.types[sig] = new_type;
		BOOST_LOG_TRIVIAL(trace) << "(I) -- Found type of "
			<< sig2str(sig) << "() : " << rr_type2str(ts.types[sig]);
		return sig;
	}
	return rr_sig{ 0, 0 };
};

template <typename... BAs>
std::pair<std::set<rr_sig>, std::set<rr_sig>> get_rr_types(
	bool& success, rr_types& ts, const tau<BAs...>& n,
	bool possible_fp = false)
{
	std::set<rr_sig> done_names;
	std::set<rr_sig> todo_names;
	for (const auto& ref : select_all(n,
			is_non_terminal<tau_parser::ref, BAs...>))
		todo_names.insert(get_rr_sig(ref));
	for (const auto& ref : select_all(n,
			is_non_terminal<tau_parser::wff_ref, BAs...>))
	{
		auto sig = get_ref_type(success, ts, ref, tau_parser::wff,
			possible_fp);
		done_names.insert(sig), todo_names.erase(sig);
	}
	for (const auto& ref : select_all(n,
			is_non_terminal<tau_parser::bf_ref, BAs...>))
	{
		auto sig = get_ref_type(success, ts, ref, tau_parser::bf,
			possible_fp);
		done_names.insert(sig), todo_names.erase(sig);
	}
	return { done_names, todo_names };
}

template <typename... BAs>
std::pair<std::set<rr_sig>, std::set<rr_sig>> get_rr_types(
	bool& success, rr_types& ts, const rr<tau<BAs...>>& nso_rr)
{
	std::set<rr_sig> done_names;
	std::set<rr_sig> todo_names;
	auto add_ref_names = [&done_names, &todo_names](const std::pair<
		std::set<rr_sig>, std::set<rr_sig>>& names)
	{
		for (const auto& sig : names.first)
			done_names.insert(sig), todo_names.erase(sig);
		for (const auto& sig : names.second)
	 		if (done_names.find(sig) == done_names.end())
				todo_names.insert(sig);
	};
	// get types from relations if any
	for (const auto& r : nso_rr.rec_relations)
		add_ref_names(get_rr_types(success, ts, r.first)),
		add_ref_names(get_rr_types(success, ts, r.second));
	// get type from main if any
	if (nso_rr.main)
		add_ref_names(get_rr_types(success, ts, nso_rr.main, true));
	return { done_names, todo_names };
}

template <typename... BAs>
std::optional<rr<tau<BAs...>>> infer_ref_types(const rr<tau<BAs...>>& nso_rr) {
	BOOST_LOG_TRIVIAL(trace) << "(I) -- Begin type inferrence"; // << ": " << nso_rr;
	// for (auto& r : nso_rr.rec_relations)
	// 	ptree<BAs...>(std::cout << "rule left: ", r.first) << "\n",
	// 	ptree<BAs...>(std::cout << "rule right: ", r.second) << "\n";
	// ptree<BAs...>(std::cout << "main: ", nso_rr.main) << "\n";
	rr<tau<BAs...>> nn = nso_rr;
	rr_types ts;
	std::set<rr_sig> done_names, todo_names;
	bool success = true;
	static auto get_nt_type = [](const tau<BAs...>& r) {
		size_t n = r | non_terminal_extractor<BAs...>
			| optional_value_extractor<size_t>;
		return static_cast<tau_parser::nonterminal>(n);
	};
	static auto update_ref = [](tau<BAs...>& r,
		const tau_parser::nonterminal& t)
	{
		//ptree<BAs...>(std::cout << "updating ref: ", r) << "\n";
		r = wrap(t, wrap(t == tau_parser::wff
			? tau_parser::wff_ref : tau_parser::bf_ref, r));
		//ptree<BAs...>(std::cout << "updated ref: ", r) << "\n";
	};
	auto add_ref_names = [&done_names, &todo_names](const std::pair<
		std::set<rr_sig>, std::set<rr_sig>>& names)
	{
		for (const auto& sig : names.first)
			done_names.insert(sig), todo_names.erase(sig);
		for (const auto& sig : names.second)
	 		if (done_names.find(sig) == done_names.end())
				todo_names.insert(sig);
	};
	auto done = [&done_names, &todo_names](const rr_sig& sig) {
		todo_names.erase(sig), done_names.insert(sig);
	};
	// get types from relations if any
	for (const auto& r : nn.rec_relations)
		add_ref_names(get_rr_types(success, ts, r.first)),
		add_ref_names(get_rr_types(success, ts, r.second));
	// get type from main if any
	if (nn.main) add_ref_names(get_rr_types(success, ts, nn.main, true));
	// inference loop
	for (;;) {
		bool changed = false;
		for (auto& r : nn.rec_relations) {
			// check type of the right side
			auto t = get_nt_type(r.second);
			BOOST_LOG_TRIVIAL(trace) << "(T) " << r.second << " is " << (type2str(t));
			if (t == tau_parser::ref) {
				// right side is unresolved ref
				auto it = ts.types.find(get_rr_sig(r.second)); // if we know type
				if (it != ts.types.end()) {  // update
					BOOST_LOG_TRIVIAL(trace) << "(T) updating right side: " << r.second;
					t = it->second.type;
					update_ref(r.second, t);
					changed = true;
				}
			}
			// update left side if right side is known
			if (t == tau_parser::bf || t == tau_parser::wff) {
				if (get_nt_type(r.first) == tau_parser::ref) {
					// left side is unresolved ref
					BOOST_LOG_TRIVIAL(trace) << "(T) updating left side: " << r.first;
					done(get_ref_type(
						success, ts, r.first, t));
					update_ref(r.first, t);
					changed = true;
				}
			}
			// infer capture's type from the left side if known
			if (t == tau_parser::capture) {
				auto lt = get_nt_type(r.first);
				// left side is an unresolved ref
				if (lt == tau_parser::ref) {
					auto it = ts.types
						.find(get_rr_sig(r.first)); // if we know
					if (it != ts.types.end()) { // type -> assign
						lt = it->second.type;
						BOOST_LOG_TRIVIAL(trace) << "(T) updating known type of the left side: " << r.first;
						done(get_ref_type(success, ts,
							r.first, lt));
						update_ref(r.first, lt);
						changed = true;
					}
				}
				// left side is bf or wff, update capture
				if (lt == tau_parser::bf
					|| lt == tau_parser::wff)
				{
					BOOST_LOG_TRIVIAL(trace) << "(T) updating capture: " << r.second;
					//ptree<BAs...>(std::cout << "updating ref: ", r) << "\n";
					r.second = wrap(lt, r.second);
					//ptree<BAs...>(std::cout << "updated ref: ", r) << "\n";
				}
			}
		}
		if (!changed) break; // fixed point
	}

	// infer main if unresolved ref
	if (nn.main) {
		//add_ref_names(get_rr_types(success, types, nn.main, true));
		auto t = get_nt_type(nn.main);
		BOOST_LOG_TRIVIAL(trace) << "(T) main " << nn.main << " is " << (type2str(t));
		if (t == tau_parser::ref) {
			// main is an unresolved ref
			auto it = ts.types.find(get_rr_sig(nn.main)); // if we know type
			if (it != ts.types.end()) {  // update
				BOOST_LOG_TRIVIAL(trace) << "(T) updating main: " << nn.main;
				t = it->second.type;
				update_ref(nn.main, t);
			}
		}
	}

	for (const auto& err : ts.errors)
		BOOST_LOG_TRIVIAL(error) << "(Error) " << err;
	if (ts.errors.size()) return {};

	if (todo_names.size()) {
		std::stringstream ss;
		for (auto& sig : todo_names) ss << " " << sig2str(sig);
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) Unknown type for:" << ss.str();
		return {};
	}
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End type inferrence"; // << ": " << nn;
	return { nn };
}

} // namespace tau
