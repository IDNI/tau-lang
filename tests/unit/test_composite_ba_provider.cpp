// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// composite_ba_provider conformance tests.
//
// Phase D iter 2: CompositeBAProvider aggregates BAProvider
// implementations for ocfuncs_compile and the Coordinator's
// per-grounder routing.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "composite_ba_provider.h"
#include "ba_provider.h"

using idni::tau_lang::BAProvider;
using idni::tau_lang::BAProviderAdapter;
using idni::tau_lang::BAProviderHandle;
using idni::tau_lang::CompleteType;
using idni::tau_lang::CompositeBAProvider;
using idni::tau_lang::TypedContext;

// A tiny BAProvider impl for testing — satisfies the concept
// without depending on any particular BA's machinery.
struct StubProvider {
	std::string name;
	bool sat_verdict = false;
	std::optional<bool> realizable_verdict = std::nullopt;
	mutable size_t calls_to_satisfies = 0;
	mutable size_t calls_to_realizable = 0;

	std::vector<CompleteType> types(const TypedContext& ctx) const {
		std::vector<CompleteType> r;
		r.push_back({0, ctx.hash()});
		return r;
	}
	CompleteType project(const CompleteType& tau,
	                      const TypedContext& /*sub*/) const {
		return tau;
	}
	std::vector<CompleteType> extend(const CompleteType& sigma,
	                                  const TypedContext& /*ctx*/) const {
		return {sigma};
	}
	bool satisfies(const CompleteType& /*tau*/, idni::tref /*phi*/) const {
		++calls_to_satisfies;
		return sat_verdict;
	}
	std::optional<bool> realizable(const CompleteType& /*profile*/) const {
		++calls_to_realizable;
		return realizable_verdict;
	}
};

static_assert(BAProvider<StubProvider>,
	"StubProvider must satisfy BAProvider concept (test invariant)");

using node_t = idni::tau_lang::node<idni::tau_lang::bv,
                                     idni::tau_lang::Bool>;

TEST_SUITE("CompositeBAProvider — registration") {

	TEST_CASE("empty composite has size 0 and empty=true") {
		CompositeBAProvider c;
		CHECK(c.size() == 0);
		CHECK(c.empty());
		CHECK(c.get("nope") == nullptr);
		CHECK(!c.has("nope"));
	}

	TEST_CASE("register_provider takes ownership and is name-addressable") {
		CompositeBAProvider c;
		c.register_provider("stub_a", StubProvider{"a"});
		CHECK(c.size() == 1);
		CHECK(!c.empty());
		CHECK(c.has("stub_a"));
		CHECK(c.get("stub_a") != nullptr);
		CHECK(c.get("stub_b") == nullptr);
	}

	TEST_CASE("register multiple providers; names() returns all") {
		CompositeBAProvider c;
		c.register_provider("stub_a", StubProvider{"a"});
		c.register_provider("stub_b", StubProvider{"b"});
		c.register_provider("stub_c", StubProvider{"c"});
		CHECK(c.size() == 3);
		auto names = c.names();
		std::sort(names.begin(), names.end());
		REQUIRE(names.size() == 3);
		CHECK(names[0] == "stub_a");
		CHECK(names[1] == "stub_b");
		CHECK(names[2] == "stub_c");
	}

	// Registering DefaultBAProvider here surfaces an unrelated
	// pre-existing -Werror=unused-variable in master's
	// ba_provider.h:141 (DefaultBAProvider::types has `for (size_t
	// sid : seen_sorts)` with sid unused). The instantiation path
	// is template-dependent so the warning only fires at use sites
	// like this. Until master patches the warning, we skip the
	// DefaultBAProvider integration here. The CompositeBAProvider
	// surface itself is fully exercised by the StubProvider cases
	// below.

}

