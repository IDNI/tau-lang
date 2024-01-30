// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#ifndef __TAU_H__
#define __TAU_H__

#include <iostream>

#include "normalizer2.h"

using namespace std;
using namespace idni::tau;

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

namespace idni::tau {

// tau rules
RULE(TAU_DISTRIBUTE_0, "(($X ||| $Y) &&& $Z) := (($X &&& $Z) ||| ($Y &&& $Z)).")
RULE(TAU_DISTRIBUTE_1, "($X &&& ($Y ||| $Z)) := (($X &&& $Y) ||| ($X &&& $Z)).")
RULE(TAU_PUSH_NEGATION_INWARDS_0, "!!! ($X &&& $Y) := (!!! $X ||| !!! $Y).")
RULE(TAU_PUSH_NEGATION_INWARDS_1, "!!! ($X ||| $Y) := (!!! $X &&& !!! $Y).")
RULE(TAU_ELIM_DOUBLE_NEGATION_0, "!!! !!! $X :=  $X.")
RULE(TAU_SIMPLIFY_ONE_0, "( {T} ||| $X ) := {T}.")
RULE(TAU_SIMPLIFY_ONE_1, "( $X ||| {T} ) := {T}.")
RULE(TAU_SIMPLIFY_ONE_2, "( {T} &&& $X ) := $X.")
RULE(TAU_SIMPLIFY_ONE_3, "( $X &&& {T} ) := $X.")
RULE(TAU_SIMPLIFY_ONE_4, "!!! {T} := {F}.")
RULE(TAU_SIMPLIFY_ZERO_0, "( {F} &&& $X ) := {F}.")
RULE(TAU_SIMPLIFY_ZERO_1, "( $X &&& {F} ) := {F}.")
RULE(TAU_SIMPLIFY_ZERO_2, "( {F} ||| $X ) := $X.")
RULE(TAU_SIMPLIFY_ZERO_3, "( $X ||| {F} ) := $X.")
RULE(TAU_SIMPLIFY_ZERO_4, "!!! {F} := {T}.")
RULE(TAU_SIMPLIFY_SELF_0, "( $X &&& $X ) := $X.")
RULE(TAU_SIMPLIFY_SELF_1, "( $X ||| $X ) := $X.")
RULE(TAU_SIMPLIFY_SELF_2, "( $X &&& !!! $X ) := {F}.")
RULE(TAU_SIMPLIFY_SELF_3, "( $X ||| !!! $X ) := {T}.")
RULE(TAU_SIMPLIFY_SELF_4, "( !!! $X &&& $X ) := {F}.")
RULE(TAU_SIMPLIFY_SELF_5, "( !!! $X ||| $X ) := {T}.")

RULE(TAU_COLLAPSE_POSITIVES_0, "($X &&& $Y) := tau_collapse_positives_cb $X $Y.")
RULE(TAU_COLLAPSE_POSITIVES_1, "($X &&& ($Y &&& $Z)) := tau_collapse_positives_cb $X $Y $Z.")
RULE(TAU_COLLAPSE_POSITIVES_2, "($X &&& ($Y &&& $Z)) := tau_collapse_positives_cb $X $Z $Y.")
RULE(TAU_COLLAPSE_POSITIVES_3, "($X &&& ($Y &&& $Z)) := tau_collapse_positives_cb $Y $Z $X.")
RULE(TAU_COLLAPSE_POSITIVES_4, "(($X &&& $Y) &&& $Z) := tau_collapse_positives_cb $Y $Z $X.")
RULE(TAU_COLLAPSE_POSITIVES_5, "(($X &&& $Y) &&& $Z) := tau_collapse_positives_cb $X $Z $Y.")
RULE(TAU_COLLAPSE_POSITIVES_6, "(($X &&& $Y) &&& $Z) := tau_collapse_positives_cb $X $Y $Z.")
RULE(TAU_PUSH_POSITIVES_UPWARDS_0, "($X &&& ($Y &&& $Z)) := tau_positives_upwards_cb $Y ($Y &&& ($X &&& $Z)).")
RULE(TAU_PUSH_POSITIVES_UPWARDS_1, "($X &&& ($Y &&& $Z)) := tau_positives_upwards_cb $Z ($Z &&& ($X &&& $Y)).")
RULE(TAU_PUSH_POSITIVES_UPWARDS_2, "(($X &&& $Y) &&& $Z) := tau_positives_upwards_cb $X ($X &&& ($Y &&& $Z)).")
RULE(TAU_PUSH_POSITIVES_UPWARDS_3, "(($X &&& $Y) &&& $Z) := tau_positives_upwards_cb $Y ($Y &&& ($X &&& $Z)).")

template<typename... BAs>
static auto to_dnf_tau = make_library<BAs...>(
	TAU_DISTRIBUTE_0
	+ TAU_DISTRIBUTE_1
	+ TAU_PUSH_NEGATION_INWARDS_0
	+ TAU_PUSH_NEGATION_INWARDS_1
	+ TAU_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto simplify_tau = make_library<BAs...>(
	TAU_SIMPLIFY_ONE_0
	+ TAU_SIMPLIFY_ONE_1
	+ TAU_SIMPLIFY_ONE_2
	+ TAU_SIMPLIFY_ONE_3
	+ TAU_SIMPLIFY_ONE_4
	+ TAU_SIMPLIFY_ZERO_0
	+ TAU_SIMPLIFY_ZERO_1
	+ TAU_SIMPLIFY_ZERO_2
	+ TAU_SIMPLIFY_ZERO_3
	+ TAU_SIMPLIFY_ZERO_4
	+ TAU_SIMPLIFY_SELF_0
	+ TAU_SIMPLIFY_SELF_1
	+ TAU_SIMPLIFY_SELF_2
	+ TAU_SIMPLIFY_SELF_3
	+ TAU_SIMPLIFY_SELF_4
	+ TAU_SIMPLIFY_SELF_5
);

template<typename... BAs>
static auto collapse_positives_tau = make_library<BAs...>(
	TAU_COLLAPSE_POSITIVES_0
	+ TAU_COLLAPSE_POSITIVES_1
	+ TAU_COLLAPSE_POSITIVES_2
	+ TAU_COLLAPSE_POSITIVES_3
	+ TAU_COLLAPSE_POSITIVES_4
	+ TAU_COLLAPSE_POSITIVES_5
	+ TAU_COLLAPSE_POSITIVES_6
	+ TAU_PUSH_POSITIVES_UPWARDS_0
	+ TAU_PUSH_POSITIVES_UPWARDS_1
	+ TAU_PUSH_POSITIVES_UPWARDS_2
	+ TAU_PUSH_POSITIVES_UPWARDS_3
);

// Check https://gcc.gnu.org/bugzilla/show_bug.cgi?id=102609 to follow up on
// the implementation of "Deducing this" on gcc.
// See also (https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0847r7.html)
// and https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/ for how to use
// "Deducing this" on CRTP..

// TODO (HIGH) give a proper implementation for <=>, == and != operators
template<typename...BAs>
auto operator<=>(const tau_sym<BAs...>& l, const tau_sym<BAs...>& r) {
	/*auto check = std::addressof(l)<=>std::addressof(r);
	std::cout << "l  <=> r = ";
	if (check == std::strong_ordering::less) std::cout << "less" << std::endl;
	else if (check == std::strong_ordering::equal) std::cout << "equal" << std::endl;
	else if (check == std::strong_ordering::greater) std::cout << "greater" << std::endl;
	else std::cout << "unordered" << std::endl;
	return check;*/
	return std::addressof(l)<=>std::addressof(r);
	/*auto cmp = overloaded(*/
		/*[](const tau_source_sym& l, const tau_source_sym& r) -> std::partial_ordering {*/
		/*[](const tau_source_sym& l, const tau_source_sym& r) -> std::strong_ordering {
			return l<=>r;*/
			/*std::strong_ordering strong = l<=> r;
			if (strong == std::strong_ordering::less) {
				return std::partial_ordering::less;
			} else if (strong == std::strong_ordering::equal) {
				return std::partial_ordering::equivalent;
			} else  {
				return std::partial_ordering::greater;
			}*/
		/*}, [](const tau_source_sym&, const auto&) -> std::partial_ordering { return std::partial_ordering::greater; },*/
		/*}, [](const tau_source_sym&, const auto&) -> std::strong_ordering { return std::strong_ordering::greater; },*/
		/*[](const auto&, const tau_source_sym&) -> std::partial_ordering { return std::strong_ordering::less; },*/
		/*[](const auto&, const tau_source_sym&) -> std::strong_ordering { return std::strong_ordering::less; },*/
		/*[](const size_t& l, const size_t& r) -> std::partial_ordering {*/
		/*[](const size_t& l, const size_t& r) -> std::strong_ordering {
			return l<=>r;*/
			/*std::strong_ordering strong = l<=> r;
			if (strong == std::strong_ordering::less) {
				return std::partial_ordering::less;
			} else if (strong == std::strong_ordering::equal) {
				return std::partial_ordering::equivalent;
			} else  {
				return std::partial_ordering::greater;
			}*/
		/*},*/
		/*[](const size_t&, const auto&) -> std::partial_ordering { return std::partial_ordering::greater; },*/
		/*[](const size_t&, const auto&) -> std::strong_ordering { return std::strong_ordering::greater; },*/
		/*[](const auto&, const size_t&) -> std::partial_ordering { return std::partial_ordering::less; },*/
		/*[](const auto&, const size_t&) -> std::strong_ordering { return std::strong_ordering::less; },*/
		// we could allow unordered BAs by defining here the order of the BAs as follows
		// []<typename T>(const T& l, const T& r) -> std::partial_ordering {
		//		if ((l & ~r == false) | (l ^ ~r != false)) return std::partial_ordering::less;
		//		if (l ^ r == false) return std::partial_ordering::equivalent;
		//		if ((l & ~r != false) | (l ^ ~r == false)) return std::partial_ordering::greater;
		//		return std::partial_ordering::unordered;
		// otherwise, we need the following:
		/*[]<typename T>(const T& l, const T& r) -> std::partial_ordering {*/
		/*[]<typename T>(const T& l, const T& r) -> std::strong_ordering {
			return std::addressof(l)<=>std::addressof(r);*/
			//return l<=>r;
		/*},*/
		/*[](const auto&, const auto&) -> std::partial_ordering { return std::partial_ordering::unordered; }*/
		/*[](const auto&, const auto&) -> std::partial_ordering { throw std::logic_error("bad type"); }
	);
	return std::visit(cmp, l, r);*/
}

template<typename...BAs>
struct tau_ba {

	tau_ba(nso_rr<tau_ba<BAs...>, BAs...>& form) : form(form.main) {}
	tau_ba(wff<tau_ba<BAs...>, BAs...>& main) : form(main) {}

	auto operator<=>(const tau_ba<BAs...>& other) {
		return form <=> other.form;
	}

	bool operator==(const tau_ba<BAs...>& other) const {
		return form == other.form;
	}

	bool operator!=(const tau_ba<BAs...>& other) const {
		return form != other.form;
	}

	tau_ba<BAs...> operator~() const {
		auto nform = build_wff_neg<tau_ba<BAs...>, BAs...>(form);
		return tau_ba<BAs...>(nform);
	}

	tau_ba<BAs...> operator&(const tau_ba<BAs...>& other) const {
		auto nform = build_wff_and<tau_ba<BAs...>, BAs...>(form, other.form);
		return tau_ba<BAs...>(nform);
	}

	tau_ba<BAs...> operator|(const tau_ba<BAs...>& other) const {
		auto nform = build_wff_or<tau_ba<BAs...>, BAs...>(form, other.form);
		return tau_ba<BAs...>(nform);
	}

	tau_ba<BAs...> operator^(const tau_ba<BAs...>& other) const {
		auto nform = build_wff_xor<tau_ba<BAs...>, BAs...>(form, other.form);
		return tau_ba<BAs...>(nform);
	}

	tau_ba<BAs...> operator+(const tau_ba<BAs...>& other) const {
		auto nform = build_wff_xor<tau_ba<BAs...>, BAs...>(form, other.form);
		return tau_ba<BAs...>(nform);
	}

	bool is_zero() const {
		auto normalized = normalizer<tau_ba<BAs...>, BAs...>(form);
		return (normalized | tau_parser::wff_f).has_value();
	}

	bool is_one() const {
		auto normalized = normalizer<tau_ba<BAs...>, BAs...>(form);
		return (normalized | tau_parser::wff_t).has_value();
	}

	wff<tau_ba<BAs...>, BAs...> form;
};

// TODO (HIGH) give a proper implementation for <=>, == and != operators
template<typename...BAs>
auto operator<=>(const tau_ba<BAs...>& l, const tau_ba<BAs...>& r) {
	return std::addressof(l)<=>std::addressof(r);
}

template<typename...BAs>
bool operator==(const tau_ba<BAs...>& other, const bool& b) {
	auto normalized = normalizer<tau_ba<BAs...>, BAs...>(other.form).main;
	auto is_one = (normalized | tau_parser::wff_t).has_value();
	auto is_zero = (normalized | tau_parser::wff_f).has_value();
	return b ? is_one : is_zero ;
}

template<typename...BAs>
bool operator==(const bool& b, const tau_ba<BAs...>& other) {
	return other == b;
}

template<typename...BAs>
bool operator!=(const tau_ba<BAs...>& other, const bool& b) {
	return !(other == b);
}

template<typename...BAs>
bool operator!=(const bool& b, const tau_ba<BAs...>& other) {
	return !(other == b);
}

template<typename...BAs>
using tau_spec = wff<tau_ba<BAs...>, BAs...>;

template<typename base_factory_t, typename...BAs>
struct tau_factory {

	tau_factory(base_factory_t& bf) : bf(bf) {}

	sp_tau_node<tau_ba<BAs...>, BAs...> build(const std::string type_name, const sp_tau_node<tau_ba<BAs...>, BAs...>& n) {
		if (auto nn = bf.build(type_name, n); nn != n) return nn;
		auto source = n | tau_parser::source_binding | tau_parser::source | optional_value_extractor<sp_tau_node<tau_ba<BAs...>, BAs...>>;
		std::string var = idni::tau::make_string(idni::tau::tau_node_terminal_extractor<tau_ba<BAs...>, BAs...>, source);
		factory_binder<tau_factory<base_factory_t, BAs...>, tau_ba<BAs...>, BAs...> fb(*this);
		auto form = make_nso_rr_using_factory<factory_binder<tau_factory<base_factory_t, BAs...>, tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>(var, fb).main;
		tau_ba<BAs...> t(form);
		return make_node<tau_sym<tau_ba<BAs...>, BAs...>>(t, {});
	}

	base_factory_t& bf;
};

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
tau_spec<BAs...> make_tau_spec_using_binder(sp_tau_source_node& tau_source, binder_t& binder) {
	auto src = make_tau_code<tau_ba<BAs...>, BAs...>(tau_source);
	auto unbinded_form = src | tau_parser::gssotc | tau_parser::tau | optional_value_extractor<tau_spec<BAs...>>;
	auto binded_form = post_order_traverser<
			binder_t,
			all_t<tau_spec<BAs...>>,
			tau_spec<BAs...>>(
		binder, all<tau_spec<BAs...>>)(unbinded_form);
	return binded_form;
}

// make a nso_rr from the given tau source and bindings.
template<typename... BAs>
tau_spec<BAs...> make_tau_spec_using_bindings(sp_tau_source_node& tau_source, const bindings<BAs...>& bindings) {
	name_binder<tau_ba<BAs...>, BAs...> nb(bindings);
	bind_transformer<name_binder<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...> bs(nb);
	return make_tau_spec_using_binder<bind_transformer<name_binder<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>, BAs...>(tau_source, bs);
}

// make a nso_rr from the given tau source and bindings.
template<typename factory_t, typename... BAs>
tau_spec<BAs...> make_tau_spec_using_factory(sp_tau_source_node& tau_source, factory_t& factory) {
	bind_transformer<factory_t, tau_ba<BAs...>, BAs...> bs(factory);
	return make_tau_spec_using_binder<bind_transformer<factory_t, tau_ba<BAs...>,  BAs...>, BAs...>(tau_source, bs);
}

// make a nso_rr from the given tau source and bindings.
template<typename factory_t, typename... BAs>
tau_spec<BAs...> make_tau_spec_using_factory(const std::string& source, factory_t& factory) {
	auto tau_source = make_tau_source(source);
	return make_tau_spec_using_factory<factory_t, tau_ba<BAs...>, BAs...>(tau_source, factory);
}

// make a nso_rr from the given tau source and bindings.
template<typename... BAs>
tau_spec<BAs...> make_tau_spec_using_bindings(const std::string& source, const bindings<BAs...>& bindings) {
	auto tau_source = make_tau_source(source);
	name_binder<BAs...> nb(bindings);
	bind_transformer<name_binder<BAs...>, BAs...> bs(nb);
	return make_tau_spec_using_bindings<
			bind_transformer<name_binder<BAs...>, BAs...>,
			BAs...>(
		tau_source, bs);
}

template<typename... BAs>
void get_clauses(const tau_spec<BAs...>& n, std::vector<sp_tau_node<tau_ba<BAs...>, BAs...>>& clauses) {
	if (auto check = n | tau_parser::tau_or; !check.has_value() && is_non_terminal(tau_parser::tau, n)) clauses.push_back(n);
	else for (auto& c: n->child)
		if (is_non_terminal(tau_parser::tau_or, c)) get_clauses(c ,clauses);
}

template<typename... BAs>
std::vector<tau_spec<BAs...>> get_clauses(const tau_spec<BAs...>& n) {
	std::vector<tau_spec<BAs...>> clauses;
	get_clauses(n, clauses);

	#ifdef DEBUG
	std::cout << "(I) clauses: " ;
	for (auto& c: clauses) std::cout << c << ", ";
	std::cout << std::endl;
	#endif // DEBUG

	return clauses;
}

template<typename... BAs>
using extracted_bindings = std::map<std::variant<BAs...>, std::string>;

template<typename... BAs>
std::string clause_to_string(const tau_spec<BAs...>& clause,
		extracted_bindings<tau_ba<BAs...>, BAs...>& extracted_bindings,
		size_t& binding_seed) {
	std::basic_stringstream<char> str;

	auto visitor = overloaded(
		[&str] (const tau_source_sym& l) {
			if (!l.nt()) str << l.t(); },
		[&str, &extracted_bindings, &binding_seed] (const std::variant<tau_ba<BAs...>, BAs...>& bae) {
			if (extracted_bindings.contains(bae)) str << extracted_bindings[bae];
			else {
				auto binding = "binding" + std::to_string(binding_seed++);
				extracted_bindings[bae] = binding;
				str << binding;
			}},
		[&str] (const size_t& n) { str << n; });

	auto print = [&str, &visitor] (const auto& e) {
		std::visit(visitor, e->value);
		return e;
	};
	using print_t = decltype(print);

	post_order_tree_traverser<
			print_t,
			all_t<sp_tau_node<tau_ba<BAs...>, BAs...>>,
			sp_tau_node<tau_ba<BAs...>, BAs...>>(
		print, all<sp_tau_node<tau_ba<BAs...>, BAs...>>)(clause);

	return str.str();
}

template<typename... BAs>
void get_positive_and_negative_literals(const tau_spec<BAs...> collapsed,
		std::optional<wff<tau_ba<BAs...>, BAs...>>& positive, std::vector<wff<tau_ba<BAs...>, BAs...>>& negatives) {
	auto is_negative = [] (const auto& n) {
		auto check = n | tau_parser::tau | tau_parser::tau_neg | tau_parser::wff;
		return check.has_value();
	};
	auto is_positive = [] (const auto& n) {
		auto check = n | tau_parser::tau | tau_parser::wff;
		return !check.has_value();
	};
	negatives = select_top(collapsed, is_negative);
	positive = find_top(collapsed, is_positive);

	#ifdef DEBUG
	if (positive.has_value()) std::cout << "(I) positive: " << positive.value() << std::endl;
	if (!negatives.empty()) {
		std::cout << "(I) negatives: ";
		for (auto& n: negatives) std::cout << n << ", ";
		std::cout << std::endl;
	}
	#endif // DEBUG
}

template<typename... BAs>
std::pair<std::optional<tau_spec<BAs...>>, std::vector<tau_spec<BAs...>>> get_positive_and_negative_literals(
		const tau_spec<BAs...> collapsed) {
	std::optional<wff<tau_ba<BAs...>, BAs...>> positive;
	std::vector<wff<tau_ba<BAs...>, BAs...>> negatives;
	get_positive_and_negative_literals(collapsed, positive, negatives);
	return {positive, negatives};
}

template<typename...BAs>
struct tau_spec_vars {

	void add(const sp_tau_node<tau_ba<BAs...>, BAs...>& io) {
		auto pos = io
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| tau_parser::var_pos
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| non_terminal_extractor<tau_ba<BAs...>, BAs...>
			| optional_value_extractor<size_t>;
		auto var_name = (io
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| optional_value_extractor<sp_tau_node<tau_ba<BAs...>, BAs...>>)->child[0];
		switch (pos) {
			case tau_parser::current_pos:
				name.emplace(var_name);
				break;
			case tau_parser::absolute_pos:
				name.emplace(var_name);
				loopback = max(loopback,
					io | only_child_extractor<tau_ba<BAs...>, BAs...>
					| tau_parser::var_pos
					| tau_parser::absolute_pos
					| tau_parser::num
					| only_child_extractor<tau_ba<BAs...>, BAs...>
					| offset_extractor<tau_ba<BAs...>, BAs...>
					| optional_value_extractor<size_t>);
				break;
			case tau_parser::relative_pos:
				name.emplace(var_name);
				loopback = max(loopback,
					io | only_child_extractor<tau_ba<BAs...>, BAs...>
					| tau_parser::var_pos
					| tau_parser::relative_pos
					| tau_parser::num
					| only_child_extractor<tau_ba<BAs...>, BAs...>
					| offset_extractor<tau_ba<BAs...>, BAs...>
					| optional_value_extractor<size_t>);
				break;
			default:
				throw std::logic_error("bad timed pos");
		}
	}

	std::set<wff<tau_ba<BAs...>, BAs...>> name;
	size_t loopback = 0;
};

template<typename... BAs>
std::pair<tau_spec_vars<BAs...>, tau_spec_vars<BAs...>> get_io_vars(const tau_spec<BAs...> collapsed) {
	tau_spec_vars<BAs...> inputs;
	tau_spec_vars<BAs...> outputs;
	for (const auto& variable: select_top(collapsed, is_non_terminal<tau_parser::io_var, tau_ba<BAs...>, BAs...>)) {
		auto type = variable
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| non_terminal_extractor<tau_ba<BAs...>, BAs...>
			| optional_value_extractor<size_t>;
		(type == tau_parser::in ? inputs : outputs).add(variable);
	}

	#ifdef DEBUG
	std::cout << "(I) inputs: " ; for (auto& i: inputs.name) std::cout << i << ", "; std::cout << std::endl;
	std::cout << "(I) outputs: " ; for (auto& o: outputs.name) std::cout << o << ", "; std::cout << std::endl;
	#endif // DEBUG

	return {inputs, outputs};
}

template<typename... BAs>
std::pair<std::string, extracted_bindings<tau_ba<BAs...>, BAs...>>  get_eta_nso_rr(
		const std::optional<wff<tau_ba<BAs...>, BAs...>>& positive,
		const std::vector<wff<tau_ba<BAs...>, BAs...>>& negatives,
		const tau_spec_vars<BAs...>& inputs,
		const tau_spec_vars<BAs...>& outputs) {
	auto print_vars = [] (const auto& vars) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << var;
			if (var != *vars.name.rbegin()) str << ", ";
		}
		return str.str();
	};
	auto universally_quantify_vars = [] (const auto& vars, size_t loopback) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << "all " << var << "[" << loopback << "] ";
		}
		return str.str();
	};
	auto existentially_quantify_vars = [] (const auto& vars, size_t loopback) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << "ex " << var << "[" << loopback << "] ";
		}
		return str.str();
	};
	auto print_boolean_existential_quantifiers = [] (size_t negatives) {
		std::basic_stringstream<char> str;
		for (size_t i = 1; i <= negatives; ++i) {
			str << "bex $nn" << i << " ";
			if (i < negatives) str << ",";
		}
		return str.str();
	};
	auto print_t = [] (size_t negatives) {
		std::basic_stringstream<char> str;
		for (size_t i = 1; i <= negatives; ++i) {
			str << "t";
			if (i < negatives) str << ",";
		}
		return str.str();
	};
	auto print_negative_vars = [] (size_t negatives) {
		std::basic_stringstream<char> str;
		for (size_t i = 1; i <= negatives; ++i) {
			str << "$n" << i;
			if (i < negatives) str << ",";
		}
		return str.str();
	};
	auto print_new_negative_vars = [] (size_t negatives) {
		std::basic_stringstream<char> str;
		for (size_t i = 1; i <= negatives; ++i) {
			str << "$nn" << i;
			if (i < negatives) str << ",";
		}
		return str.str();
	};

	extracted_bindings<tau_ba<BAs...>, BAs...> bindings;
	size_t binding_seed = 0;
	size_t loopback = max(inputs.loopback, outputs.loopback);
	std::basic_stringstream<char> nsorr;
	nsorr << "eta[t](" << print_vars(outputs) << ") := ";
		// add call to etas
		if (negatives.size() > 0) {
			nsorr << "etas[t](" << print_vars(outputs) << print_t(outputs.name.size()) << ").\n";
		}
	nsorr << "etas[t](" << print_vars(outputs) << print_negative_vars(outputs.name.size()) <<") := ";
		// quantify input and output variables
		for (size_t i = 1; i <= loopback; ++i) {
			nsorr << universally_quantify_vars(inputs, i);
			nsorr << existentially_quantify_vars(outputs, i);
		}
		// boolean quantify new negative vars
		nsorr << print_boolean_existential_quantifiers(outputs.name.size());
		// add positive literal if it exists
		if (positive.has_value()) {
			nsorr << clause_to_string(positive.value(), bindings, binding_seed);
			if (negatives.size() > 0) nsorr << " && ";
		}
		// generate a disjunction of all negations conjucted with the max_loopback variables and
		if (negatives.size() > 0) {
			for (size_t i = 0; i < negatives.size(); ++i) {
				nsorr << "(( $n" << i << " && " << "$nn" << i << " ) &&& " << clause_to_string(negatives[i], bindings, binding_seed) << ") &&& ";
			}
			// add recursive call with t-1
			nsorr << "etas[t-1](" << print_vars(outputs) << print_new_negative_vars(outputs.name.size()) << ").\n";
		}
	// add main

	#ifdef DEBUG
	std::cout << "(I) get_eta_nso_rr: " << nsorr.str() << std::endl;
	#endif // DEBUG

	return {nsorr.str(), bindings};
}

