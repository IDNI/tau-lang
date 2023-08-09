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

#include "../src/doctest.h"
#include "../src/program.h"
#include "../src/bool.h"
#include "../src/bdd_handle.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

/*TEST_SUITE("testing order") {

	TEST_CASE("bool") {
		auto t = std::variant<bool>(true);
		auto f = std::variant<bool>(false);
		CHECK( (f <=> t) == std::strong_ordering::less );
	}

	struct Char {
		char c;
		Char(char c) : c(c) {}
		auto operator<=>(const Char& o) const = default;
	};

	TEST_CASE("Char") {
		auto b = std::variant<Char>(Char('b'));
		auto a = std::variant<Char>(Char('a'));
		CHECK( (a <=> b) == std::strong_ordering::less );
	}

	TEST_CASE("Bool") {
		auto t = std::variant<Bool>(Bool(true));
		auto f = std::variant<Bool>(Bool(false));
		CHECK( (f <=> t) == std::strong_ordering::less );
	}
}*/

TEST_SUITE("get") {

}

TEST_SUITE("is_ignore") {

}

TEST_SUITE("is_capture") {

}

TEST_SUITE("non_terminal") {

}

TEST_SUITE("bind") {

	sp_tau_node<Bool> make_statement(const sp_tau_source_node& source) {
	tauify<Bool> tf;
	map_transformer<decltype(tf), sp_tau_source_node, sp_tau_node<Bool>> transform(tf);
	true_predicate<sp_node<tau_source_sym>> always;
	return post_order_traverser<decltype(transform), decltype(always),
		sp_node<tau_source_sym>, sp_tau_node<Bool>>(transform, always)(source);
	}

	sp_tau_node<Bool> make_binding(const sp_tau_node<Bool>& statement, const bindings<Bool>& bs) {
		true_predicate<sp_tau_node<Bool>> always;
		name_binder<Bool> nb(bs);
		bind_transformer<decltype(nb), Bool> binder(nb); 
		return post_order_traverser<decltype(binder), decltype(always), sp_tau_node<Bool>>(binder, always)(statement);
	}

	TEST_CASE("binding: given one statement with no bindigns, the binding process returns the same statement.") {
		static constexpr char* sample =	"bf_neg ( bf_neg ( $X ) ) = $X .";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		bindings<Bool> bs; bs["binding"] = { Bool(true) };
		auto binded = make_binding(statement, bs);
		CHECK( binded == statement );
	}

	TEST_CASE("binding: given one statement with one binding, the binding process returns the statement with the binding replaced.") {
		static constexpr char* sample =	"bf_neg ( bf_neg ( { binding } ) ) = { binding } .";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		bindings<Bool> bs; bs["binding"] = { Bool(true) };
		auto binded = make_binding(statement, bs);
		CHECK( binded != statement );
	}

	TEST_CASE("binding: given one statement with one non-matching binding, the binding process returns the original statement.") {
		static constexpr char* sample =	"bf_neg ( bf_neg ( { non_matching } ) ) = { non_matching } .";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		bindings<Bool> bs; bs["binding"] = { Bool(true) };
		auto binded = make_binding(statement, bs);
		CHECK( binded == statement );
	}
}

TEST_SUITE("callback_applier") {

}

TEST_SUITE("apply") {

}

TEST_SUITE("make_tau_source") {

	static constexpr char* sample =	
		"bf_neg ( bf_neg ( $X ) ) = $X .";

	TEST_CASE("one_rule_n_main") {
		auto src = make_tau_source(sample);
		CHECK( true );
	}
}

TEST_SUITE("make_library") {

	static constexpr char* sample =	"bf_neg ( bf_neg ( $X ) ) = $X .";

	TEST_CASE("one_rule") {
		auto src = make_tau_source(sample);
		auto lib = make_library<Bool>(src);
		CHECK( true );
	}
}

/* TEST_SUITE("make_program") {

	static constexpr char* sample =	
		"bf_neg ( bf_neg ( $X ) ) = $X ."
		" { 0 } .";

	TEST_CASE("one_rule_n_main") {
		auto src = make_tau_source(sample);
		bindings<Bool> bs;
		tau_source t(src);
		auto lib = make_program<Bool>(t, bs);
		CHECK( true );
	}

}*/

