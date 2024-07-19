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

#ifndef __NORMAL_FORMS_H__
#define __NORMAL_FORMS_H__

#include <list>
#include <numeric>

#include "nso_rr.h"
#include "execution.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

using namespace idni::rewriter;
using namespace idni::tau;

namespace idni::tau {

// tau system library, used to define the tau system of rewriting rules
#define RULE(name, code) const std::string name = code;

// IDEA (MEDIUM) add commutative rule and halve the number of rules if is performance friendly

// bf rules
RULE(BF_TO_DNF_0, "($X | $Y) & $Z := $X & $Z | $Y & $Z.")
RULE(BF_TO_DNF_1, "$X & ($Y | $Z) := $X & $Y | $X & $Z.")
RULE(BF_PUSH_NEGATION_INWARDS_0, "($X & $Y)' := $X' | $Y'.")
RULE(BF_PUSH_NEGATION_INWARDS_1, "($X | $Y)' := $X' & $Y'.")
RULE(BF_ELIM_DOUBLE_NEGATION_0, "$X'' :=  $X.")
RULE(BF_SIMPLIFY_ONE_0, "1 | $X := 1.")
RULE(BF_SIMPLIFY_ONE_1, "$X | 1 := 1.")
RULE(BF_SIMPLIFY_ONE_2, "1 & $X := $X.")
RULE(BF_SIMPLIFY_ONE_3, "$X & 1 := $X.")
RULE(BF_SIMPLIFY_ONE_4, "1' := 0.")
RULE(BF_SIMPLIFY_ZERO_0, "0 & $X := 0.")
RULE(BF_SIMPLIFY_ZERO_1, "$X & 0 := 0.")
RULE(BF_SIMPLIFY_ZERO_2, "0 | $X := $X.")
RULE(BF_SIMPLIFY_ZERO_3, "$X | 0 := $X.")
RULE(BF_SIMPLIFY_ZERO_4, "0' := 1.")
RULE(BF_SIMPLIFY_SELF_0, "$X & $X := $X.")
RULE(BF_SIMPLIFY_SELF_1, "$X | $X := $X.")
RULE(BF_SIMPLIFY_SELF_2, "$X & $X' := 0.")
RULE(BF_SIMPLIFY_SELF_3, "$X | $X' := 1.")
RULE(BF_SIMPLIFY_SELF_4, "$X' & $X := 0.")
RULE(BF_SIMPLIFY_SELF_5, "$X' | $X := 1.")

// bf definitions
RULE(BF_DEF_XOR, "$X + $Y := $X & $Y' | $X' & $Y.")

// bf callbacks
RULE(BF_CALLBACK_AND, "{ $X } & { $Y } := bf_and_cb $X $Y.")
RULE(BF_CALLBACK_OR, "{ $X } | { $Y } := bf_or_cb $X $Y.")
RULE(BF_CALLBACK_XOR, "{ $X } + { $Y } := bf_xor_cb $X $Y.")
RULE(BF_CALLBACK_NEG, "{ $X }' := bf_neg_cb $X.")
RULE(BF_CALLBACK_NORMALIZE, "{ $X } := bf_normalize_cb $X.")
RULE(BF_CALLBACK_IS_ZERO, "{ $X } := bf_is_zero_cb { $X } 0.")
RULE(BF_CALLBACK_IS_ONE, "{ $X } := bf_is_one_cb { $X } 1.")
RULE(BF_PUSH_CTES_UPWARDS_0, "$X & ({ $Y } & $Z) := { $Y } & ($X & $Z).")
RULE(BF_PUSH_CTES_UPWARDS_1, "$X & ($Y & { $Z }) := { $Z } & ($X & $Y).")
RULE(BF_PUSH_CTES_UPWARDS_2, "($X & { $Y }) & $Z := { $Y } & ($X & $Z).")
RULE(BF_PUSH_CTES_UPWARDS_3, "$X & ($Y & { $Z }) := { $Z } & ($X & $Y).")
RULE(BF_PUSH_CTES_UPWARDS_4, "$X | ({ $Y } | $Z) := { $Y } | ($X | $Z).")
RULE(BF_PUSH_CTES_UPWARDS_5, "$X | ($Y | { $Z }) := { $Z } | ($X | $Y).")
RULE(BF_PUSH_CTES_UPWARDS_6, "($X | { $Y }) | $Z := { $Y } | ($X | $Z).")
RULE(BF_PUSH_CTES_UPWARDS_7, "$X | ($Y | { $Z }) := { $Z } | ($X | $Y).")

// wff rules
RULE(WFF_TO_DNF_0, "($X || $Y) && $Z ::= $X && $Z || $Y && $Z.")
RULE(WFF_TO_DNF_1, "$X && ($Y || $Z) ::= $X && $Y || $X && $Z.")
RULE(WFF_PUSH_NEGATION_INWARDS_0, "!($X && $Y) ::= ! $X || !$Y.")
RULE(WFF_PUSH_NEGATION_INWARDS_1, "!($X || $Y) ::= ! $X && !$Y.")
RULE(WFF_PUSH_NEGATION_INWARDS_2, "!($X = 0) ::= $X != 0.")
RULE(WFF_PUSH_NEGATION_INWARDS_3, "!($X != 0) ::= $X = 0.")
RULE(WFF_PUSH_NEGATION_INWARDS_4, "! always $X ::= sometimes (! $X).")
RULE(WFF_PUSH_NEGATION_INWARDS_5, "! sometimes $X ::= always (! $X).")
RULE(WFF_ELIM_DOUBLE_NEGATION_0, "! ! $X ::=  $X.")
RULE(WFF_SIMPLIFY_ONE_0, "T || $X ::= T.")
RULE(WFF_SIMPLIFY_ONE_1, "$X || T ::= T.")
RULE(WFF_SIMPLIFY_ONE_2, "T && $X ::= $X.")
RULE(WFF_SIMPLIFY_ONE_3, "$X && T ::= $X.")
RULE(WFF_SIMPLIFY_ONE_4, " ! T ::= F.")
RULE(WFF_SIMPLIFY_ONE_5, " always T ::= T.")
RULE(WFF_SIMPLIFY_ONE_6, " sometimes T ::= T.")
RULE(WFF_SIMPLIFY_ZERO_0, "F && $X ::= F.")
RULE(WFF_SIMPLIFY_ZERO_1, "$X && F ::= F.")
RULE(WFF_SIMPLIFY_ZERO_2, "F || $X ::= $X.")
RULE(WFF_SIMPLIFY_ZERO_3, "$X || F ::= $X.")
RULE(WFF_SIMPLIFY_ZERO_4, "! F ::= T.")
RULE(WFF_SIMPLIFY_ZERO_5, "always F ::= F.")
RULE(WFF_SIMPLIFY_ZERO_6, "sometimes F ::= F.")
RULE(WFF_SIMPLIFY_SELF_0, "$X && $X ::= $X.")
RULE(WFF_SIMPLIFY_SELF_1, "$X || $X ::= $X.")
RULE(WFF_SIMPLIFY_SELF_2, "$X && ! $X ::= F.")
RULE(WFF_SIMPLIFY_SELF_3, "$X || ! $X ::= T.")
RULE(WFF_SIMPLIFY_SELF_4, "!$X && $X ::= F.")
RULE(WFF_SIMPLIFY_SELF_5, "!$X || $X ::= T.")
RULE(WFF_SIMPLIFY_SOMETIMES_1,  "sometimes sometimes $X ::= sometimes $X.")
RULE(WFF_SIMPLIFY_SOMETIMES_2,  "sometimes always $X ::= always $X.")
RULE(WFF_SIMPLIFY_SOMETIMES_3,  "(sometimes $X) && (always $X) ::= always $X.")
RULE(WFF_SIMPLIFY_ALWAYS_1,     "always always $X ::= always $X.")
RULE(WFF_SIMPLIFY_ALWAYS_2,     "always sometimes $X ::= sometimes $X.")
RULE(WFF_SIMPLIFY_ALWAYS_3,		"(always $X) && (sometimes $X) ::= always $X.")
RULE(WFF_PUSH_SOMETIMES_INWARDS,"sometimes($X || $Y) ::= (sometimes $X) || (sometimes $Y).")
RULE(WFF_PUSH_ALWAYS_INWARDS,   "always($X && $Y) ::= (always $X) && (always $Y).")

// wff definitions
RULE(WFF_DEF_XOR, "$X ^ $Y ::= $X && !$Y || !$X && $Y.")
RULE(WFF_DEF_CONDITIONAL, "$X ? $Y : $Z ::= ($X -> $Y) && (!$X -> $Z).")
RULE(WFF_DEF_IMPLY, "$X -> $Y ::= !$X || $Y.")
RULE(WFF_DEF_EQUIV, "$X <-> $Y ::= ($X -> $Y) && ($Y -> $X).")
RULE(WFF_DEF_BEX_0, "b_ex $X $Y ::= wff_remove_bexistential_cb $X $Y T F.")
RULE(WFF_DEF_BALL_0, "b_all $X $Y ::=  wff_remove_buniversal_cb $X $Y T F.")

// additional wff definitions (include wff formulas)
RULE(BF_DEF_LESS_EQUAL, "$X <= $Y ::= $X & $Y' = 0.")
RULE(BF_DEF_LESS, "$X < $Y ::= $X & $Y' = 0 && $X + $Y' != 0.")
RULE(BF_DEF_GREATER, "$X > $Y ::= $X & $Y' != 0 || $X + $Y' = 0.")

// we must expand the xor as its definition has been allready processed
RULE(BF_DEF_EQ, "$X = $Y ::= $X & $Y' | $X' & $Y = 0.")
RULE(BF_DEF_NEQ, "$X != $Y ::= $X & $Y' | $X' & $Y != 0.")

// wff callbacks
RULE(BF_CALLBACK_EQ, "{ $X } = 0 ::= bf_eq_cb $X T F.") // (T|F) is wff_(t|f)
RULE(BF_CALLBACK_NEQ, "{ $X } != 0 ::= bf_neq_cb $X T F.") // (T|F) is wff_(t|f)

// trivialities
RULE(BF_EQ_SIMPLIFY_0, "1 = 0 ::=  F.")
RULE(BF_EQ_SIMPLIFY_1, "0 = 0 ::= T.")
RULE(BF_NEQ_SIMPLIFY_0, "0 != 0 ::= F.")
RULE(BF_NEQ_SIMPLIFY_1, "1 != 0 ::= T.")
RULE(BF_EQ_AND_SIMPLIFY_0, "$X != 0 && $X = 0 ::= F.")
RULE(BF_EQ_AND_SIMPLIFY_1, "$X = 0 && $X != 0 ::= F.")
RULE(BF_EQ_OR_SIMPLIFY_0, "$X != 0 || $X = 0 ::= T.")
RULE(BF_EQ_OR_SIMPLIFY_1, "$X = 0 || $X != 0 ::= T.")

// bf conjunctive normal form
RULE(BF_TO_CNF_0, "$X & $Y | $Z := ($X | $Z) & ($Y | $Z).")
RULE(BF_TO_CNF_1, "$X | $Y & $Z := ($X | $Y) & ($X | $Z).")

// wff conjunctive normal form
RULE(WFF_TO_CNF_0, "$X && $Y || $Z ::= ($X || $Z) && ($Y || $Z).")
RULE(WFF_TO_CNF_1, "$X || $Y && $Z ::= ($X || $Y) && ($X || $Z).")

RULE(WFF_PUSH_NEGATION_UPWARDS_0, "$X != $Y ::= !($X = $Y).")
RULE(WFF_UNSQUEEZE_POSITIVES_0, "$X | $Y = 0 ::= $X = 0 && $Y = 0.")
RULE(WFF_UNSQUEEZE_NEGATIVES_0, "$X | $Y != 0 ::= $X != 0 || $Y != 0.")

RULE(BF_POSITIVE_LITERAL_UPWARDS_0, "$X != 0 && $Y  = 0 && $Z != 0 ::= $Y = 0 && $X != 0 && $Z != 0.")
RULE(BF_POSITIVE_LITERAL_UPWARDS_1, "$X != 0 && $Y != 0 && $Z  = 0 ::= $Z = 0 && $X != 0 && $Y != 0.")
RULE(BF_POSITIVE_LITERAL_UPWARDS_2, "$Z != 0 && $X  = 0 && $Y != 0 ::= $X = 0 && $Z != 0 && $Y != 0.")
RULE(BF_POSITIVE_LITERAL_UPWARDS_3, "$Z != 0 && $X != 0 && $Y  = 0 ::= $Z != 0 && $Y = 0 && $X != 0.")
RULE(BF_POSITIVE_LITERAL_UPWARDS_4, "$X != 0 && $Y  = 0 ::= $Y = 0 && $X != 0.")
RULE(WFF_SQUEEZE_POSITIVES_0, "$X = 0 && $Y = 0 ::= $X | $Y = 0.")

// TODO (LOW) delete trivial quantified formulas (i.e. ∀x. F = no_x..., ).

// tau rules to dnf
RULE(TAU_TO_DNF_0, "($X ||| $Y) &&& $Z :::= $X &&& $Z ||| $Y &&& $Z.")
RULE(TAU_TO_DNF_1, "$X &&& ($Y ||| $Z) :::= $X &&& $Y ||| $X &&& $Z.")
RULE(TAU_PUSH_NEGATION_INWARDS_0, "- ($X &&& $Y) :::= - $X ||| - $Y.")
RULE(TAU_PUSH_NEGATION_INWARDS_1, "- ($X ||| $Y) :::= - $X &&& - $Y.")
RULE(TAU_ELIM_DOUBLE_NEGATION_0, "- - $X :::=  $X.")
RULE(TAU_SIMPLIFY_ONE_0, "{T} ||| $X :::= {T}.")

// wff conjunctive normal form
RULE(TAU_TO_CNF_0, "$X &&& $Y ||| $Z ::= ($X ||| $Z) &&& ($Y ||| $Z).")
RULE(TAU_TO_CNF_1, "$X ||| $Y &&& $Z ::= ($X ||| $Y) &&& ($X ||| $Z).")

// tau simplifications
RULE(TAU_SIMPLIFY_ONE_1, "$X ||| {T} :::= {T}.")
RULE(TAU_SIMPLIFY_ONE_2, "{T} &&& $X :::= $X.")
RULE(TAU_SIMPLIFY_ONE_3, "$X &&& {T} :::= $X.")
RULE(TAU_SIMPLIFY_ONE_4, "- {T} :::= {F}.")
RULE(TAU_SIMPLIFY_ZERO_0, "{F} &&& $X :::= {F}.")
RULE(TAU_SIMPLIFY_ZERO_1, "$X &&& {F} :::= {F}.")
RULE(TAU_SIMPLIFY_ZERO_2, "{F} ||| $X :::= $X.")
RULE(TAU_SIMPLIFY_ZERO_3, "$X ||| {F} :::= $X.")
RULE(TAU_SIMPLIFY_ZERO_4, "- {F} :::= {T}.")
RULE(TAU_SIMPLIFY_SELF_0, "$X &&& $X :::= $X.")
RULE(TAU_SIMPLIFY_SELF_1, "$X ||| $X :::= $X.")
RULE(TAU_SIMPLIFY_SELF_2, "$X &&& - $X :::= {F}.")
RULE(TAU_SIMPLIFY_SELF_3, "$X ||| - $X :::= {T}.")
RULE(TAU_SIMPLIFY_SELF_4, "- $X &&& $X :::= {F}.")
RULE(TAU_SIMPLIFY_SELF_5, "- $X ||| $X :::= {T}.")

template<typename... BAs>
// TODO (LOW) rename library with rwsys or another name
static auto apply_defs = make_library<BAs...>(
	// wff defs
	WFF_DEF_XOR
	+ WFF_DEF_CONDITIONAL
	+ WFF_DEF_IMPLY
	+ WFF_DEF_EQUIV
	// bf defs
	+ BF_DEF_XOR
	+ WFF_DEF_BEX_0
	+ WFF_DEF_BALL_0
);

template<typename... BAs>
static auto apply_wff_defs = make_library<BAs...>(
	WFF_DEF_XOR
	+ WFF_DEF_CONDITIONAL
	+ WFF_DEF_IMPLY
	+ WFF_DEF_EQUIV
);

template<typename... BAs>
static auto apply_bf_defs = make_library<BAs...>(
	BF_DEF_XOR
	+ WFF_DEF_BEX_0
	+ WFF_DEF_BALL_0
);

template<typename... BAs>
static auto apply_defs_once = make_library<BAs...>(
	// wff defs
	BF_DEF_LESS_EQUAL
	+ BF_DEF_LESS
	+ BF_DEF_GREATER
	+ BF_DEF_EQ
	+ BF_DEF_NEQ
);

template<typename... BAs>
static auto to_dnf_bf = make_library<BAs...>(
	BF_TO_DNF_0
	+ BF_TO_DNF_1
	+ BF_PUSH_NEGATION_INWARDS_0
	+ BF_PUSH_NEGATION_INWARDS_1
	+ BF_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto to_dnf_tau = make_library<BAs...>(
	TAU_TO_DNF_0
	+ TAU_TO_DNF_1
	+ TAU_PUSH_NEGATION_INWARDS_0
	+ TAU_PUSH_NEGATION_INWARDS_1
	+ TAU_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto to_dnf_wff = make_library<BAs...>(
	WFF_TO_DNF_0
	+ WFF_TO_DNF_1
	+ WFF_PUSH_NEGATION_INWARDS_0
	+ WFF_PUSH_NEGATION_INWARDS_1
	+ WFF_PUSH_NEGATION_INWARDS_2
	+ WFF_PUSH_NEGATION_INWARDS_3
	+ WFF_PUSH_NEGATION_INWARDS_4
	+ WFF_PUSH_NEGATION_INWARDS_5
	+ WFF_ELIM_DOUBLE_NEGATION_0
	+ WFF_PUSH_ALWAYS_INWARDS
	+ WFF_PUSH_SOMETIMES_INWARDS
);

template<typename... BAs>
static auto simplify_bf = make_library<BAs...>(
	BF_SIMPLIFY_ONE_0
	+ BF_SIMPLIFY_ONE_1
	+ BF_SIMPLIFY_ONE_2
	+ BF_SIMPLIFY_ONE_3
	+ BF_SIMPLIFY_ONE_4
	+ BF_SIMPLIFY_ZERO_0
	+ BF_SIMPLIFY_ZERO_1
	+ BF_SIMPLIFY_ZERO_2
	+ BF_SIMPLIFY_ZERO_3
	+ BF_SIMPLIFY_ZERO_4
	+ BF_SIMPLIFY_SELF_0
	+ BF_SIMPLIFY_SELF_1
	+ BF_SIMPLIFY_SELF_2
	+ BF_SIMPLIFY_SELF_3
	+ BF_SIMPLIFY_SELF_4
	+ BF_SIMPLIFY_SELF_5
);

template<typename... BAs>
static auto simplify_bf_more = make_library<BAs...>(
	BF_ELIM_DOUBLE_NEGATION_0 +
	BF_DEF_XOR
);

template<typename... BAs>
static auto simplify_wff = make_library<BAs...>(
	WFF_SIMPLIFY_ONE_0
	+ WFF_SIMPLIFY_ONE_1
	+ WFF_SIMPLIFY_ONE_2
	+ WFF_SIMPLIFY_ONE_3
	+ WFF_SIMPLIFY_ONE_4
	+ WFF_SIMPLIFY_ONE_5
	+ WFF_SIMPLIFY_ONE_6
	+ WFF_SIMPLIFY_ZERO_0
	+ WFF_SIMPLIFY_ZERO_1
	+ WFF_SIMPLIFY_ZERO_2
	+ WFF_SIMPLIFY_ZERO_3
	+ WFF_SIMPLIFY_ZERO_4
	+ WFF_SIMPLIFY_ZERO_5
	+ WFF_SIMPLIFY_ZERO_6
	+ WFF_SIMPLIFY_SELF_0
	+ WFF_SIMPLIFY_SELF_1
	+ WFF_SIMPLIFY_SELF_2
	+ WFF_SIMPLIFY_SELF_3
	+ WFF_SIMPLIFY_SELF_4
	+ WFF_SIMPLIFY_SELF_5
	+ WFF_SIMPLIFY_SOMETIMES_1
	+ WFF_SIMPLIFY_SOMETIMES_2
	+ WFF_SIMPLIFY_SOMETIMES_3
	+ WFF_SIMPLIFY_ALWAYS_1
	+ WFF_SIMPLIFY_ALWAYS_2
	+ WFF_SIMPLIFY_ALWAYS_3
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
static auto apply_cb = make_library<BAs...>(
	BF_CALLBACK_AND
	+ BF_CALLBACK_OR
	+ BF_CALLBACK_XOR
	+ BF_CALLBACK_NEG
	/*+ BF_PUSH_CTES_UPWARDS_0
	+ BF_PUSH_CTES_UPWARDS_1
	+ BF_PUSH_CTES_UPWARDS_2
	+ BF_PUSH_CTES_UPWARDS_3
	+ BF_PUSH_CTES_UPWARDS_4
	+ BF_PUSH_CTES_UPWARDS_5
	+ BF_PUSH_CTES_UPWARDS_6
	+ BF_PUSH_CTES_UPWARDS_7*/
);

template<typename... BAs>
static auto apply_normalize = make_library<BAs...>(
	BF_CALLBACK_NORMALIZE
);

template<typename... BAs>
static auto elim_bf_constant_01 = make_library<BAs...>(
	BF_CALLBACK_IS_ONE
	+ BF_CALLBACK_IS_ZERO
);

template<typename... BAs>
static auto elim_eqs = make_library<BAs...>(
	BF_CALLBACK_EQ
	+ BF_CALLBACK_NEQ
	+ BF_EQ_SIMPLIFY_0
	+ BF_EQ_SIMPLIFY_1
	+ BF_NEQ_SIMPLIFY_0
	+ BF_NEQ_SIMPLIFY_1
	+ BF_EQ_AND_SIMPLIFY_0
	+ BF_EQ_AND_SIMPLIFY_1
	+ BF_EQ_OR_SIMPLIFY_0
	+ BF_EQ_OR_SIMPLIFY_1
);

template<typename... BAs>
static auto trivialities = make_library<BAs...>(
	BF_EQ_SIMPLIFY_0
	+ BF_EQ_SIMPLIFY_1
	+ BF_NEQ_SIMPLIFY_0
	+ BF_NEQ_SIMPLIFY_1
);

template<typename... BAs>
static auto simplify_snf = repeat_all<step<BAs...>, BAs...>(
	apply_cb<BAs...>
	| elim_eqs<BAs...>
	| simplify_wff<BAs...>
	| trivialities<BAs...>);

template<typename... BAs>
static auto to_mnf_wff = make_library<BAs...>(
	WFF_PUSH_NEGATION_UPWARDS_0
);

template<typename... BAs>
static auto to_mnf_snf_wff = make_library<BAs...>(
	WFF_PUSH_NEGATION_UPWARDS_0
	+ WFF_PUSH_NEGATION_INWARDS_0
	+ WFF_PUSH_NEGATION_INWARDS_1
	+ WFF_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto unsqueeze_wff = make_library<BAs...>(
	WFF_UNSQUEEZE_POSITIVES_0
	+ WFF_UNSQUEEZE_NEGATIVES_0
);

template<typename... BAs>
static auto to_cnf_wff = make_library<BAs...>(
	WFF_TO_CNF_0
	+ WFF_TO_CNF_1
	+ WFF_PUSH_NEGATION_INWARDS_0
	+ WFF_PUSH_NEGATION_INWARDS_1
	+ WFF_PUSH_NEGATION_INWARDS_2
	+ WFF_PUSH_NEGATION_INWARDS_3
	+ WFF_PUSH_NEGATION_INWARDS_4
	+ WFF_PUSH_NEGATION_INWARDS_5
	+ WFF_ELIM_DOUBLE_NEGATION_0
	+ WFF_PUSH_ALWAYS_INWARDS
	+ WFF_PUSH_SOMETIMES_INWARDS
);

template<typename... BAs>
static auto to_cnf_tau = make_library<BAs...>(
	TAU_TO_CNF_0
	+ TAU_TO_CNF_1
	+ TAU_PUSH_NEGATION_INWARDS_0
	+ TAU_PUSH_NEGATION_INWARDS_1
	+ TAU_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto to_cnf_bf = make_library<BAs...>(
	BF_TO_CNF_0
	+ BF_TO_CNF_1
	+ BF_PUSH_NEGATION_INWARDS_0
	+ BF_PUSH_NEGATION_INWARDS_1
	+ BF_ELIM_DOUBLE_NEGATION_0
);

template<typename...BAs>
static auto to_nnf_wff = make_library<BAs...>(
	WFF_PUSH_NEGATION_INWARDS_0
	+ WFF_PUSH_NEGATION_INWARDS_1
	+ WFF_PUSH_NEGATION_INWARDS_2
	+ WFF_PUSH_NEGATION_INWARDS_3
	+ WFF_PUSH_NEGATION_INWARDS_4
	+ WFF_PUSH_NEGATION_INWARDS_5
	+ WFF_ELIM_DOUBLE_NEGATION_0
);

template<typename...BAs>
static auto to_nnf_tau = make_library<BAs...>(
	TAU_PUSH_NEGATION_INWARDS_0
	+ TAU_PUSH_NEGATION_INWARDS_1
	+ TAU_ELIM_DOUBLE_NEGATION_0
);

template<typename...BAs>
static auto to_nnf_bf = make_library<BAs...>(
	BF_PUSH_NEGATION_INWARDS_0
	+ BF_PUSH_NEGATION_INWARDS_1
	+ BF_ELIM_DOUBLE_NEGATION_0
);

template<tau_parser::nonterminal type, typename... BAs>
struct reduce {

	// TODO (VERY_HIGH) properly implement it
	nso<BAs...> operator()(const nso<BAs...>& form) const {
		std::map<nso<BAs...>, nso<BAs...>> changes;
		// for all type dnfs do...
		for (auto& dnf: select_top(form, is_non_terminal<type, BAs...>)) {
			auto simplified = simplify(dnf);
			if (simplified != dnf) changes[dnf] = simplified;
		}
		return replace(form, changes);
	}

private:

	void get_literals(const nso<BAs...>& clause, std::set<nso<BAs...>>& literals) const {
		BOOST_LOG_TRIVIAL(trace) << "(I) get_bf_literals of: " << clause;
		if constexpr (type == tau_parser::bf) {
			if (auto check = clause | tau_parser::bf_and; check.has_value())
				for (auto& c : check || tau_parser::bf)
					get_literals(c, literals);
			else {
				literals.insert(clause);
				BOOST_LOG_TRIVIAL(trace) << "(I) found literal: " << clause;
			}
		} else {
			if (auto check = clause | tau_parser::wff_and; check.has_value())
				for (auto& c : check || tau_parser::wff)
					get_literals(c , literals);
			else {
				literals.insert(clause);
				BOOST_LOG_TRIVIAL(trace) << "(I) found literal: " << clause;
			}
		}
	}

	std::set<nso<BAs...>> get_literals(const nso<BAs...>& clause) const {
		std::set<nso<BAs...>> literals;
		get_literals(clause, literals);
		return literals;
	}

	std::pair<std::set<nso<BAs...>>, std::set<nso<BAs...>>> get_positive_negative_literals(const nso<BAs...> clause) const {
		std::set<nso<BAs...>> positives;
		std::set<nso<BAs...>> negatives;

		for(auto& l: get_literals(clause)) {
			if constexpr (type == tau_parser::bf) {
				if (auto check = l | tau_parser::bf_neg; !check.has_value()) {
					positives.insert(l);
					BOOST_LOG_TRIVIAL(trace) << "(I) found positive: " << l;
				} else {
					negatives.insert(l);
					BOOST_LOG_TRIVIAL(trace) << "(I) found negative: " << l;
				}
			} else {
				if (auto check = l | tau_parser::wff_neg; !check.has_value()) {
					positives.insert(l);
					BOOST_LOG_TRIVIAL(trace) << "(I) found positive: " << l;
				} else {
					negatives.insert(l);
					BOOST_LOG_TRIVIAL(trace) << "(I) found negative: " << l;
				}
			}
		}

		return {positives, negatives};
	}

	std::set<nso<BAs...>> get_dnf_clauses(const nso<BAs...>& n, std::set<nso<BAs...>> clauses = {}) const {
		if constexpr (type == tau_parser::bf)
			if (auto check = n | tau_parser::bf_or; check.has_value())
				for (auto& clause: check || tau_parser::bf)
					clauses = get_dnf_clauses(clause, clauses);
			else
				clauses.insert(n);
		else
			if (auto check = n | tau_parser::wff_or; check.has_value())
				for (auto& clause: check || tau_parser::wff)
					clauses = get_dnf_clauses(clause, clauses);
			else
				clauses.insert(n);

		#ifdef DEBUG
		if (clauses.empty()) BOOST_LOG_TRIVIAL(trace) << "(I) found clause: " << n << std::endl;
		else for (auto& clause: clauses) BOOST_LOG_TRIVIAL(trace) << "(I) found clause: " << clause;
		#endif // DEBUG

		return clauses;
	}

	std::optional<nso<BAs...>> build_dnf_clause_from_literals(const std::set<nso<BAs...>>& positives, const std::set<nso<BAs...>>& negatives) const {
		if (positives.empty() && negatives.empty()) {
			BOOST_LOG_TRIVIAL(debug) << "(F) {}";
			return {};
		}

		std::vector<nso<BAs...>> literals;
		literals.insert(literals.end(), positives.begin(), positives.end());
		literals.insert(literals.end(), negatives.begin(), negatives.end());

		if (literals.size() == 1) return literals[0];
		auto clause = literals[0];
		for (size_t i = 1; i < literals.size(); ++i)
			if constexpr (type == tau_parser::bf) clause = build_bf_and(clause, literals[i]);
			else clause = build_wff_and(clause, literals[i]);

		BOOST_LOG_TRIVIAL(debug) << "(I) " << clause;
		return { clause };
	}

	std::optional<nso<BAs...>> to_minterm(const nso<BAs...>& clause) const {
		auto [positives, negatives] = get_positive_negative_literals(clause);
		if constexpr (type == tau_parser::bf) {
			for (auto& negation: negatives) {
				auto negated = negation
					| tau_parser::bf_neg
					| tau_parser::bf
					| optional_value_extractor<sp_tau_node<BAs...>>;
				for (auto& positive: positives) {
					BOOST_LOG_TRIVIAL(trace) << "(I) are literals " << positive << " and " << negation << " clashing? ";
					if (positive == _0<BAs...>) {
						BOOST_LOG_TRIVIAL(trace) << "yes" << std::endl;
						return {};
					} else if (positive == negated) {
						BOOST_LOG_TRIVIAL(trace) << "yes" << std::endl;
						return {};
					} else {
						BOOST_LOG_TRIVIAL(trace) << "no" << std::endl;
					}
				}
			}
		} else {
			for (auto& negation: negatives) {
				auto neq_bf = negation
					| tau_parser::wff_neg
					| tau_parser::wff
					| optional_value_extractor<sp_tau_node<BAs...>>;
				for (auto& positive: positives) {
					auto eq_bf = positive
						| first_child_extractor<BAs...>
						| tau_parser::bf
						| optional_value_extractor<sp_tau_node<BAs...>>;
					BOOST_LOG_TRIVIAL(trace) << "(I) are literals " << positive << " and " << negation << " clashing: ";
					if (eq_bf == _F<BAs...>) {
						BOOST_LOG_TRIVIAL(trace) << "yes" << std::endl;
						return {};
					} if (eq_bf == neq_bf) {
						BOOST_LOG_TRIVIAL(trace) << "yes" << std::endl;
						return {};
					} else {
						BOOST_LOG_TRIVIAL(trace) << "no" << std::endl;
					}
				}
			}
		}
		return build_dnf_clause_from_literals(positives, negatives);
	}

	nso<BAs...> build_dnf_from_clauses(const std::set<nso<BAs...>>& clauses) const {
		if constexpr (type == tau_parser::bf) {
			if (clauses.empty()) {
				BOOST_LOG_TRIVIAL(debug) << "(F) " << _0<BAs...>;
				return _0<BAs...>;
			}
		} else {
			if (clauses.empty()) {
				BOOST_LOG_TRIVIAL(debug) << "(F) " << _F<BAs...>;
				return _F<BAs...>;
			}
		}
		auto dnf = *clauses.begin();
		auto it = ++clauses.begin();
		for (; it != clauses.end(); ++it)
			if constexpr (type == tau_parser::bf) dnf = build_bf_or(dnf, *it);
			else dnf = build_wff_or(dnf, *it);

		BOOST_LOG_TRIVIAL(debug) << "(F) " << dnf;
		return dnf;
	}

	nso<BAs...> simplify(const nso<BAs...>& form) const {
		std::set<nso<BAs...>> clauses;
		BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin simplifying of " << form;
		for (auto& clause: get_dnf_clauses(form))
			if (auto dnf = to_minterm(clause); dnf) clauses.insert(dnf.value());
		auto dnf = build_dnf_from_clauses(clauses);
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End simplifying";
		return dnf;
	}
};

template<typename... BAs>
static const reduce<tau_parser::bf, BAs...> reduce_bf;
template<typename... BAs>
using reduce_bf_t = reduce<tau_parser::bf, BAs...>;

template<typename...BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const reduce_bf_t<BAs...>& r) {
	return r(n);
}

template<typename... BAs>
static const reduce<tau_parser::wff, BAs...> reduce_wff;
template<typename... BAs>
using reduce_wff_t = reduce<tau_parser::wff, BAs...>;

template<typename...BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const reduce_wff_t<BAs...>& r) {
	return r(n);
}

// return the inner quantifier or the top wff if the formula is not quantified
template<typename...BAs>
std::pair<std::optional<nso<BAs...>>, nso<BAs...>> get_inner_quantified_wff(const nso<BAs...>& n) {
	// TODO (LOW) extract to a utils file
	auto quantified = [](const auto& n) -> bool {
		return (n | tau_parser::wff_ex).has_value() || (n | tau_parser::wff_all).has_value();
	};
	if (auto quantifier = find_bottom(n, quantified); quantifier) {
		return { quantifier | tau_parser::variable | optional_value_extractor<nso<BAs...>>,
			quantifier | tau_parser::wff | optional_value_extractor<nso<BAs...>> };
	}
	return { {}, n };
}

template<typename...BAs>
struct onf_wff {

	onf_wff(const nso<BAs...>& var) : var(var) {}

	nso<BAs...> operator()(const nso<BAs...>& n) const {
		auto [var, nn] = get_inner_quantified_wff(n);
		return onf_subformula(nn);
	}

private:

	nso<BAs...> onf_subformula(const nso<BAs...>& n) const {
		auto eq = find_bottom(n, is_non_terminal<tau_parser::bf_eq, BAs...>);
		std::map<nso<BAs...>, nso<BAs...>> changes_0 = {{var, _0<BAs...>}};
		auto f_0 = replace((eq || tau_parser::bf)[0],  changes_0)
			| repeat_each<step<BAs...>, BAs...>(
				simplify_bf<BAs...>
				| apply_cb<BAs...>
				| elim_eqs<BAs...>)
			| reduce_bf<BAs...>;
		std::map<nso<BAs...>, nso<BAs...>> changes_1 = {{var, _1<BAs...>}};
		auto f_1 = replace((eq || tau_parser::bf)[0],  changes_1)
			| repeat_each<step<BAs...>, BAs...>(
				simplify_bf<BAs...>
				| apply_cb<BAs...>
				| elim_eqs<BAs...>)
			| reduce_bf<BAs...>;
		std::map<nso<BAs...>, nso<BAs...>> changes;
		auto eq_change = trim(build_bf_interval(f_0, var, f_1));
		changes[eq | optional_value_extractor<nso<BAs...>>] = eq_change;
		for (auto& neq: select_all(n, is_non_terminal<tau_parser::bf_neq, BAs...>)) {
			auto bounds = neq || tau_parser::bf;
			std::map<nso<BAs...>, nso<BAs...>> changes_neq_0 = {{var, _0<BAs...>}};
			auto f_0 = replace(bounds[0],  changes_neq_0)
				| repeat_each<step<BAs...>, BAs...>(
					simplify_bf<BAs...>
					| apply_cb<BAs...>
					| elim_eqs<BAs...>)
				| reduce_bf<BAs...>;
			std::map<nso<BAs...>, nso<BAs...>> changes_neq_1 = {{var, _1<BAs...>}};
			auto f_1 = replace(bounds[0],  changes_neq_1)
				| repeat_each<step<BAs...>, BAs...>(
					simplify_bf<BAs...>
					| apply_cb<BAs...>
					| elim_eqs<BAs...>)
				| reduce_bf<BAs...>;
			auto nleq_change = build_bf_or(build_bf_nleq_lower(f_0, var), build_bf_nleq_upper(f_1, var));
			changes[neq] = nleq_change;
		}
		return replace(n, changes) | repeat_all<step<BAs...>, BAs...>(to_dnf_wff<BAs...>);
	}

	nso<BAs...> var;
};

template<typename... BAs>
using onf_wff_t = onf_wff<BAs...>;

template<typename...BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const onf_wff_t<BAs...>& r) {
	return r(n);
}

template<typename... BAs>
static const auto is_not_eq_or_neq_to_zero_predicate = [](const nso<BAs...>& n) {
	auto check = (n | only_child_extractor<BAs...> || tau_parser::bf)[1] || tau_parser::bf_f;
	return check.empty();
};

template<typename... BAs>
using is_not_eq_or_neq_predicate_t = decltype(is_not_eq_or_neq_to_zero_predicate<BAs...>);

// TODO (LOW) wthis should be converted into a struct
template<typename... BAs>
nso<BAs...> apply_once_definitions(const nso<BAs...>& form) {
	return nso_rr_apply_if(apply_defs_once<BAs...>, form, is_not_eq_or_neq_to_zero_predicate<BAs...>);
}

template<typename...BAs>
std::optional<nso<BAs...>> onf(const nso<BAs...>& n, const nso<BAs...>& var) {
	return apply_once_definitions(n)
		| apply_wff_defs<BAs...>
		| onf_wff<BAs...>(var)
		| repeat_all<step<BAs...>, BAs...>(
			to_dnf_wff<BAs...>
			| simplify_wff<BAs...>
			| trivialities<BAs...>);
}

template<typename...BAs>
nso<BAs...> dnf_wff(const nso<BAs...>& n) {
	auto [_, nn] = get_inner_quantified_wff(n);
	auto nform = apply_once_definitions(nn)
		| repeat_each<step<BAs...>, BAs...>(
			apply_wff_defs<BAs...>
			| to_dnf_wff<BAs...>
			| simplify_wff<BAs...>
			| trivialities<BAs...>
		);
	// finally, we also simplify the bf part of the formula
	return dnf_bf(nform);
}

template<typename...BAs>
nso<BAs...> dnf_bf(const nso<BAs...>& n) {
	return apply_once_definitions(n)
		| repeat_all<step<BAs...>, BAs...>(
			apply_bf_defs<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			to_dnf_bf<BAs...>
			| simplify_bf<BAs...>
			| apply_cb<BAs...>
			| elim_eqs<BAs...>)
		// TODO (MEDIUM) review after we fully normalize bf & wff
		| reduce_bf<BAs...>;
}

template<typename...BAs>
nso<BAs...> cnf_wff(const nso<BAs...>& n) {
	auto [_, nn] = get_inner_quantified_wff(n);
	auto wff = apply_once_definitions(nn)
		| repeat_each<step<BAs...>, BAs...>(
			apply_wff_defs<BAs...>
			| to_cnf_wff<BAs...>
			| simplify_wff<BAs...>
			| trivialities<BAs...>
		);
	// finally, we also simplify the bf part of the formula
	return cnf_bf(wff);
}

template<typename...BAs>
nso<BAs...> cnf_bf(const nso<BAs...>& n) {
	return n
		| repeat_each<step<BAs...>, BAs...>(
			apply_bf_defs<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			to_cnf_bf<BAs...>
			| simplify_bf<BAs...>
			| apply_cb<BAs...>
			| elim_eqs<BAs...>)
		// TODO (MEDIUM) review after we fully normalize bf & wff
		| reduce_bf<BAs...>;
}

template<typename...BAs>
nso<BAs...> nnf_bf(const nso<BAs...>& n) {
	return apply_once_definitions(n)
		| repeat_each<step<BAs...>, BAs...>(
			apply_bf_defs<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			to_nnf_bf<BAs...>
			| simplify_bf<BAs...>
			| apply_cb<BAs...>
			| elim_eqs<BAs...>)
		// TODO (MEDIUM) review after we fully normalize bf & wff
		| reduce_bf<BAs...>;
}

template<typename...BAs>
nso<BAs...> nnf_wff(const nso<BAs...>& n) {
	auto [_, nn] = get_inner_quantified_wff(n);
	auto nform = apply_once_definitions(nn)
		| repeat_each<step<BAs...>, BAs...>(
			apply_wff_defs<BAs...>
			| to_nnf_wff<BAs...>
			| simplify_wff<BAs...>
			| trivialities<BAs...>
		);
	// finally, we also simplify the bf part of the formula
	return nnf_bf(nform);
}

// Reduce currrent dnf due to update by coeff and variable assignment i
inline bool reduce_paths (vector<int_t>& i, vector<vector<int_t>>& paths, int_t p, bool surface = true) {
		for (size_t j=0; j < paths.size(); ++j) {
			// Get Hamming distance between i and path and position of last difference
			// while different irrelevant variables make assignments incompatible
			int_t dist = 0, pos = 0;
			for (int_t k=0; k < p; ++k) {
				if (i[k] == paths[j][k]) continue;
				else if (dist == 2) break;
				else if (i[k] == 2 || paths[j][k] == 2) { dist = 0; break; }
				else dist += 1, pos = k;
			}
			if (dist == 1) {
				// Remove i from paths if recursion depth is greater 0
				if(!surface) {
					paths[j] = {};
					// Resolve variable
					i[pos] = 2;
					if(ranges::all_of(i, [](const auto el) {return el == 2;}))
						return paths = {}, true;
					// Continue with resulting assignment
					reduce_paths(i, paths, p, false);
				} else {
					// Resolve variable
					paths[j][pos] = 2;
					if(ranges::all_of(paths[j], [](const auto el) {return el == 2;}))
						return paths = {}, true;
					// Continue with resulting assignment
					reduce_paths(paths[j], paths, p, false);
				}
				return true;
			}
		}
		return false;
}

// Ordering function for variables from nso formula
template<typename... BAs>
auto lex_var_comp = [](const auto x, const auto y) {
	auto xx = make_string(tau_node_terminal_extractor<BAs...>, x);
	auto yy = make_string(tau_node_terminal_extractor<BAs...>, y);
	return xx < yy;
};

// Starting from variable at position p+1 in vars write to i which variables are irrelevant in assignment
template<typename... BAs>
void elim_vars_in_assignment (const auto& fm, const auto&vars, auto& i, const int_t p) {
	auto cvars = select_all(fm, is_non_terminal<tau_parser::variable, BAs...>);
	sort(cvars.begin(), cvars.end(), lex_var_comp<BAs...>);

	// Set irrelevant vars in assignment i to 2
	int_t v_iter = p+1, cv_iter = 0;
	while (v_iter < (int_t)vars.size()) {
		if (cv_iter < (int_t)cvars.size() && vars[v_iter] == cvars[cv_iter]) {
			++v_iter, ++cv_iter;
		} else {
			i[v_iter] = 2;
			++v_iter;
		}
	}
}

// Create assignment in formula and reduce resulting clause
template<typename... BAs>
bool assign_and_reduce(const nso<BAs...>& fm, const vector<nso<BAs...>>& vars, vector<int_t>& i, auto& dnf, int_t p) {
	// Check if all variables are assigned
	if((int_t)vars.size() == p) {
		// Normalize tau subformulas
		auto fm_simp = fm | repeat_once<step<BAs...>, BAs...>(apply_normalize<BAs...>)
							| repeat_once<step<BAs...>, BAs...>(elim_bf_constant_01<BAs...>)
							| repeat_all<step<BAs...>, BAs...>(simplify_bf<BAs...>);

		// Do not add to dnf if the coefficient is 0
		if(is_non_terminal(tau_parser::bf_f, fm_simp->child[0]))
			return false;
		if(ranges::all_of(i, [](const auto el) {return el == 2;})) {
			//bool t = is_non_terminal(tau_parser::bf_t, fm->child[0]);
			return dnf.emplace(fm_simp, vector(0, i)), true;
		}

		auto it = dnf.find(fm_simp);
		if (it == dnf.end()) return dnf.emplace(fm_simp, vector(p==0?0:1, i)), false;
		else if (!reduce_paths(i, it->second, p)) {
			// Place coefficient together with variable assignment if no reduction happend
			it->second.push_back(i);
		} else erase_if(it->second, [](const auto& v){return v.empty();});
		return it->second.empty();
	}
	// variable was already eliminated
	if (i[p] == 2) {
		if (assign_and_reduce(fm, vars, i, dnf, p+1)) return true;
		i[p] = 0;
		return false;
	}
	// Substitute 1 and 0 for v and simplify
	const auto& v = vars[p];
	map<nso<BAs...>, nso<BAs...>> c = {{v, _1_trimmed<BAs...>}};
	auto fm_v1 = replace(fm, c) | repeat_all<step<BAs...>, BAs...>(
			simplify_bf<BAs...> | simplify_bf_more<BAs...> | apply_cb<BAs...>);
	c = {{v, _0_trimmed<BAs...>}};
	auto fm_v0 = replace(fm, c) | repeat_all<step<BAs...>, BAs...>(
			simplify_bf<BAs...> | simplify_bf_more<BAs...> | apply_cb<BAs...>);

	elim_vars_in_assignment<BAs...>(fm_v1, vars, i, p);
	if(fm_v1 == fm_v0) {
		i[p] = 2;
		if (assign_and_reduce(fm_v1, vars, i, dnf, p+1)) return true;
		i[p] = 0;
	} else {
		i[p] = 1;
		if (assign_and_reduce(fm_v1, vars, i, dnf, p+1)) return true;
		i[p] = 0;

		elim_vars_in_assignment<BAs...>(fm_v0, vars, i, p);
		i[p] = -1;
		if (assign_and_reduce(fm_v0, vars, i, dnf, p+1)) return true;
		i[p] = 0;
	}
	return false;
}

// Given a BF b, calculate the Boole normal form (DNF corresponding to the pathes to true in the BDD) of b
// where the variable order is given by the function lex_var_comp
template<typename... BAs>
nso<BAs...> bf_boole_normal_form (const nso<BAs...>& fm) {
	// Function can only be applied to a BF
	assert(is_non_terminal(tau_parser::bf, fm));

	// This defines the variable order used to calculate DNF
	// It is made canonical by sorting the variables
	auto vars = select_top(fm, is_non_terminal<tau_parser::variable, BAs...>);
	sort(vars.begin(), vars.end(), lex_var_comp<BAs...>);

	vector<int_t> i (vars.size()); // Record assignments of vars

	// Resulting DNF - make it ordered for consistency
	// Key is coefficient, value is possible variable assignments for coefficient
	map<nso<BAs...>, vector<vector<int_t>>> dnf;

	//Simplify formula as initial step
	auto fm_simp = fm | repeat_all<step<BAs...>, BAs...>(
			simplify_bf<BAs...> | simplify_bf_more<BAs...> | apply_cb<BAs...>);

	if(assign_and_reduce(fm_simp, vars, i, dnf, 0)) {
		assert(dnf.size() == 1);
		return dnf.begin()->first;
	}
	if(dnf.empty()) return _0<BAs...>;

	// Convert map structure dnf back to rewrite tree
	nso<BAs...> reduced_dnf;
	bool first = true;
	for (const auto& [coeff, paths] : dnf) {
		bool t = is_non_terminal(tau_parser::bf_t, coeff->child[0]);
		if (paths.empty()) {
			assert(!t);
			if (first) reduced_dnf = coeff;
			else reduced_dnf = build_bf_or(reduced_dnf, coeff);
			continue;
		}
		for (const auto& path : paths) {
			bool first_var = true;
			nso<BAs...> var_path;
			for (size_t k=0; k < vars.size(); ++k) {
				if (path[k] == 2) continue;
				if (first_var) var_path = path[k] == 1 ? wrap(tau_parser::bf, vars[k]) :
					build_bf_neg(wrap(tau_parser::bf, vars[k])), first_var = false;
				else var_path = path[k] == 1 ? build_bf_and(var_path, wrap(tau_parser::bf, vars[k])) :
					build_bf_and(var_path, build_bf_neg(wrap(tau_parser::bf, vars[k])));
			}
			if (first) reduced_dnf = t ? var_path : build_bf_and(coeff, var_path), first = false;
			else reduced_dnf = t ? build_bf_or(reduced_dnf, var_path) :
				build_bf_or(reduced_dnf, build_bf_and(coeff, var_path));
		}
	}
	return reduced_dnf;
}

// The needed class in order to make bf_to_reduce_dnf work with rule applying process
template<typename... BAs>
struct bf_reduce_canonical {
	nso<BAs...> operator() (const nso<BAs...>& fm) const {
		std::map<nso<BAs...>, nso<BAs...>> changes = {};
		for (const auto& bf: select_top(fm, is_non_terminal<tau_parser::bf, BAs...>)) {
			auto dnf = bf_boole_normal_form(bf);
			if (dnf != bf) changes[bf] = dnf;
		}
		if (changes.empty()) return fm;
		else return replace(fm, changes);
	}
};
template<typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& fm, const bf_reduce_canonical<BAs...>& r) {
	return r(fm);
}

// Assumes a sometimes formula in dnf with negation pushed in containing no wff_or with max nesting depth 1
template<typename... BAs>
nso<BAs...> extract_sometimes (nso<BAs...> fm) {
	// Extract the i-th child of c assuming that the child is not an always or sometimes statement
	auto extract = [](const auto& c, int_t i, auto& ex, auto& st) {
		assert(!is_non_terminal(tau_parser::wff_sometimes, trim(c->child[i])) &&
					!is_non_terminal(tau_parser::wff_always, trim(c->child[i])));
		if (auto t = find_top(c->child[i], is_non_terminal<tau_parser::io_var, BAs...>); !t.has_value())
			ex.push_back(c->child[i]);
		else st.push_back(c->child[i]);
	};

	std::map<nso<BAs...>, nso<BAs...>> l_changes = {};
	// Collect remaining nested "sometimes" formulas
	vector<nso<BAs...>> sometimes_extractions = {};
	for (const auto &inner_st: select_top(trim2(fm), is_non_terminal<tau_parser::wff_sometimes, BAs...>)) {
		l_changes[inner_st] = _T_trimmed<BAs...>;
		sometimes_extractions.push_back(inner_st);
	}
	// Collect remaining nested "always" formulas
	vector<nso<BAs...>> always_extractions = {};
	for (const auto &inner_aw: select_top(trim2(fm), is_non_terminal<tau_parser::wff_always, BAs...>)) {
		l_changes[inner_aw] = _T_trimmed<BAs...>;
		always_extractions.push_back(inner_aw);
	}
	// Apply always/sometimes extractions to fm
	if (!l_changes.empty()) fm = replace(fm, l_changes);

	vector<nso<BAs...>> extracted = {}, staying = {};
	bool no_conjunction = true;
	// Now extract from all conjuncts
	for (const nso<BAs...> &c: select_all(fm, is_non_terminal<tau_parser::wff_and, BAs...>)) {
		no_conjunction = false;
		assert(c->child.size() == 2);
		if (!is_non_terminal(tau_parser::wff_and, trim(c->child[0])))
			extract(c, 0, extracted, staying);
		if (!is_non_terminal(tau_parser::wff_and, trim(c->child[1])))
			extract(c, 1, extracted, staying);
	}
	if (no_conjunction)
		extract(trim(fm), 0, extracted, staying);

	// From here we build the formula which we will return
	nso<BAs...> extracted_fm;
	if (extracted.empty()) extracted_fm = _T<BAs...>;
	else {
		bool first = true;
		for (const auto &e: extracted) {
			if (first) { first = false; extracted_fm = e; }
			else extracted_fm = build_wff_and(extracted_fm, e);
		}
	}
	for (const auto &se: sometimes_extractions)
		extracted_fm = build_wff_and(extracted_fm, wrap(tau_parser::wff, se));
	for (const auto &ae : always_extractions)
		extracted_fm = build_wff_and(extracted_fm, wrap(tau_parser::wff, ae));
	nso<BAs...> staying_fm;
	if (staying.empty()) return extracted_fm;
	else {
		bool first = true;
		for (const auto &s: staying) {
			if (first) { first = false; staying_fm = s; }
			else staying_fm = build_wff_and(staying_fm, s);
		}
	}
	if (extracted_fm == _T<BAs...>) return build_wff_sometimes(staying_fm);
	return build_wff_and(build_wff_sometimes(staying_fm), extracted_fm);
}

// Assumes an always formula in cnf with negation pushed in containing no wff_and with max nesting depth 1
template<typename... BAs>
nso<BAs...> extract_always (nso<BAs...> fm) {
	// Extract the i-th child of c assuming that the child is not an always or sometimes statement
	auto extract = [](const auto& c, int_t i, auto& ex, auto& st) {
		assert(!is_non_terminal(tau_parser::wff_sometimes, trim(c->child[i])) &&
					!is_non_terminal(tau_parser::wff_always, trim(c->child[i])));
		if (auto t = find_top(c->child[i], is_non_terminal<tau_parser::io_var, BAs...>); !t.has_value())
			ex.push_back(c->child[i]);
		else st.push_back(c->child[i]);
	};

	std::map<nso<BAs...>, nso<BAs...>> l_changes = {};
	// Collect remaining nested "sometimes" formulas
	vector<nso<BAs...>> sometimes_extractions = {};
	for (const auto &inner_st: select_top(trim2(fm), is_non_terminal<tau_parser::wff_sometimes, BAs...>)) {
		l_changes[inner_st] = _F_trimmed<BAs...>;
		sometimes_extractions.push_back(inner_st);
	}
	// Collect remaining nested "always" formulas
	vector<nso<BAs...>> always_extractions = {};
	for (const auto &inner_aw: select_top(trim2(fm), is_non_terminal<tau_parser::wff_always, BAs...>)) {
		l_changes[inner_aw] = _F_trimmed<BAs...>;
		always_extractions.push_back(inner_aw);
	}
	// Apply always/sometimes extractions to flat_st
	if (!l_changes.empty()) fm = replace(fm, l_changes);

	// Now extract from all disjuncts
	vector<nso<BAs...>> extracted = {}, staying = {};
	bool no_disjunction = true;
	for (const nso<BAs...> &c: select_all(fm, is_non_terminal<tau_parser::wff_or, BAs...>)) {
		no_disjunction = false;
		assert(c->child.size() == 2);
		if (!is_non_terminal(tau_parser::wff_or, trim(c->child[0])))
			extract(c, 0, extracted, staying);
		if (!is_non_terminal(tau_parser::wff_or, trim(c->child[1])))
			extract(c, 1, extracted, staying);
	}
	if (no_disjunction)
		extract(trim(fm), 0, extracted, staying);

	// From here we build the formula to return based on the extractions
	nso<BAs...> extracted_fm;
	if (extracted.empty()) extracted_fm = _F<BAs...>;
	else {
		bool first = true;
		for (const auto &e: extracted) {
			if (first) { first = false; extracted_fm = e; }
			else extracted_fm = build_wff_or(extracted_fm, e);
		}
	}
	for (const auto &se: sometimes_extractions)
		extracted_fm = build_wff_or(extracted_fm, wrap(tau_parser::wff, se));
	for (const auto &ae : always_extractions)
		extracted_fm = build_wff_or(extracted_fm, wrap(tau_parser::wff, ae));
	nso<BAs...> staying_fm;
	if (staying.empty()) return extracted_fm;
	else {
		bool first = true;
		for (const auto &s: staying) {
			if (first) { first = false; staying_fm = s; }
			else staying_fm = build_wff_or(staying_fm, s);
		}
	}
	if (extracted_fm == _F<BAs...>) return build_wff_always(staying_fm);
	return build_wff_or(build_wff_always(staying_fm), extracted_fm);
}

// Recursively extract non-dependend formulas under sometimes
template<typename... BAs>
nso<BAs...> push_sometimes_always_in (nso<BAs...> fm) {
	std::map<nso<BAs...>, nso<BAs...>> g_changes = {};
	for (const auto &st : select_top_until(fm, is_non_terminal<tau_parser::wff_sometimes, BAs...>,
								is_non_terminal<tau_parser::wff_always, BAs...>)) {
		// Recursively denest sometimes and always statements contained in sometimes statement st
		auto flat_st = build_wff_sometimes(push_sometimes_always_in(st->child[0]));
		// Simplyfy current formula and convert to DNF
		flat_st = flat_st | repeat_each<step<BAs...>, BAs...>(simplify_wff<BAs...> | to_dnf_wff<BAs...>);
		if (flat_st != st) g_changes[st] = trim(flat_st);
	}
	// Apply changes
	if (!g_changes.empty()) {
		fm = replace(fm, g_changes);
		g_changes = {};
	}
	for (const auto &st : select_top_until(fm, is_non_terminal<tau_parser::wff_sometimes, BAs...>,
								is_non_terminal<tau_parser::wff_always, BAs...>)) {
		// Here a formula under "sometimes" is of the form (phi_1 && ... && phi_n)
		// Pull out each phi_i that does not contain a time variable or is "sometimes/always"
		auto simp_st = trim(extract_sometimes(wrap(tau_parser::wff, st)));
		if (st != simp_st) g_changes[st] = simp_st;
	}
	// Apply changes
	if (!g_changes.empty()) {
		fm = replace(fm, g_changes);
		g_changes = {};
	}
	for (const auto& aw : select_top_until(fm, is_non_terminal<tau_parser::wff_always, BAs...>,
								is_non_terminal<tau_parser::wff_sometimes, BAs...>)) {
		// Recursively denest sometimes and always statements contained in always statement aw
		auto flat_aw = build_wff_always(push_sometimes_always_in(aw->child[0]));
		// Simplyfy current formula and convert to CNF
		flat_aw = flat_aw | repeat_each<step<BAs...>, BAs...>(simplify_wff<BAs...> | to_cnf_wff<BAs...>);
		if (flat_aw != aw) g_changes[aw] = trim(flat_aw);
	}
	// Apply changes
	if (!g_changes.empty()) {
		fm = replace(fm, g_changes);
		g_changes = {};
	}
	for (const auto& aw : select_top_until(fm, is_non_terminal<tau_parser::wff_always, BAs...>,
								is_non_terminal<tau_parser::wff_sometimes, BAs...> )) {
		// Here the formula under "always" is of the form (phi_1 || ... || phi_n)
		// Pull out each phi_i that does not contain a time variable or is "sometimes/always"
		auto simp_aw = trim(extract_always(wrap(tau_parser::wff, aw)));
		if (aw != simp_aw) g_changes[aw] = simp_aw;
	}
	// Apply changes and return
	if (!g_changes.empty()) return replace(fm, g_changes);
	return fm;
}

// Assumes a single DNF clause and normalizes the "always" parts into one
template<typename... BAs>
nso<BAs...> pull_always_out(const nso<BAs...>& fm) {
	std::map<nso<BAs...>, nso<BAs...>> l_changes = {};
	std::vector<nso<BAs...>> collected_always_fms;
	// Collect all always statments
	bool no_conjunction = true;
	for (const auto& _and : select_all_until(fm,
								is_non_terminal<tau_parser::wff_and, BAs...>,
								is_non_terminal<tau_parser::wff_sometimes, BAs...>)) {
		no_conjunction = false;
		assert(_and->child.size() == 2);
		for (int i = 0; i < 2; ++i) {
			if (is_non_terminal(tau_parser::wff_and, trim(_and->child[i])))
				continue;
			if (is_non_terminal(tau_parser::wff_sometimes, trim(_and->child[i])))
				continue;
			if (is_non_terminal(tau_parser::wff_always, trim(_and->child[i]))) {
				l_changes[_and->child[i]] = _T<BAs...>;
				collected_always_fms.push_back(trim2(_and->child[i]));
			} else {
				l_changes[_and->child[i]] = _T<BAs...>;
				collected_always_fms.push_back(_and->child[i]);
			}
		}
	}
	if (no_conjunction) {
		if (!find_top(fm, is_non_terminal<tau_parser::wff_sometimes, BAs...>).has_value()) {
			l_changes[fm] = _T<BAs...>;
			collected_always_fms.push_back(fm);
		}
	}
	if (collected_always_fms.empty()) return fm;
	// Rebuild formula based on the extraction
	nso<BAs...> always_part;
	bool first = true;
	for (const auto& fa : collected_always_fms) {
		if (first) {first = false; always_part = fa;}
		else always_part = build_wff_and(always_part, fa);
	}
	assert(!l_changes.empty());
	return build_wff_and(build_wff_always(always_part), replace(fm, l_changes));
}

// We assume that there is no nesting of "sometimes" and "always" in fm
template<typename... BAs>
nso<BAs...> pull_sometimes_always_out(nso<BAs...> fm) {
	fm = fm | repeat_all<step<BAs...>, BAs...>(to_dnf_wff<BAs...>);
	std::map<nso<BAs...>, nso<BAs...>> changes = {};
	std::vector<nso<BAs...>> collected_fms;
	bool no_disjunction = true;
	// Collect all disjuncts which are sometimes statements and call pull_always_out on the others
	for (const auto& _or : select_all(fm, is_non_terminal<tau_parser::wff_or, BAs...>)) {
		no_disjunction = false;
		assert(_or->child.size() == 2);
		for (int i = 0; i < 2; ++i) {
			if (is_non_terminal(tau_parser::wff_or, trim(_or->child[i])))
				continue;
			if (is_non_terminal(tau_parser::wff_sometimes, trim(_or->child[i]))) {
				changes[_or->child[i]] = _F<BAs...>;
				collected_fms.push_back(trim2(_or->child[i]));
			} else {
				auto r = pull_always_out(_or->child[i]);
				if (_or->child[i] != r) changes[_or->child[i]] = r;
			}
		}
	}
	if (no_disjunction) {
		auto r = pull_always_out(fm);
		if (fm != r) changes[fm] = r;
	}
	if (!changes.empty()) fm = replace(fm, changes);
	// Merge collected formulas under "sometimes"
	if (!collected_fms.empty()) {
		nso<BAs...> r;
		bool first = true;
		for (const auto& f : collected_fms) {
			if (first) { first = false; r = f ;}
			else r = build_wff_or(r, f);
		}
		r = build_wff_sometimes(r);
		fm = build_wff_or(fm, r);
	}
	return fm;
}

// The needed class in order to make sometimes/always normalization work with normalizer
template<typename... BAs>
struct sometimes_always_normalization {
	nso<BAs...> operator() (const nso<BAs...>& fm) const {
		auto res = push_sometimes_always_in(fm)
					| repeat_all<step<BAs...>, BAs...>(simplify_wff<BAs...>)
					| reduce_wff<BAs...>;
		return pull_sometimes_always_out(res)
				| repeat_all<step<BAs...>, BAs...>(simplify_wff<BAs...>);
	}
};
template<typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& fm, const sometimes_always_normalization<BAs...>& r) {
	return r(fm);
}

// We assume that the input is a formula is in MNF (with no quantifiers whatsoever).
// We implicitly transformed into BDD form and compute one step of the SNF transformation.
template<typename...BAs>
struct to_snf_step {

