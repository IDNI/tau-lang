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
#include "program.h"

namespace idni::tau {

// tau system library, used to define the tau system of rewriting rules
#define RULE(name, code) const std::string name = code;
RULE(BF_ELIM_FORALL, "bf_all $X ($Y) = bf_neg (bf_ex $X (bf_neg ($Y))).")
RULE(BF_DISTRIBUTE_0, "($X bf_or $Y) bf_and $Z = ($X bf_and $Y) bf_or ($X bf_and $Z)).")
RULE(BF_DISTRIBUTE_1, "$X bf_and ($Y bf_or $Z) = ($X bf_and $Y) bf_or ($X bf_and $Z)).")
RULE(BF_PUSH_NEGATION_INWARDS_0, "bf_neg ($X bf_and $Y) = (bf_neg $X) bf_or (bf_neg $Y).")
RULE(BF_PUSH_NEGATION_INWARDS_1, "bf_neg ($X bf_or $Y) = (bf_neg $X) bf_and (bf_neg $Y).")
RULE(BF_ELIM_DOUBLE_NEGATION_0, "bf_neg (bf_neg $X) =  $X.")
RULE(BF_SIMPLIFY_ONE_0, "( { 1 } bf_or $X ) = { 1 }.")
RULE(BF_SIMPLIFY_ONE_1, "( $X bf_or { 1 } ) = { 1 }.")
RULE(BF_SIMPLIFY_ONE_2, "( { 1 } bf_and $X ) = $X.")
RULE(BF_SIMPLIFY_ONE_3, "( $X bf_and { 1 } ) = $X.")
RULE(BF_SIMPLIFY_ZERO_0, "( { 0 } bf_and $X ) = { 0 }.")
RULE(BF_SIMPLIFY_ZERO_1, "( $X bf_and { 0 } ) = { 0 }.")
RULE(BF_SIMPLIFY_ZERO_2, "( { 0 } bf_or $X ) = $X.")
RULE(BF_SIMPLIFY_ZERO_3, "( $X bf_or { 0 } ) = $X.")
RULE(BF_SIMPLIFY_SELF_0, "( $X bf_and $X ) = $X.")
RULE(BF_SIMPLIFY_SELF_1, "( $X bf_or $X ) = $X.")
RULE(BF_SIMPLIFY_SELF_2, "( $X bf_and (bf_neg $X) ) = { 0 }.")
RULE(BF_SIMPLIFY_SELF_3, "( $X bf_or (bf_neg $X) ) = { 1 }.")
RULE(BF_SIMPLIFY_SELF_4, "( (bf_neg $X) bf_and $X ) = { 0 }.")
RULE(BF_SIMPLIFY_SELF_5, "( (bf_neg $X) bf_or $X ) = { 1 }.")
RULE(BF_CALLBACK_0, "( { $X } bf_or { $Y } ) = { $X bf_or_bltin $Y }.")
RULE(BF_CALLBACK_1, "( { $X } bf_and { $Y } ) = { $X bf_and_bltin $Y }.")
RULE(BF_CALLBACK_2, "( { $X } bf_xor { $Y } ) = { $X bf_xor_bltin $Y }.")
RULE(BF_CALLBACK_3, "bf_neg( { $X } ) = { bf_neg_bltin $X }.")
RULE(BF_SQUEEZE_POSITIVES_0, "( $X = 0 ) wwf_and ($Y = 0) = ($X bf_or $Y) = 0.")
RULE(BF_FUNCTIONAL_QUANTIFIERS_0, "bf_all $X ( $Y ) = ( bf_subs_bltn $X { 0 } $Y  bf_and bf_subs_bltn $X { 1 } $Y ).")
RULE(BF_FUNCTIONAL_QUANTIFIERS_1, "bf_ex $X ( $Y ) = ( bf_subs_bltn $X { 0 } $Y  bf_or bf_subs_bltn $X { 1 } $Y ).")
// further processing (a + b := (a ∧ ¬b) ∨ (b ∧ ¬a) = (a ∨ b) ∧ ¬(a ∧ b))
// "( ($X bf_and $Y) = 0 ) wwf_and ( ($X bf_and $Z) != 0) = ( bf_all $X ( ( $X bf_and $Y$ ) = 0 )  wwf_and ( bf_ex $X ( ( $X bf_or ( $X bf_and $Y )) bf_and bf_neg ( $X bf_and ($X bf_and $Y ) ) wwf_and $Z )."
RULE(BF_PROCESS_0, "( ($X bf_and $Y) = 0 ) wwf_and ( ($X bf_and $Z) != 0) = ( bf_all $X ( ( $X bf_and $Y$ ) = 0 )  wwf_and ( bf_ex $X ( ( $X bf_or ( $X bf_and $Y )) bf_and bf_neg ( $X bf_and $Y ) ) wwf_and $Z ).")
RULE(BF_SKIP_CONSTANTS_0, "{ $X } bf_and $Y = $Y bf_and { $X }.")
RULE(BF_ROTATE_LITERALS_0, "( $X bf_ and ( $Y bf_and $Z ) ) = ( $Y bf_and ( $Z bf_and $X ) ).")
RULE(BF_ROTATE_LITERALS_1, "( ( $X bf_ and $Y ) bf_and $Z ) ) = ( ( $Y bf_and $Z ) bf_and $X ) ).")
RULE(BF_TRIVIALITY_0, "( { 0 } = 0 ) = { 1 }.")
RULE(BF_TRIVIALITY_1, "( { 1 } = 0 ) = { 0 }.")
RULE(BF_TRIVIALITY_2, "( { 0 } != 0 ) = { 0 }.")
RULE(BF_TRIVIALITY_3, "( { 1 } != 0 ) = { 1 }.")
// TODO:MEDIUM define rules for wwf and bbf

// rules to be used during normalization, the order matters.
//
// TODO:MEDIUM add the rules for wff and cbf when ready.
const std::string system = 
	BF_ELIM_FORALL 
	+ BF_DISTRIBUTE_0 + BF_DISTRIBUTE_1
	+ BF_PUSH_NEGATION_INWARDS_0 + BF_PUSH_NEGATION_INWARDS_1 
	+ BF_ELIM_DOUBLE_NEGATION_0
	+ BF_SIMPLIFY_ONE_0 + BF_SIMPLIFY_ONE_1 + BF_SIMPLIFY_ONE_2 + BF_SIMPLIFY_ONE_3
	+ BF_SIMPLIFY_ZERO_0 + BF_SIMPLIFY_ZERO_1 + BF_SIMPLIFY_ZERO_2 + BF_SIMPLIFY_ZERO_3
	+ BF_SIMPLIFY_SELF_0 + BF_SIMPLIFY_SELF_1 + BF_SIMPLIFY_SELF_2 + BF_SIMPLIFY_SELF_3
		+ BF_SIMPLIFY_SELF_4 + BF_SIMPLIFY_SELF_5 
	+ BF_CALLBACK_0 + BF_CALLBACK_1 + BF_CALLBACK_2 + BF_CALLBACK_3 
	+ BF_SQUEEZE_POSITIVES_0 
	+ BF_FUNCTIONAL_QUANTIFIERS_0 + BF_FUNCTIONAL_QUANTIFIERS_1 
	+ BF_PROCESS_0 
	+ BF_SKIP_CONSTANTS_0 
	+ BF_ROTATE_LITERALS_0	+ BF_ROTATE_LITERALS_1 
	+ BF_TRIVIALITY_0 + BF_TRIVIALITY_1 + BF_TRIVIALITY_2 + BF_TRIVIALITY_3;

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
		auto is_bf_cte = is_predicate<tau_parser::bf_constant>();
		auto cte = find_top(n, is_bf_cte).value()[1];
		if (cte->value.index() == 0) return cte->child[0]->value;
		return cte->value;
	}
};

// TODO:HIGH tweak the execution of the system rules, maybe we can do it in a more
// efficient way.

// execute one step of the formula
template<typename... BAs>
formula<BAs...> program_step(const formula<BAs...>& p, const library<BAs...>& l) {
	auto main = p.main;
	auto main_after_prog = apply(p.rec_relations, main);
	// TODO sys rules should be applied while possible
	auto main_after_sys = tau_apply(l.system, main_after_prog);
	return formula<BAs...>(main_after_sys, p.rec_relations);
}

// executes the normalizer on the given source code, maybe in the future we 
// need to pass a formula instead of a source code.
template<typename binder_t, typename... BAs>
formula<BAs...> normalizer(std::string source, bindings<BAs...> bs) {
	auto prog_source = make_tau_source(source);
	auto sys_source = make_tau_source(system);
	auto prog = make_program(prog_source, bs);
	auto lib = make_library<BAs...>(sys_source);
	std::set<formula<BAs...>, prog_less<BAs...>> previous;
	previous.insert(prog);
	auto nprog = program_step(prog, lib);
	while (!previous.insert(nprog).second) {
		prog = nprog;
		nprog = program_step(prog, lib);
	}
	return prog;
}

} // namespace idni::tau

#endif // __NORMALIZER2_H__
