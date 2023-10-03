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

// bf rules
RULE(BF_ELIM_FORALL, "bf_all $X $Y := bf_neg bf_ex $X bf_neg $Y.")
RULE(BF_DISTRIBUTE_0, "(($X bf_or $Y) bf_and $Z) := (($X bf_and $Y) bf_or ($X bf_and $Z)).")
RULE(BF_DISTRIBUTE_1, "($X bf_and ($Y bf_or $Z)) := (($X bf_and $Y) bf_or ($X bf_and $Z)).")
RULE(BF_PUSH_NEGATION_INWARDS_0, "bf_neg ($X bf_and $Y) := (bf_neg $X bf_or bf_neg $Y).")
RULE(BF_PUSH_NEGATION_INWARDS_1, "bf_neg ($X bf_or $Y) := (bf_neg $X bf_and bf_neg $Y).")
RULE(BF_ELIM_DOUBLE_NEGATION_0, "bf_neg bf_neg $X :=  $X.")
RULE(BF_SIMPLIFY_ONE_0, "( 1 bf_or $X ) := 1.")
RULE(BF_SIMPLIFY_ONE_1, "( $X bf_or 1 ) := 1.")
RULE(BF_SIMPLIFY_ONE_2, "( 1 bf_and $X ) := $X.")
RULE(BF_SIMPLIFY_ONE_3, "( $X bf_and 1 ) := $X.")
RULE(BF_SIMPLIFY_ZERO_0, "( 0 bf_and $X ) := 0.")
RULE(BF_SIMPLIFY_ZERO_1, "( $X bf_and 0 ) := 0.")
RULE(BF_SIMPLIFY_ZERO_2, "( 0 bf_or $X ) := $X.")
RULE(BF_SIMPLIFY_ZERO_3, "( $X bf_or 0 ) := $X.")
RULE(BF_SIMPLIFY_SELF_0, "( $X bf_and $X ) := $X.")
RULE(BF_SIMPLIFY_SELF_1, "( $X bf_or $X ) := $X.")
RULE(BF_SIMPLIFY_SELF_2, "( $X bf_and bf_neg $X ) := 0.")
RULE(BF_SIMPLIFY_SELF_3, "( $X bf_or bf_neg $X ) := 1.")
RULE(BF_SIMPLIFY_SELF_4, "( bf_neg $X bf_and $X ) := 0.")
RULE(BF_SIMPLIFY_SELF_5, "( bf_neg $X bf_or $X ) := 1.")
RULE(BF_CALLBACK_0, "( { $X } bf_or { $Y } ) := { $X bf_or_cb $Y }.")
RULE(BF_CALLBACK_1, "( { $X } bf_and { $Y } ) := { $X bf_and_cb $Y }.")
RULE(BF_CALLBACK_2, "( { $X } bf_xor { $Y } ) := { $X bf_xor_cb $Y }.")
RULE(BF_CALLBACK_3, "bf_neg { $X }  := { bf_neg_cb $X }.")
// TODO (HIGH) check if the following rules are correct
RULE(BF_FUNCTIONAL_QUANTIFIERS_0, "bf_all $X $Y := ( bf_subs_cb $X 0 $Y bf_and bf_subs_cb $X 1 $Y).")
RULE(BF_FUNCTIONAL_QUANTIFIERS_1, "bf_ex $X $Y := ( bf_subs_cb $X 0 $Y bf_or bf_subs_cb $X 1 $Y).")
RULE(BF_SKIP_CONSTANTS_0, "({ $X } bf_and $Y) := ($Y bf_and { $X }).")
RULE(BF_ROTATE_LITERALS_0, "( $X bf_and ( $Y bf_and $Z ) ) := ( $Y bf_and ( $Z bf_and $X ) ).")
RULE(BF_ROTATE_LITERALS_1, "( ( $X bf_and $Y ) bf_and $Z ) := ( ( $Y bf_and $Z ) bf_and $X ).")

