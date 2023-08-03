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

// execute one step of the program
template<typename... BAs>
program<BAs...> program_step(const program<BAs...>& p, const library<BAs...>& l) {
	auto main = p.main;
	auto main_after_prog = apply(p.rec_relations, main);
	// TODO sys rules should be applied while possible
	auto main_after_sys = tau_apply(l.system, main_after_prog);
	return program<BAs...>(main_after_sys, p.rec_relations);
}

// tau system library, used to define the tau system of rewriting rules
static constexpr char* system = 
	// distributivity
	"($X bf_or $Y) bf_and $Z = ($X bf_and $Y) bf_or ($X bf_and $Z))."
	"$X bf_and ($Y bf_or $Z) = ($X bf_and $Y) bf_or ($X bf_and $Z))."
	"$X wwf_and ($Y wwf_or $Z) = ($X wwf_and $Y) wwf_or ($X wwf_and $Z))."
	"$X wwf_or ($Y wwf_and $Z) = ($X wwf_or $Y) wwf_and ($X wwf_or $Z))."
	// push negations inwards
	"bf_neg ($X bf_and $Y) = (bf_neg $X) bf_or (bf_neg $Y)."
	"bf_neg ($X bf_or $Y) = (bf_neg $X) bf_and (bf_neg $Y)."
	"bf_neg (bf_neg $X) =  $X."
	// eliminate for all
	"bf_neg (bf_all $X ($Y)) == bf_ex $X ( bf_neg ($Y))."
	// some simplifications
	"( { 1 } bf_or $X ) = { 1 }."
	"( { 0 } bf_or $X ) = $X."
	"( { 1 } bf_and $X ) = $X."
	"( { 0 } bf_and $X ) = { 0 }."
	"( $X bf_or { 1 } ) = { 1 }."
	"( $X bf_or { 0 } ) = $X."
	"( $X bf_and { 1 } ) = $X."
	"( $X bf_and { 0 } ) = { 0 }."
	// ba computations
	"( { $X } bf_or { $Y } ) = { $X bf_or_bltin $Y }."
	"( { $X } bf_and { $Y } ) = { $X bf_and_bltin $Y }."
	"( { $X } bf_xor { $Y } ) = { $X bf_xor_bltin $Y }."
	"bf_neg( { $X } ) = { bf_neg_bltin $X }."
	// squeeze positives
	"( $X = 0 ) wwf_and ($Y = 0) = ($X bf_or $Y) = 0."
	// functional quantifiers
	"bf_all $X ( $Y ) = ( bf_subs_bltn $X { 0 } $Y  bf_and bf_subs_bltn $X { 1 } $Y )."
	"bf_ex $X ( $Y ) = ( bf_subs_bltn $X { 0 } $Y  bf_or bf_subs_bltn $X { 1 } $Y )."
	// further processing (a + b := (a ∧ ¬b) ∨ (b ∧ ¬a) = (a ∨ b) ∧ ¬(a ∧ b))
	// "( ($X bf_and $Y) = 0 ) wwf_and ( ($X bf_and $Z) != 0) = ( bf_all $X ( ( $X bf_and $Y$ ) = 0 )  wwf_and ( bf_ex $X ( ( $X bf_or ( $X bf_and $Y )) bf_and bf_neg ( $X bf_and ($X bf_and $Y ) ) wwf_and $Z )."
	"( ($X bf_and $Y) = 0 ) wwf_and ( ($X bf_and $Z) != 0) = ( bf_all $X ( ( $X bf_and $Y$ ) = 0 )  wwf_and ( bf_ex $X ( ( $X bf_or ( $X bf_and $Y )) bf_and bf_neg ( $X bf_and $Y ) ) wwf_and $Z )."
	// skip constants
	"{ $X } bf_and $Y = $Y bf_and { $X }."
	// rotate literals
	"( $X bf_ and ( $Y bf_and $Z ) ) = ( $Y bf_and ( $Z bf_and $X ) )."
	"( ( $X bf_ and $Y ) bf_and $Z ) ) = ( ( $Y bf_and $Z ) bf_and $X ) )."
	// trivialities
	"( { 0 } = 0 ) = { 1 }"
	"( { 1 } = 0 ) = { 0 }"
	;

// CHECK could we assume we are working with the product algebra?
template<typename... BAs>
struct prog_less {

	bool operator()(const program<BAs...>& p1, const program<BAs...>& p2) const {
		auto m1 = extract_cte(p1.main);
		auto m2 = extract_cte(p2.main);
		// CHECK Could we assume we have a partial order on the algebra induced by
		// the operations of the algebra (i.e. a ≤ b iff a = b ∧ a )?
		// CHECK Do we need < or ≤ is enough for working?
		return m1 < m2;
	}
private:
	sp_tau_node<BAs...> extract_cte(sp_tau_node<BAs...> n) const {
		auto is_bf_cte = is<tau_parser::bf_constant>();
		auto cte = find_top(n, is_bf_cte).value()[1];
		if (cte->value.index() == 0) return cte->child[0]->value;
		return cte->value;
	}
};

template<typename... BAs>
program<BAs...> normalizer(std::string source, bindings<BAs...> bs) {
	auto prog_source = make_tau_source(source);
	auto sys_source = make_tau_source(system);
	auto prog = make_program(prog_source, bs);
	auto lib = make_library<BAs...>(sys_source);
	std::set<program<BAs...>, prog_less<BAs...>> previous;
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