	using var = nso<BAs...>;
	using vars = std::set<var>;
	using constant = std::variant<BAs...>;
	using exponent = std::set<var>;
	using literal = nso<BAs...>;
	using literals = std::set<literal>;
	using partition = std::map<exponent, literals>;
	using bdd_path = std::pair<partition /* positive */, partition /* negatives */>;

	nso<BAs...> operator()(const nso<BAs...>& form) const {
		// we select all literals, i.e. wff equalities or it negations.
		static const auto is_literal = [](const auto& n) -> bool {
			return (n | tau_parser::bf_eq).has_value();
		};
		if (auto literals = select_all(form, is_literal); !literals.empty()) {
			// we call the recursive method traverse to traverse all the paths
			// of the BDD.
			std::set<nso<BAs...>> remaining(literals.begin(), literals.end());
			bdd_path path;
			return traverse(path, remaining, form)
				| bf_reduce_canonical<BAs...>() | reduce_wff<BAs...>;
		}
		return form;
	}

private:

	static constexpr auto _or = overloaded([]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			return l | r;
	}, [](const auto&, const auto&) -> std::variant<BAs...> { throw std::logic_error("wrong types"); });

	static constexpr auto _and = overloaded([]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			return l & r;
	}, [](const auto&, const auto&) -> std::variant<BAs...> { throw std::logic_error("wrong types"); });

	static constexpr auto _neg = overloaded([]<typename T>(const T& l) -> std::variant<BAs...> {
			return ~l;
	}, [](const auto&) -> std::variant<BAs...> { throw std::logic_error("wrong types"); });

	static constexpr auto _neq_zero = overloaded([]<typename T>(const T& l) -> bool {
			return l != false;
	}, [](const auto&) -> bool { throw std::logic_error("wrong types"); });

	static constexpr auto _leq = overloaded([]<typename T>(const T& l, const T& r) -> bool {
			return (l & ~r) == false;
	}, [](const auto&, const auto&) -> bool { throw std::logic_error("wrong types"); });

	nso<BAs...> bdd_path_to_snf(const bdd_path& path, const nso<BAs...>& form) const {
		// we simplify the constant part of the formula
		// TODO (HIGH) fix simplification
		auto simplified = form | simplify_snf<BAs...>;
		return build_wff_and(normalize(path), form) | simplify_snf<BAs...>;
	}

	bdd_path add_to_negative_path(const bdd_path& path, const literal& lit) const {
		bdd_path npath;
		npath.first = path.first;
		auto lit_exp = get_exponent(lit);

		for (auto& [exp, negatives]: path.second)
			if (exp != lit_exp) npath.second[exp] = negatives;
		if (!path.second.contains(lit_exp)) npath.second[lit_exp] = { lit };
		else {
			bool insert = true;
			for (auto& n: path.second.at(lit_exp)){
				// careful with the order of the statements
				if (is_less_eq_than(n, lit)) insert = false;
				if (is_less_eq_than(lit, n)) continue;
				npath.second[lit_exp].insert(n);
			}
			if (insert) npath.second[lit_exp].insert(lit);
		}
		return npath;
	}

	bdd_path add_to_positive_path(const bdd_path& path, const literal& lit) const {
		bdd_path npath;
		npath.second = path.second;
		auto lit_exp = get_exponent(lit);

		for (auto& [exp, positives]: path.first)
			if (exp != lit_exp) npath.first[exp] = positives;
		if (!path.first.contains(lit_exp)) npath.first[lit_exp] = { lit };
		else {
			bool insert = true;
			for (auto& n: path.first.at(lit_exp)) {
				// careful with the order of the statements
				if (is_less_eq_than(lit, n)) insert = false;
				if (is_less_eq_than(n, lit)) continue;
				npath.first[lit_exp].insert(n);
			}
			if (insert) npath.first[lit_exp].insert(lit);
		}
		return npath;
	}

	nso<BAs...> traverse(const bdd_path& path, const literals& remaining, const nso<BAs...>& form) const {
		// we only cache results in release mode
		#ifdef CACHE
		static std::map<std::tuple<bdd_path, literals, nso<BAs...>>, nso<BAs...>> cache;
		if (auto it = cache.find({path, remaining, form}); it != cache.end()) return it->second;
		#endif // CACHE

		if (remaining.empty()) return bdd_path_to_snf(path, form);

		auto lit = *remaining.begin();
		auto exponent = get_exponent(lit);
		auto f = normalize_negative(path, lit) == _F<BAs...> ? _F<BAs...> : replace_with(lit, _F<BAs...>, form);
		auto t = normalize_positive(path, lit) == _F<BAs...> ? _F<BAs...> : replace_with(lit, _T<BAs...>, form);

		if (f == _F<BAs...> && t == _F<BAs...>) {
			// we only cache results in release mode
			#ifdef CACHE
			cache[{path, remaining, form}] = _F<BAs...>;
			#endif // CACHE
			return _F<BAs...>;
		}

		literals nremaining(++remaining.begin(), remaining.end());

		if (f == t) return traverse(path, nremaining, t);

		auto t_snf = _F<BAs...>, f_snf = _F<BAs...>;

		if (f != _F<BAs...>) {
			auto f_path = add_to_negative_path(path, lit);
			f_snf = traverse(f_path, nremaining, f);
		}

		if (t != _F<BAs...>) {
			auto t_path = add_to_positive_path(path, lit);
			t_snf = traverse(t_path, nremaining, t);
		}

		if (f_snf == _F<BAs...>) {
			#ifdef CACHE
			cache[{path, remaining, form}] = t_snf;
			#endif // CACHE
			return t_snf;
		}

		if (t_snf == _F<BAs...>) {
			#ifdef CACHE
			cache[{path, remaining, form}] = f_snf;
			#endif // CACHE
			return f_snf;
		}

		auto result = build_wff_or(t_snf, f_snf);
		// we only cache results in release mode
		#ifdef CACHE
		cache[{path, remaining, form}] = result;
		#endif // CACHE
		return result;
	}

	exponent get_exponent(const nso<BAs...>& n) const {
		auto is_bf_literal = [](const auto& n) -> bool {
			return (n | tau_parser::variable).has_value()
				|| (n | tau_parser::bf_neg | tau_parser::bf | tau_parser::variable).has_value();
		};
		auto all_vs = select_top(n, is_bf_literal);
		return exponent(all_vs.begin(), all_vs.end());
	}

	std::optional<constant> get_constant(const literal& lit) const {
		return find_top(lit, is_non_terminal<tau_parser::constant, BAs...>)
			| only_child_extractor<BAs...> | ba_extractor<BAs...>;
	}

	nso<BAs...> replace_with(const literal& lit, const nso<BAs...>& by, const nso<BAs...> form) const {
		std::map<nso<BAs...>, nso<BAs...>> changes = {{lit, by}};
		return replace<nso<BAs...>>(form, changes) | simplify_snf<BAs...>;
	}

	partition make_partition_by_exponent(const literals& s) const {
		partition p;
		for (auto& e: s) p[get_exponent(e)].insert(e);
		return p;
	}

	nso<BAs...> squeeze_positives(const std::set<literal>& positives, const exponent& exp) const {
		// find first element with non trivial constant
		auto first = std::find_if(positives.begin(), positives.end(), [&](const auto& l) {
			return get_constant(l).has_value();
		});
		// if there is no such element we return the first element
		if (first == positives.end()) return nso<BAs...>(*positives.begin());
		// otherwise...
		auto first_cte = build_bf_constant(get_constant(*first).value());
		auto cte = std::accumulate(++positives.begin(), positives.end(), first_cte, [&](const auto& l, const auto& r) {
			auto l_cte = get_constant(l), r_cte = get_constant(r);
			if (l_cte && r_cte)
				return build_bf_constant(std::visit(_or, l_cte.value(), r_cte.value()));
			return l;
		});

		// return the conjunction of all the same exponent literals and the accumulated constant
		auto term = std::accumulate(exp.begin(), exp.end(), cte, [](const auto& l, const auto& r) {
			return build_bf_and(l, r);
		});
		// return the corresponding wff
		return build_wff_eq(term);
	}

	// squeezed positives into one literal if possible.
	std::map<exponent, literal> squeeze_positives(const partition& positives) const {
		std::map<exponent, literal> squeezed;
		for (auto& [exponent, literals]: positives)
			squeezed[exponent] = squeeze_positives(literals, exponent);
		return squeezed;
	}

	bool is_less_eq_than(const literal& l, const literal& r) const {
		auto l_cte = get_constant(l);
		auto r_cte = get_constant(r);
		if (!l_cte) return !r_cte;
		if (!r_cte) return true;
		return std::visit(_leq, l_cte.value(), r_cte.value());
	}

	/*literals mins(const literals& ls) const {
		static auto _explicit_constant = [&](const auto& l) {
			return get_constant(l).has_value();
		};

		auto first = std::find_if(ls.begin(), ls.end(), _explicit_constant);
		if (first == ls.end()) return {*ls.begin()};

		literals mins = { *first };
		literals remaining(++first, ls.end());

		while (!remaining.empty()) {
			first = std::find_if(remaining.begin(), remaining.end(), _explicit_constant);
			if (first == remaining.end()) break;
			bool add_first = true;
			literals bigger;
			for (auto& m: mins) {
				if (is_less_eq_than(*first, m)) bigger.insert(m);
				else if (is_less_eq_than(m, *first)) {
					add_first = false; break;
				}
			}
			for (auto& b: bigger) mins.erase(b);
			remaining.erase(first);
			if (add_first) mins.insert(*first);
		}
		return mins;
	}

	partition squeeze_negatives(const partition& negatives) const {
		partition squeezed;
		for (auto& [exponent, literals]: negatives)
			squeezed[exponent] = mins(literals);
		return squeezed;
	}*/

	literal normalize(const literals& negatives, const literal& positive, const exponent& exp) const {
		// we tacitely assume that the positive literal has a constant
		// different from 1. Otherwise, the normalizer should already
		// return F.
		auto neg_positive_cte = build_bf_neg(build_bf_constant(get_constant(positive).value()));
		// now we conjunct the previous result with the constant of n
		literals lits; lits.insert(positive);
		for (auto& negative: negatives) {
			auto n_cte = build_bf_constant(get_constant(negative));
			auto nn_cte = n_cte ? build_bf_and(neg_positive_cte, n_cte.value()) : neg_positive_cte;
			auto term = build_bf_and(exp);
			auto nn = build_bf_and(nn_cte, term);
			lits.insert(build_wff_neq(nn));
		}
		return build_wff_and(lits);
	}

	// normalize each bdd path applying Corollary 3.1 from TABA book with few
	// improvements related to the handling of negative literals.
	nso<BAs...> normalize(const bdd_path& path) const {
		// if we have no positive literals we return the conjunction of all the
		// negative literals negated. PLease note that we store the positive
		// versions of the literals in the second component of the path. Thus,
		// we need to negate them or, equivalently, to build the conjunction of
		// them, we compute the negation of the the disjunction.
		if (path.first.empty()) {
			literals negs;
			for (auto& [_, lits]: path.second)
				negs.insert(lits.begin(), lits.end());
			return build_wff_neg(build_wff_or(negs));
		}

		// otherwise, let us consider lits the set of literals to be returned
		// conjuncted.
		literals lits;
		// first we squeezed positive literals...
		auto squeezed_positives = squeeze_positives(path.first);
		// ...the negatives are already squeezed (by order)
		// auto squeezed_negatives = squeeze_negatives();
		// for every negative class (same exponent) of literals...
		for (auto& [negative_exponent, negatives]: path.second) {
			// - if no positive literal has the same exponent as n, we add n to
			//   the literals
			if (!squeezed_positives.contains(negative_exponent)) {
				lits.insert(build_wff_neg(build_wff_or(negatives))); continue;
			}
			// - if the positive literal has 1 as constant we return F,
			if (!get_constant(squeezed_positives[negative_exponent]).has_value())
				return _F<BAs...>;
            // otherwise we compute the new negated literal following the Corollary 3.1
			// from TABA book.
			lits.insert(normalize(negatives, squeezed_positives.at(negative_exponent), negative_exponent));
		}

		// we also add the positive terms...
		for (auto [_, positive]: squeezed_positives)
			lits.insert(positive);
		// and return the conjunction of all the lits
		return build_wff_and(lits);
	}

	bdd_path get_relative_path(const bdd_path& path, const literal& lit) const {
		bdd_path relative_path;
		auto exp = get_exponent(lit);
		if (path.first.contains(exp)) relative_path.first[exp] = path.first.at(exp);
		if (path.second.contains(exp)) relative_path.second[exp] = path.second.at(exp);
		return relative_path;
	}

	nso<BAs...> normalize_positive(const bdd_path& path, const literal& positive) const {
		auto relative_path = get_relative_path(path, positive);
		return normalize(add_to_positive_path(relative_path, positive));
	}

	nso<BAs...> normalize_negative(const bdd_path& path, const literal& negative) const {
		auto relative_path = get_relative_path(path, negative);
		return normalize(add_to_negative_path(relative_path, negative));
	}
};