// wff rules
RULE(BF_TRIVIALITY_0, "( 0 = 0 ) := 1.")
RULE(BF_TRIVIALITY_1, "( 1 = 0 ) :=  0.")
RULE(BF_TRIVIALITY_2, "( 0 != 0 ) := 0.")
RULE(BF_TRIVIALITY_3, "( 1 != 0 ) := 1.")
RULE(BF_SQUEEZE_POSITIVES_0, "(( $X = 0 ) wff_and ($Y = 0)) := (( $X bf_or $Y ) = 0).")
// TODO (HIGH) review this rule, something is wrong, check point (d) of the paper tauimpl1.pdf
// further processing (a + b := (a ∧ ¬b) ∨ (b ∧ ¬a) = (a ∨ b) ∧ ¬(a ∧ b))
// "( ($X bf_and $Y) = 0 ) wwf_and ( ($X bf_and $Z) != 0) = ( bf_all $X ( ( $X bf_and $Y$ ) = 0 )  wwf_and ( bf_ex $X ( ( $X bf_or ( $X bf_and $Y )) bf_and bf_neg ( $X bf_and ($X bf_and $Y ) ) wwf_and $Z )."
RULE(BF_PROCESS_0, "((($X bf_and $Y) = 0) wff_and (($X bf_and $Z) != 0)) := (bf_all $X ((($X bf_and $Y) = 0 )  wff_and ( bf_ex $X (( $X bf_or ( $X bf_and $Y )) bf_and bf_neg ($X bf_and $Y)) wff_and $Z )).")

// TODO (HIGH) define rules for wwf and cbf

// rules to be used during normalization, the order matters.
//
// TODO (HIGH) add the rules for wff and cbf when ready.
const std::string system = 
	BF_ELIM_FORALL 
	+ BF_DISTRIBUTE_0 
	+ BF_DISTRIBUTE_1
	+ BF_PUSH_NEGATION_INWARDS_0 
	+ BF_PUSH_NEGATION_INWARDS_1 
	+ BF_ELIM_DOUBLE_NEGATION_0
	+ BF_SIMPLIFY_ONE_0 
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
	+ BF_CALLBACK_0 
	+ BF_CALLBACK_1 
	+ BF_CALLBACK_2 
	+ BF_CALLBACK_3 
	+ BF_SQUEEZE_POSITIVES_0 
	+ BF_FUNCTIONAL_QUANTIFIERS_0 
	+ BF_FUNCTIONAL_QUANTIFIERS_1 
	+ BF_PROCESS_0 
	+ BF_SKIP_CONSTANTS_0 
	+ BF_ROTATE_LITERALS_0	
	+ BF_ROTATE_LITERALS_1 
	+ BF_TRIVIALITY_0 
	+ BF_TRIVIALITY_1 
	+ BF_TRIVIALITY_2 
	+ BF_TRIVIALITY_3;

// CHECK could we assume we are working with the product algebra?
// this should be used in conjuction with std::set. it must provide
// a strict weak ordering in such a way that equivalent formulas are
// considered equal.
// TODO once a final implementation is done, check the comments.
template<typename... BAs>
struct prog_less {

	bool operator()(const formula<BAs...>& p1, const formula<BAs...>& p2) const {
		auto m1 = extract_cte(p1.main);
		auto m2 = extract_cte(p2.main);
		// CHECK Could we assume we have a partial order on the algebra induced by
		// the operations of the algebra (i.e. a ≤ b iff a = b ∧ a )?
		// CHECK Do we need < or ≤ is enough for working?
		return m1 < m2;
	}
private:
	sp_tau_node<BAs...> extract_cte(sp_tau_node<BAs...> n) const {
		auto cte = find_top(n, is_non_terminal<tau_parser::bf_constant, BAs...>).value()[1];
		if (cte->value.index() == 0) return cte->child[0]->value;
		return cte->value;
	}
};

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
	auto sys_source = make_tau_source(system);
	auto lib = make_library<BAs...>(sys_source);
	return normalizer(form, lib);
}

// TODO (HIGH) tweak the execution of the system rules, maybe we can do it in a more
// efficient way.

// execute one step of the formula
template<typename... BAs>
formula<BAs...> program_step(const formula<BAs...>& form, const library<BAs...>& lib) {
	auto main = form.main;
	auto main_after_prog = apply(form.rec_relations, main);
	// TODO sys rules should be applied while possible
	auto main_after_sys = tau_apply(lib.system, main_after_prog);
	return formula<BAs...>(main_after_sys, form.rec_relations);
}

template <typename... BAs>
formula<BAs...> normalizer(formula<BAs...> form, library<BAs...> lib) {
	std::set<formula<BAs...>, prog_less<BAs...>> previous;
	previous.insert(form);
	auto current = form;
	auto next = program_step(current, lib);
	while (!previous.insert(next).second) {
		current = next;
		next = program_step(current, lib);
	}
	return current;
}

} // namespace idni::tau

#endif // __NORMALIZER2_H__
