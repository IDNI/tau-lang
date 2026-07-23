// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"
#include "integration/parser_helper.h"


TEST_SUITE("Configuration") {

	TEST_CASE("logging") {
		logging::trace();
	}

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

tref parse(const std::string& sample) {
	auto opts = tau::get_options{
		.parse = { .start = tau::wff },
		.reget_with_hooks = true
	};
	tref src = tree<node_t>::get(sample, opts);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}

TEST_SUITE("bv to tau tree translation") {

	TEST_CASE("1") {
		const char* sample = "(ex x x = { 1 }:bv[16]) && { 16 } : bv[7] - { 8 } = h && asdf' = j+k:bv[4] || !(j = i)";
		tref src = parse(sample);
		// tau::get(src).print_tree(std::cout << "parse tree: ") << "\n";
		subtree_map<node_t, bv> vars, free_vars;
		bv bv_tree = bv_eval_node<node_t>(src, vars, free_vars).value();
		tref tau_tree = cvc5_tree_to_tau_tree<node_t>(bv_tree);
		CHECK(tau::get(src).to_str() == tau::get(tau_tree).to_str());
	}
}

TEST_SUITE("bv division hook") {

	TEST_CASE("symbolic X / X is not folded") {
		tref src = tau::get("X:bv[8] / X:bv[8]", parse_opts_bf);
		tref folded = tau::get("{1}:bv[8]", parse_opts_bf);
		CHECK(src != nullptr);
		CHECK(src != folded);
	}

	TEST_CASE("{0} / {0} is top") {
		CHECK(tau::get("{0}:bv[8] / {0}:bv[8]", parse_opts_bf)
			== tau::get("1:bv[8]", parse_opts_bf)); // <- top element 1111....
	}

	TEST_CASE("non-zero {c} / {c} is 1") {
		CHECK(tau::get("{42}:bv[8] / {42}:bv[8]", parse_opts_bf)
			== tau::get("{1}:bv[8]", parse_opts_bf)); // <- 1 (not top element)
	}
}

TEST_SUITE("bv modulo hook") {

	TEST_CASE("1 % 0 is top") {
		CHECK(tau::get("1:bv[8] % 0:bv[8]", parse_opts_bf)
			== tau::get("1:bv[8]", parse_opts_bf)); // <- top element 1111....
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