template<typename... BAs>
std::string get_check_nso_rr(const tau_spec_vars<BAs...>& outputs, size_t loopback, size_t current) {
	auto print_vars = [] (const auto& vars) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << var;
			if (var != *vars.name.rbegin()) str << ", ";
		}
		return str.str();
	};
	auto existentially_quantify_vars = [] (const auto& vars, size_t loopback) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << "ex " << var << "[" << loopback << "] ";
		}
		return str.str();
	};
	std::basic_stringstream<char> check;
	check << existentially_quantify_vars(outputs, loopback) <<
		" (eta[" << current << "](" << print_vars(outputs) << ").\n";

	#ifdef DEBUG
	std::cout << "(I) get_check_nso_rr: " << check.str() << std::endl;
	#endif // DEBUG

	return check.str();
}

template<typename... BAs>
std::string get_main_nso_rr(const tau_spec_vars<BAs...>& outputs, size_t loopback, size_t current, size_t previous) {
	auto print_vars = [] (const auto& vars) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << var;
			if (var != *vars.name.rbegin()) str << ", ";
		}
		return str.str();
	};
	auto universally_quantify_vars = [] (const auto& vars, size_t loopback) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << "all " << var << "[" << loopback << "] ";
		}
		return str.str();
	};
	std::basic_stringstream<char> main;
	main << universally_quantify_vars(outputs, loopback) <<
		" (eta[" << current << "](" << print_vars(outputs) << ") <-> eta[" << previous << "]).\n";

	#ifdef DEBUG
	std::cout << "(I) get_main_nso_rr: " << main.str() << std::endl;
	#endif // DEBUG

	return main.str();
}