template<typename...BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const to_snf_step<BAs...>& r) {
	return r(n);
}

template<typename...BAs>
nso<BAs...> snf_bf(const nso<BAs...>& n) {
	// TODO (HIGH) give a proper implementation (call to_bdd...)
	return apply_once_definitions(n)
		| repeat_each<step<BAs...>, BAs...>(
			apply_bf_defs<BAs...>)
		| bf_reduce_canonical<BAs...>()
		| repeat_all<step<BAs...>, BAs...>(
			to_dnf_bf<BAs...>
			| simplify_bf<BAs...>
			| apply_cb<BAs...>
			| elim_eqs<BAs...>)
		// TODO (MEDIUM) review after we fully normalize bf & wff
		| reduce_bf<BAs...>;
}

// We mostly follow the Remark 3.5 from the TABA book. However, we deviate at
// some points. In particular, we assume that the formula is in MNF,
// instead of in MNF+BDD. The reason behind this is that we want to avoid the
// construction of the BDD form and then traverse it. Our aim is to build the
// BDD form and traverse it afterwards.
template<typename...BAs>
nso<BAs...> snf_wff(const nso<BAs...>& n) {
	auto [_, nn] = get_inner_quantified_wff(n);
	auto wo_defs = apply_once_definitions(nn)
		| repeat_each<step<BAs...>, BAs...>(apply_wff_defs<BAs...>);
	// in the first step we apply compute the SNF of the formula, as a result we get
	// the formula in SNF with positive equal exponent literals sqeezed.
	auto first_step = wo_defs
		| repeat_all<step<BAs...>, BAs...>(
			unsqueeze_wff<BAs...>
			| apply_cb<BAs...>
			| elim_eqs<BAs...>
			| simplify_wff<BAs...>
			| trivialities<BAs...>
			| to_mnf_snf_wff<BAs...>)
		| repeat_all<to_snf_step<BAs...>, BAs...>(to_snf_step<BAs...>())
		| repeat_all<step<BAs...>, BAs...>(to_dnf_wff<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(simplify_snf<BAs...>);
	// in the second step we compute the SNF of the negation of the the result
	// of the first step in order to squeeze the negative equal exponent literals.
	// Note that in this case we don't need to unsqueeze the formula.
	auto second_step = build_wff_neg(first_step)
		| repeat_all<step<BAs...>, BAs...>(
			apply_cb<BAs...>
			| elim_eqs<BAs...>
			| simplify_wff<BAs...>
			| trivialities<BAs...>
			| to_mnf_snf_wff<BAs...>)
		| repeat_all<to_snf_step<BAs...>, BAs...>(to_snf_step<BAs...>());
		//| repeat_all<step<BAs...>, BAs...>(to_dnf_wff<BAs...>)
		//| repeat_all<step<BAs...>, BAs...>(simplify_snf<BAs...>);
	// finally we return the negation to get the SNF of the original formula with both
	// positive and negative equal exponent literals squeezed.
	auto result = build_wff_neg(second_step)
		| repeat_all<step<BAs...>, BAs...>(to_dnf_wff<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			apply_cb<BAs...>
			| elim_eqs<BAs...>
			| simplify_wff<BAs...>
			| trivialities<BAs...>)
			| bf_reduce_canonical<BAs...>();
	return result;
}

template<typename...BAs>
nso<BAs...> build_split_wff_using(tau_parser::nonterminal type, const nso<BAs...>& a, const nso<BAs...>& b) {
	// TODO (HIGH) check formulas, should depend on the type
	if (type == tau_parser::bf_eq) return build_wff_or(build_wff_and(a, b), build_wff_and(build_wff_neg(a), build_wff_neg(b)));
	else return build_wff_and(build_wff_or(a, build_wff_neg(b)), build_wff_or(build_wff_neg(a), b));
}

template<typename...BAs>
nso<BAs...> mnf_wff(const nso<BAs...>& n) {
	auto [_, nn] = get_inner_quantified_wff(n);
	return apply_once_definitions(nn)
		| repeat_all<step<BAs...>, BAs...>(
			apply_wff_defs<BAs...>
			| to_dnf_wff<BAs...>
			| to_mnf_wff<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			to_mnf_wff<BAs...>)
		| reduce_wff<BAs...>;
}

template<typename...BAs>
nso<BAs...> mnf_bf(const nso<BAs...>& n) {
	return apply_once_definitions(n)
		| repeat_each<step<BAs...>, BAs...>(
			apply_bf_defs<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			to_dnf_bf<BAs...>
			| simplify_bf<BAs...>
			| apply_cb<BAs...>
			| elim_eqs<BAs...>)
		// TODO (MEDIUM) review after we fully normalize bf & wff
		| reduce_bf<BAs...>;
}

template<size_t type, typename...BAs>
nso<BAs...> anf(const nso<BAs...>& n) {
	// TODO (MEDIUM) write anf (using?)
	std::cout << "Not implemented yet." << std::endl;
	return n;
}

template<typename...BAs>
nso<BAs...> pnf(const nso<BAs...>& n) {
	// TODO (MEDIUM) write pnf (using?)
	std::cout << "Not implemented yet." << std::endl;
	return n;
}

} // namespace idni::tau

#endif // __NORMAL_FORMS_H__