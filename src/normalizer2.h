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

#ifndef __NORMALIZER2_H__
#define __NORMALIZER2_H__

#include <string>
#include <optional>

#include "rewriting.h"
#include "formula.h"

namespace idni::tau {

// tau system library, used to define the tau system of rewriting rules
#define RULE(name, code) const std::string name = code;

// IDEA (MEDIUM) add commutative rule and halve the number of rules if is performance friendly

// bf rules 
RULE(BF_SIMPLIFY_ONE_0, "( T bf_or $X ) := T.")
RULE(BF_SIMPLIFY_ONE_1, "( $X bf_or T ) := T.")
RULE(BF_SIMPLIFY_ONE_2, "( T bf_and $X ) := $X.")
RULE(BF_SIMPLIFY_ONE_3, "( $X bf_and T ) := $X.")
RULE(BF_SIMPLIFY_ZERO_0, "( F bf_and $X ) := F.")
RULE(BF_SIMPLIFY_ZERO_1, "( $X bf_and F ) := F.")
RULE(BF_SIMPLIFY_ZERO_2, "( F bf_or $X ) := $X.")
RULE(BF_SIMPLIFY_ZERO_3, "( $X bf_or F ) := $X.")
RULE(BF_SIMPLIFY_SELF_0, "( $X bf_and $X ) := $X.")
RULE(BF_SIMPLIFY_SELF_1, "( $X bf_or $X ) := $X.")
RULE(BF_SIMPLIFY_SELF_2, "( $X bf_and bf_neg $X ) := F.")
RULE(BF_SIMPLIFY_SELF_3, "( $X bf_or bf_neg $X ) := T.")
RULE(BF_SIMPLIFY_SELF_4, "( bf_neg $X bf_and $X ) := F.")
RULE(BF_SIMPLIFY_SELF_5, "( bf_neg $X bf_or $X ) := T.")

RULE(BF_FUNCTIONAL_QUANTIFIERS_0, "bf_all $X $Y := ( bf_subs_cb $X F $Y bf_and bf_subs_cb $X T $Y).")
RULE(BF_FUNCTIONAL_QUANTIFIERS_1, "bf_ex $X $Y := ( bf_subs_cb $X F $Y bf_or bf_subs_cb $X T $Y).")
RULE(BF_SKIP_CONSTANTS_0, "({ $X } bf_and $Y) := ($Y bf_and { $X }).")
RULE(BF_ROTATE_LITERALS_0, "( $X bf_and ( $Y bf_and $Z ) ) := ( $Y bf_and ( $Z bf_and $X ) ).")
RULE(BF_ROTATE_LITERALS_1, "( ( $X bf_and $Y ) bf_and $Z ) := ( ( $Y bf_and $Z ) bf_and $X ).")

// bf callbacks
RULE(BF_CALLBACK_AND, "( { $X } bf_and { $Y } ) := { $X bf_and_cb $Y }.")
RULE(BF_CALLBACK_OR, "( { $X } bf_or { $Y } ) := { $X bf_or_cb $Y }.")
RULE(BF_CALLBACK_XOR, "( { $X } bf_xor { $Y } ) := { $X bf_xor_cb $Y }.")
RULE(BF_CALLBACK_NEG, "bf_neg { $X } := { bf_neg_cb $X }.")
RULE(BF_CALLBACK_LESS, "( { $X } bf_less { $Y } ) := bf_less_cb $X $Y T F.")
RULE(BF_CALLBACK_LESS_EQUAL, "( { $X } bf_less_equal { $Y } ) := bf_less_equal_cb $X $Y T F.")
RULE(BF_CALLBACK_GREATER, "( { $X } bf_greater { $Y } ) := bf_greater_cb $X $Y T F.")
RULE(BF_CALLBACK_EQ, "( { $X } == F ) := bf_eq_cb $X T F.")
RULE(BF_CALLBACK_NEQ, "( { $X } != F ) := bf_neq_cb $X T F.")
RULE(BF_CALLBACK_IS_ZERO, "{ $X } := bf_is_zero_cb { $X } T.")
RULE(BF_CALLBACK_IS_ONE, "{ $X } := bf_is_one_cb { $X } T.")

// cbf rules
RULE(CBF_DISTRIBUTE_0, "(($X cbf_or $Y) cbf_and $Z) := (($X cbf_and $Y) cbf_or ($X cbf_and $Z)).")
RULE(CBF_DISTRIBUTE_1, "($X cbf_and ($Y cbf_or $Z)) := (($X cbf_and $Y) cbf_or ($X cbf_and $Z)).")
RULE(CBF_PUSH_NEGATION_INWARDS_0, "cbf_neg ($X cbf_and $Y) := (cbf_neg $X cbf_or cbf_neg $Y).")
RULE(CBF_PUSH_NEGATION_INWARDS_1, "cbf_neg ($X cbf_or $Y) := (cbf_neg $X cbf_and cbf_neg $Y).")
RULE(CBF_ELIM_DOUBLE_NEGATION_0, "cbf_neg cbf_neg $X :=  $X.")
RULE(CBF_SIMPLIFY_ONE_0, "( T cbf_or $X ) := T.")
RULE(CBF_SIMPLIFY_ONE_1, "( $X cbf_or T ) := T.")
RULE(CBF_SIMPLIFY_ONE_2, "( T cbf_and $X ) := $X.")
RULE(CBF_SIMPLIFY_ONE_3, "( $X cbf_and T ) := $X.")
RULE(CBF_SIMPLIFY_ZERO_0, "( F cbf_and $X ) := F.")
RULE(CBF_SIMPLIFY_ZERO_1, "( $X cbf_and F ) := F.")
RULE(CBF_SIMPLIFY_ZERO_2, "( F cbf_or $X ) := $X.")
RULE(CBF_SIMPLIFY_ZERO_3, "( $X cbf_or F ) := $X.")
RULE(CBF_SIMPLIFY_SELF_0, "( $X cbf_and $X ) := $X.")
RULE(CBF_SIMPLIFY_SELF_1, "( $X cbf_or $X ) := $X.")
RULE(CBF_SIMPLIFY_SELF_2, "( $X cbf_and cbf_neg $X ) := F.")
RULE(CBF_SIMPLIFY_SELF_3, "( $X cbf_or cbf_neg $X ) := T.")
RULE(CBF_SIMPLIFY_SELF_4, "( cbf_neg $X cbf_and $X ) := F.")
RULE(CBF_SIMPLIFY_SELF_5, "( cbf_neg $X cbf_or $X ) := T.")

// cbf definitions of xor, ->, <- and <->.
RULE(CBF_DEF_XOR, "( $X cbf_xor $Y ) := (( $X cbf_and cbf_neg $Y ) cbf_or ( cbf_neg $X cbf_and $Y )).")
RULE(CBF_DEF_IMPLY, "( $X cbf_imply $Y ) := ( cbf_neg $X cbf_or $Y).")
RULE(CBF_DEF_COIMPLY, "( $X cbf_coimply $Y ) := ( $Y cbf_imply $X).")
RULE(CBF_DEF_EQUIV, "( $X cbf_equiv $Y ) := (( $X cbf_imply $Y ) cbf_and ( $Y cbf_imply $X )).")
RULE(CBF_DEF_IF, "if $X then $Y else $Z := (( $X wff_imply $Y ) wff_and ( wff_neg $X wff_imply $Z )).")

// wff rules
RULE(WFF_DISTRIBUTE_0, "(($X wff_or $Y) wff_and $Z) := (($X wff_and $Y) wff_or ($X wff_and $Z)).")
RULE(WFF_DISTRIBUTE_1, "($X wff_and ($Y wff_or $Z)) := (($X wff_and $Y) wff_or ($X wff_and $Z)).")
RULE(WFF_PUSH_NEGATION_INWARDS_0, "wff_neg ($X wff_and $Y) := (wff_neg $X wff_or wff_neg $Y).")
RULE(WFF_PUSH_NEGATION_INWARDS_1, "wff_neg ($X wff_or $Y) := (wff_neg $X wff_and wff_neg $Y).")
// TODO rename to WFF_ELIM_DOUBLE_NEGATION
RULE(WFF_ELIM_DOUBLE_NEGATION_0, "wff_neg wff_neg $X :=  $X.")
RULE(WFF_ELIM_FORALL, "wff_all $X $Y := wff_neg wff_ex $X wff_neg $Y.")
RULE(WFF_SIMPLIFY_ONE_0, "( T wff_or $X ) := T.")
RULE(WFF_SIMPLIFY_ONE_1, "( $X wff_or T ) := T.")
RULE(WFF_SIMPLIFY_ONE_2, "( T wff_and $X ) := $X.")
RULE(WFF_SIMPLIFY_ONE_3, "( $X wff_and T ) := $X.")
RULE(WFF_SIMPLIFY_ZERO_0, "( F wff_and $X ) := F.")
RULE(WFF_SIMPLIFY_ZERO_1, "( $X wff_and F ) := F.")
RULE(WFF_SIMPLIFY_ZERO_2, "( F wff_or $X ) := $X.")
RULE(WFF_SIMPLIFY_ZERO_3, "( $X wff_or F ) := $X.")
RULE(WFF_SIMPLIFY_SELF_0, "( $X wff_and $X ) := $X.")
RULE(WFF_SIMPLIFY_SELF_1, "( $X wff_or $X ) := $X.")
RULE(WFF_SIMPLIFY_SELF_2, "( $X wff_and wff_neg $X ) := F.")
RULE(WFF_SIMPLIFY_SELF_3, "( $X wff_or wff_neg $X ) := T.")
RULE(WFF_SIMPLIFY_SELF_4, "( wff_neg $X wff_and $X ) := F.")
RULE(WFF_SIMPLIFY_SELF_5, "( wff_neg $X wff_or $X ) := T.")

// wff definitions of xor, ->, <- and <->.
RULE(WFF_DEF_XOR, "( $X wff_xor $Y ) := (( $X wff_and wff_neg $Y ) wff_or ( wff_neg $X wff_and $Y )).")
RULE(WFF_DEF_IMPLY, "( $X wff_imply $Y ) := ( wff_neg $X wff_or $Y).")
RULE(WFF_DEF_COIMPLY, "( $X wff_coimply $Y ) := ( $Y wff_imply $X).")
RULE(WFF_DEF_EQUIV, "( $X wff_equiv $Y ) := (( $X wff_imply $Y ) wff_and ( $Y wff_imply $X )).")

RULE(BF_TRIVIALITY_0, "( F == F ) := T.")
RULE(BF_TRIVIALITY_1, "( T == F ) :=  F.")
RULE(BF_TRIVIALITY_2, "( F != F ) := F.")
RULE(BF_TRIVIALITY_3, "( T != F ) := T.")
RULE(BF_SQUEEZE_POSITIVES_0, "(( $X == F ) wff_and ($Y == F)) := (( $X bf_or $Y ) == F).")

// TODO (VERY HIGH) review this rule, something is wrong, check point (d) of the paper tauimpl1.pdf
// further processing (a + b := (a ∧ ¬b) ∨ (b ∧ ¬a) == (a ∨ b) ∧ ¬(a ∧ b))
// "( ($X bf_and $Y) == F ) wwf_and ( ($X bf_and $Z) != 0) == ( bf_all $X ( ( $X bf_and $Y$ ) == F )  wwf_and ( bf_ex $X ( ( $X bf_or ( $X bf_and $Y )) bf_and bf_neg ( $X bf_and ($X bf_and $Y ) ) wwf_and $Z )."
RULE(BF_PROCESS_0, "((($X bf_and $Y) == 0) wff_and (($X bf_and $Z) != 0)) := (bf_all $X ((($X bf_and $Y) == F )  wff_and ( bf_ex $X (( $X bf_or ( $X bf_and $Y )) bf_and bf_neg ($X bf_and $Y)) wff_and $Z )).")

// TODO (MEDIUM) delete trivial quantified formulas (i.e. ∀x. F == no_x..., ). 

// bf defs are just callbacks
template<typename... BAs>
static auto apply_defs = make_library<BAs...>(
	// wff defs
	WFF_DEF_XOR
	+ WFF_DEF_IMPLY
	+ WFF_DEF_COIMPLY
	+ WFF_DEF_EQUIV	
	// cbf defs
	+ CBF_DEF_XOR
	+ CBF_DEF_IMPLY
	+ CBF_DEF_COIMPLY
	+ CBF_DEF_EQUIV
	+ CBF_DEF_IF
);

template<typename... BAs>
static auto elim_for_all = make_library<BAs...>(
	WFF_ELIM_FORALL
);


template<typename... BAs>
static auto to_dnf_wff = make_library<BAs...>(
	WFF_DISTRIBUTE_0 
	+ WFF_DISTRIBUTE_1
	+ WFF_PUSH_NEGATION_INWARDS_0 
	+ WFF_PUSH_NEGATION_INWARDS_1 
	+ WFF_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto to_dnf_cbf = make_library<BAs...>(
	CBF_DISTRIBUTE_0 
	+ CBF_DISTRIBUTE_1
	+ CBF_PUSH_NEGATION_INWARDS_0 
	+ CBF_PUSH_NEGATION_INWARDS_1 
	+ CBF_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto simplify_bf = make_library<BAs...>(
	BF_SIMPLIFY_ONE_0 
	+ BF_SIMPLIFY_ONE_1 
	+ BF_SIMPLIFY_ONE_2 
	+ BF_SIMPLIFY_ONE_3
	+ BF_SIMPLIFY_ZERO_0 
	+ BF_SIMPLIFY_ZERO_1 
	+ BF_SIMPLIFY_ZERO_2 
	+ BF_SIMPLIFY_ZERO_3
	+ BF_SIMPLIFY_SELF_0 
	+ BF_SIMPLIFY_SELF_1 
	+ BF_SIMPLIFY_SELF_2 
	+ BF_SIMPLIFY_SELF_3
	+ BF_SIMPLIFY_SELF_4 
	+ BF_SIMPLIFY_SELF_5
);

template<typename... BAs>
static auto simplify_wff = make_library<BAs...>(
	WFF_SIMPLIFY_ONE_0 
	+ WFF_SIMPLIFY_ONE_1 
	+ WFF_SIMPLIFY_ONE_2 
	+ WFF_SIMPLIFY_ONE_3
	+ WFF_SIMPLIFY_ZERO_0 
	+ WFF_SIMPLIFY_ZERO_1 
	+ WFF_SIMPLIFY_ZERO_2 
	+ WFF_SIMPLIFY_ZERO_3
	+ WFF_SIMPLIFY_SELF_0 
	+ WFF_SIMPLIFY_SELF_1 
	+ WFF_SIMPLIFY_SELF_2 
	+ WFF_SIMPLIFY_SELF_3
	+ WFF_SIMPLIFY_SELF_4 
	+ WFF_SIMPLIFY_SELF_5
);

template<typename... BAs>
static auto simplify_cbf = make_library<BAs...>(
	CBF_SIMPLIFY_ONE_0 
	+ CBF_SIMPLIFY_ONE_1 
	+ CBF_SIMPLIFY_ONE_2 
	+ CBF_SIMPLIFY_ONE_3
	+ CBF_SIMPLIFY_ZERO_0 
	+ CBF_SIMPLIFY_ZERO_1 
	+ CBF_SIMPLIFY_ZERO_2 
	+ CBF_SIMPLIFY_ZERO_3
	+ CBF_SIMPLIFY_SELF_0 
	+ CBF_SIMPLIFY_SELF_1 
	+ CBF_SIMPLIFY_SELF_2 
	+ CBF_SIMPLIFY_SELF_3
	+ CBF_SIMPLIFY_SELF_4 
	+ CBF_SIMPLIFY_SELF_5
);

template<typename... BAs>
static auto apply_cb = make_library<BAs...>(
	BF_CALLBACK_AND 
	+ BF_CALLBACK_OR
	+ BF_CALLBACK_XOR 
	+ BF_CALLBACK_NEG 
	+ BF_CALLBACK_LESS
	+ BF_CALLBACK_LESS_EQUAL
	+ BF_CALLBACK_GREATER
	+ BF_CALLBACK_EQ
 	+ BF_CALLBACK_NEQ
);

template<typename... BAs>
static auto wff_reduce = make_library<BAs...>(
	BF_FUNCTIONAL_QUANTIFIERS_0 
	+ BF_FUNCTIONAL_QUANTIFIERS_1 
	// TODO (HIGH) fix BF_PROCESS_0, it is not working
	+ BF_PROCESS_0 
	+ BF_SKIP_CONSTANTS_0 
	+ BF_ROTATE_LITERALS_0	
	+ BF_ROTATE_LITERALS_1
	+ BF_SQUEEZE_POSITIVES_0
);

template<typename... BAs>
static auto trivialities = make_library<BAs...>(
	BF_TRIVIALITY_0 
	+ BF_TRIVIALITY_1 
	+ BF_TRIVIALITY_2 
	+ BF_TRIVIALITY_3
);

template<typename...BAs>
struct steps {
	steps(std::vector<library<BAs...>>& libraries) : libraries(libraries) {}

	sp_tau_node<BAs...>& operator()(sp_tau_node<BAs...>& n) {
		auto nn = n;
		for (auto& lib : libraries) nn = tau_apply(lib, nn);
		return nn;
	}

	std::vector<rules<BAs...>>& libraries;
};

template<typename... BAs>
struct repeat_each {
	
	repeat_each (steps<BAs...>& substeps) : substeps(substeps) {}

	sp_tau_node<BAs...>& operator()(sp_tau_node<BAs...>& n) {
		auto nn = n;
		for (auto& lib: substeps.libraries) {
			std::set<sp_tau_node<BAs...>> visited;
			while (true) {
				nn = tau_apply(lib, nn);
				if (visited.find(nn) != visited.end()) break;
				visited.insert(nn);
			}
		}
		return nn;
	}

	steps<rules<BAs...>>& substeps;
};

template<typename step_t, typename... BAs>
struct repeat_all {
	
	repeat_all (steps<BAs...>& substeps) : substeps(substeps) {}

	sp_tau_node<BAs...>& operator()(sp_tau_node<BAs...>& n) {
		auto nn = n;
		std::set<sp_tau_node<BAs...>> visited;
		while (true) {
			for (auto& lib : substeps.libraries) nn = tau_apply(lib, nn);
			if (visited.find(nn) != visited.end()) break;
			visited.insert(nn);
		}
		return nn;
	}

	steps<rules<BAs...>>& substeps;
};

template<typename step_t, typename... BAs>
struct repeat {
	
	repeat(rules<BAs...>& step) : step(step) {}

	sp_tau_node<BAs...>& operator()(sp_tau_node<BAs...>& n) {
		auto nn = n;
		std::set<sp_tau_node<BAs...>> visited;
		while (true) {
			tau_apply(step, nn);
			if (visited.find(nn) != visited.end()) break;
			visited.insert(nn);
		}
		return nn;
	}

	rules<BAs...>& step;
};


template<typename... BAs>
steps<BAs...> operator|(library<BAs...>& l, library<BAs...>& r) {
	return steps({l, r});
}

template<typename... BAs>
steps<BAs...> operator|(steps<BAs...>& s, library<BAs...>& l) {
	s.libraries.push_back(l);
	return s;
}

template<typename... BAs>
sp_tau_node<BAs...> operator|(sp_tau_node<BAs...>& n, steps<BAs...>& s) {
	return s(n);
}

template<typename... BAs>
sp_tau_node<BAs...> operator|(sp_tau_node<BAs...>& n, repeat_all<BAs...>& r) {
	return r(n);
}

template<typename... BAs>
sp_tau_node<BAs...> operator|(sp_tau_node<BAs...>& n, repeat_each<BAs...>& r) {
	return r(n);
}

template <typename... BAs>
formula<BAs...> normalizer_step(formula<BAs...> form) {
	// each bunch of rules whould be applied till no more changes are made, then we 
	// apply the next set of rules in the vector till no further changes and so on,...
	// the API would provide a method to execute the rules accodingly.
	return { 
		form.rec_relations, 
		form.main
			| form.rec_relations
			| apply_defs<BAs...>
			| repeat(elim_for_all<BAs...>)
			| repeat_all(to_dnf_wff<BAs...>)
			| repeat_each<BAs...>(
				simplify_bf<BAs...> 
				| apply_cb<BAs...>
				| to_dnf_cbf<BAs...> 
				| simplify_cbf<BAs...> 
				| apply_cb<BAs...>
				| to_dnf_wff<BAs...> 
				| wff_reduce<BAs...> 
				| trivialities<BAs...>)
	};
}

// REVIEW could we assume we are working with the product algebra?

// this should be used in conjuction with std::set. it must provide
// a strict weak ordering in such a way that equivalent formulas are
// considered equal.

// executes the normalizer on the given source code taking into account the
// bindings provided.
template<typename... BAs>
formula<BAs...> normalizer(std::string source, bindings<BAs...> binds) {
	auto form_source = make_tau_source(source);
	auto form = make_formula_using_bindings(form_source, binds);
	return normalizer(form);
}

// executes the normalizer on the given source code taking into account the
// provided factory.
template<typename factory_t, typename... BAs>
formula<BAs...> normalizer(std::string source, factory_t factory) {
	auto form_source = make_tau_source(source);
	auto form = make_formula_using_factory(form_source, factory);
	return normalizer(form);
}

template <typename... BAs>
formula<BAs...> normalizer(formula<BAs...> form) {
	std::set<formula<BAs...>> previous;
	previous.insert(form);
	auto current = form;
	auto next = normalizer_step(current);
	while (!previous.insert(next).second) {
		current = next;
		next = normalizer_step(current);
	}
	return current;
}

} // namespace idni::tau

#endif // __NORMALIZER2_H__
