// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentiTd cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

namespace testing = doctest;

TEST_SUITE("make_node_hook: simplify_bf and double negation") {
	// TODO (HIGH) write tests to check simplify_bf and double negation in make_node_hook
	//RULE(BF_ELIM_DOUBLE_NEGATION_0, "$X'' :=  $X.")
	//RULE(BF_SIMPLIFY_ONE_0, "1 | $X := 1.")
	//RULE(BF_SIMPLIFY_ONE_1, "$X | 1 := 1.")
	//RULE(BF_SIMPLIFY_ONE_2, "1 & $X := $X.")
	//RULE(BF_SIMPLIFY_ONE_3, "$X & 1 := $X.")
	//RULE(BF_SIMPLIFY_ONE_4, "1' := 0.")
	//RULE(BF_SIMPLIFY_ZERO_0, "0 & $X := 0.")
	//RULE(BF_SIMPLIFY_ZERO_1, "$X & 0 := 0.")
	//RULE(BF_SIMPLIFY_ZERO_2, "0 | $X := $X.")
	//RULE(BF_SIMPLIFY_ZERO_3, "$X | 0 := $X.")
	//RULE(BF_SIMPLIFY_ZERO_4, "0' := 1.")
	//RULE(BF_SIMPLIFY_SELF_0, "$X & $X := $X.")
	//RULE(BF_SIMPLIFY_SELF_1, "$X | $X := $X.")
	//RULE(BF_SIMPLIFY_SELF_2, "$X & $X' := 0.")
	//RULE(BF_SIMPLIFY_SELF_3, "$X | $X' := 1.")
	//RULE(BF_SIMPLIFY_SELF_4, "$X' & $X := 0.")
	//RULE(BF_SIMPLIFY_SELF_5, "$X' | $X := 1.")
}