TEST_SUITE("CompositeBAProvider — types() unions") {

	TEST_CASE("types() of empty composite returns empty") {
		CompositeBAProvider c;
		TypedContext ctx;
		CHECK(c.types(ctx).empty());
	}

	TEST_CASE("types() of single provider relays its result with re-indexed indices") {
		CompositeBAProvider c;
		c.register_provider("a", StubProvider{"a"});
		TypedContext ctx;
		auto ts = c.types(ctx);
		REQUIRE(ts.size() == 1);
		CHECK(ts[0].index == 0);
	}

	TEST_CASE("types() unions across providers with disjoint indices") {
		// Each StubProvider contributes 1 type. Two providers => 2
		// types in the union, indices 0 and 1.
		CompositeBAProvider c;
		c.register_provider("a", StubProvider{"a"});
		c.register_provider("b", StubProvider{"b"});
		TypedContext ctx;
		auto ts = c.types(ctx);
		REQUIRE(ts.size() == 2);
		CHECK(ts[0].index == 0);
		CHECK(ts[1].index == 1);
	}

}

TEST_SUITE("CompositeBAProvider — satisfies / realizable") {

	TEST_CASE("satisfies routes to first provider in iter 2") {
		StubProvider a{"a"}; a.sat_verdict = true;
		StubProvider b{"b"}; b.sat_verdict = false;
		CompositeBAProvider c;
		c.register_provider("a", a);
		c.register_provider("b", b);
		CompleteType tau{0, 0};
		CHECK(c.satisfies(tau, nullptr) == true);
	}

	TEST_CASE("realizable returns first non-nullopt verdict") {
		StubProvider a{"a"};   // realizable = nullopt
		StubProvider b{"b"}; b.realizable_verdict = true;
		StubProvider d{"d"}; d.realizable_verdict = false;
		CompositeBAProvider c;
		c.register_provider("a", a);
		c.register_provider("b", b);
		c.register_provider("d", d);
		CompleteType profile{0, 0};
		auto v = c.realizable(profile);
		REQUIRE(v.has_value());
		CHECK(*v == true);   // b's verdict wins, d never consulted
	}

	TEST_CASE("realizable returns nullopt when every provider does") {
		StubProvider a{"a"};
		StubProvider b{"b"};
		CompositeBAProvider c;
		c.register_provider("a", a);
		c.register_provider("b", b);
		CompleteType profile{0, 0};
		auto v = c.realizable(profile);
		CHECK(!v.has_value());
	}

	TEST_CASE("project on empty composite throws") {
		CompositeBAProvider c;
		TypedContext ctx;
		CompleteType tau{0, 0};
		CHECK_THROWS_AS(c.project(tau, ctx), std::runtime_error);
	}

	TEST_CASE("extend on empty composite returns empty") {
		CompositeBAProvider c;
		TypedContext ctx;
		CompleteType sigma{0, 0};
		auto ext = c.extend(sigma, ctx);
		CHECK(ext.empty());
	}

}

TEST_SUITE("CompositeBAProvider — BAProvider concept conformance") {

	TEST_CASE("CompositeBAProvider satisfies the BAProvider concept") {
		// The static_assert in composite_ba_provider.h enforces this
		// at file scope. This case re-verifies at test scope so a
		// regression in the concept's wording surfaces as a test
		// failure rather than a compile error.
		static_assert(BAProvider<CompositeBAProvider>);
		// Verify a default-constructed provider has no entries
		CompositeBAProvider c;
		CHECK_FALSE(c.has("nonexistent"));
	}

	TEST_CASE("CompositeBAProvider can be wrapped by BAProviderAdapter") {
		// Useful for nested composites — a CompositeBAProvider
		// itself plugs into another CompositeBAProvider as a single
		// named entry.
		CompositeBAProvider inner;
		inner.register_provider("a", StubProvider{"a"});
		CompositeBAProvider outer;
		outer.register_provider("inner", std::move(inner));
		CHECK(outer.has("inner"));
	}

}