template<typename... BAs>
bool is_satisfiable_clause(const tau_spec<BAs...>& clause) {

	#ifdef DEBUG
	std::cout << "(I) is_satisfiable_clause: " << clause << std::endl;
	#endif // DEBUG

	auto collapsed = clause |
		repeat_all<step<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>(
			simplify_tau<tau_ba<BAs...>, BAs...>
			| collapse_positives_tau<tau_ba<BAs...>, BAs...>);

	auto [positive, negatives] = get_positive_and_negative_literals(collapsed);
	auto [inputs, outputs] = get_io_vars(collapsed);
	size_t loopback = max(inputs.loopback, outputs.loopback);

	if (inputs.name.empty() && outputs.name.empty()) {
		auto check = clause | tau_parser::wff | tau_parser::wff_t;
		return check.has_value() ? true : false;
	}

	auto etas = get_eta_nso_rr<BAs...>(positive, negatives, inputs, outputs);
	for (size_t current = 1; /* until return statement */ ; ++current) {
		auto [eta, extracted_bindings] = get_eta_nso_rr<BAs...>(positive, negatives, inputs, outputs);
		bindings<tau_ba<BAs...>, BAs...> reversed_bindings; // TODO (LOW) reverse
		auto check = get_check_nso_rr(outputs, loopback, current);
		auto normalize = normalizer<tau_ba<BAs...>, BAs...>(eta.append(check), reversed_bindings).main;
		if ((normalize | tau_parser::wff_f).has_value()) {
			#ifdef DEBUG
			std::cout << "(I) is_satisfiable_clause: false" << std::endl;
			#endif // DEBUG

			return false;
		}
		for (size_t previous = 1; previous < current; ++previous) {
			auto main = get_main_nso_rr(outputs, loopback, current, previous);
			auto normalize = normalizer<tau_ba<BAs...>, BAs...>(eta.append(main), reversed_bindings).main;
			if ((normalize | tau_parser::wff_t).has_value()) return true;
		}
	}
}

// check satisfability of a tau_spec (boolean combination case)
template<typename...BAs>
bool is_satisfiable(const tau_spec<BAs...>& tau_spec) {
	#ifdef DEBUG
	std::cout << "(I) is_satisfiable: " << tau_spec << std::endl;
	#endif // DEBUG

	auto dnf = tau_spec |
		repeat_all<step<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>(
			to_dnf_tau<tau_ba<BAs...>, BAs...>
			| simplify_tau<tau_ba<BAs...>, BAs...>
		);
	for(auto& clause: get_clauses(dnf)) {
		if (is_satisfiable_clause(clause)) {

			#ifdef DEBUG
			std::cout << "(I) is_satisfiable: true" << std::endl;
			#endif // DEBUG

			return true;
		}
	}

	#ifdef DEBUG
	std::cout << "(I) is_satisfiable: false" << std::endl;
	#endif // DEBUG

	return false;
}

} // namespace idni::tau

#endif // __TAU_H__