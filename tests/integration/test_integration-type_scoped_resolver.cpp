// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "ba_types_inference.h"

TEST_SUITE("Configuration") {

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

tau::get_options parse_wff_no_infer() {
	static tau::get_options opts{ .parse = { .start = tau::wff },
		.infer_ba_types = false,
		.reget_with_hooks = false };
	return opts;
}

tref parse(const std::string& sample, tau::get_options opts = parse_wff_no_infer()) {
	auto src = tree<node_t>::get(sample, opts);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}


TEST_SUITE("type_scoped_resolver") {

	TEST_CASE("merging in the same scope") {
		type_scoped_resolver<node_t> r;
		tref a = parse("T"); // just an existing tref for testing
		tref b = parse("F"); // just an existing tref for testing
		r.insert(a);
		r.insert(b);
		size_t t = 1;
		CHECK(!std::holds_alternative<inference_error>(r.assign(a, t)));
		CHECK(!std::holds_alternative<inference_error>(r.merge(a, b)));
		CHECK(r.type_id_of(a) == r.type_id_of(b));
		CHECK(r.type_id_of(a) == t);
	}

	TEST_CASE("merging in different scopes") {
		type_scoped_resolver<node_t> r;
		tref a = parse("T"); // just an existing tref for testing
		tref b = parse("F"); // just an existing tref for testing
		size_t t = 1;
		r.insert(a);
		r.open({{b, t}});
		CHECK(!std::holds_alternative<inference_error>(r.merge(a, b)));
		CHECK(r.type_id_of(a) == r.type_id_of(b));
		CHECK(r.type_id_of(a) == t);
	}

	TEST_CASE("merging conflicting types in the same scope") {
		type_scoped_resolver<node_t> r;
		tref a = parse("T"); // just an existing tref for testing
		tref b = parse("F"); // just an existing tref for testing
		r.insert(a);
		r.insert(b);
		size_t t1 = 1;
		size_t t2 = 2;
		CHECK(!std::holds_alternative<inference_error>(r.assign(a, t1)));
		CHECK(!std::holds_alternative<inference_error>(r.assign(b, t2)));
		CHECK(std::holds_alternative<inference_error>(r.merge(a, b))); // conflicting types
	}

	TEST_CASE("merging conflicting types in different scopes") {
		type_scoped_resolver<node_t> r;
		tref a = parse("T"); // just an existing tref for testing
		tref b = parse("F"); // just an existing tref for testing
		size_t t1 = 1;
		size_t t2 = 2;
		r.insert(a);
		CHECK(!std::holds_alternative<inference_error>(r.assign(a, t1)));
		r.open({{b, t2}});
		CHECK(std::holds_alternative<inference_error>(r.merge(a, b))); // conflicting types
	}
}
