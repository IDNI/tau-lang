// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// What every Boolean algebra of the configured pack must satisfy, checked
// against the pack itself rather than against a list of algebras.
//
// test_ba_descriptor_pack.cpp proves a descriptor compiles; this proves it
// behaves. It folds over node_t::bas_tuple, so an algebra joins simply by being
// in TAU_BAS -- including one registered from outside the source tree, which
// otherwise has no runtime coverage at all. It declares no requirement in
// tests/CMakeLists.txt for the same reason: it must build in every pack.
//
// The laws run over the two constants a descriptor hands out, its literal one
// and zero. That is a two-element sublattice -- enough to catch a descriptor
// wired to the wrong predicate, not enough to test an algebra's theory, which
// stays with its own suite.

#include <string>
#include <tuple>
#include <utility>

#include "test_init.h"
#include "tau.h"

using namespace idni::tau_lang;

using node_t = tau_pack::node_t;
using tau = tree<node_t>;

namespace {

// A constant parsed from the descriptor's own literal, or nothing when the
// literal does not round-trip through parse().
template <typename BA>
std::optional<BA> parsed_literal(const std::string& src, tref type) {
	auto c = ba_descriptor<BA, node_t>::parse(src, type);
	if (!c) return std::nullopt;
	if (!std::holds_alternative<BA>(c->first)) return std::nullopt;
	return std::get<BA>(c->first);
}

template <typename BA>
void check_type_system() {
	using desc = ba_descriptor<BA, node_t>;
	tref type = desc::type_tree();
	REQUIRE(type != nullptr);
	CHECK(std::string(desc::type_name).size() > 0);
	CHECK(desc::matches_type(type));
	CHECK(desc::owns_type(type));
	// the name inside the type tree is the descriptor's own
	CHECK(tau::get(type)[0].get_string() == std::string(desc::type_name));

	// a parameterized algebra round-trips its parameter; an unparameterized
	// one answers nullopt and type_tree() is its only type
	if (auto param = desc::type_param(type); param)
		CHECK(desc::type_tree_for(param.value()) == type);
	CHECK(desc::owns_type(ba_types<node_t>::id(type)));
}

template <typename BA>
void check_literals_and_predicates() {
	using desc = ba_descriptor<BA, node_t>;
	tref type = desc::type_tree();
	const std::string one_src = desc::literal_one(type);
	const std::string zero_src = desc::literal_zero(type);
	CHECK(one_src.size() > 0);
	CHECK(zero_src.size() > 0);
	CHECK(one_src != zero_src);

	auto one = parsed_literal<BA>(one_src, type);
	auto zero = parsed_literal<BA>(zero_src, type);
	REQUIRE(one.has_value());
	REQUIRE(zero.has_value());

	CHECK(desc::is_one(one.value()));
	CHECK(desc::is_zero(zero.value()));
	CHECK_FALSE(desc::is_zero(one.value()));
	CHECK_FALSE(desc::is_one(zero.value()));
	CHECK(desc::is_closed(one.value()));
	CHECK(desc::is_closed(zero.value()));
	// syntactic is the stronger claim of the two
	if (desc::is_syntactic_one(one.value())) CHECK(desc::is_one(one.value()));
	if (desc::is_syntactic_zero(zero.value()))
		CHECK(desc::is_zero(zero.value()));

	// what every alternative of the constants variant owes generic core
	CHECK(one.value() == true);
	CHECK(zero.value() == false);
	std::stringstream so, sz;
	so << one.value();
	sz << zero.value();
	CHECK(so.str().size() > 0);
	CHECK(sz.str().size() > 0);
}

// The laws below compare values, and comparison is where an algebra backed by
// an external oracle leaves the process -- so an algebra declaring uses_oracle
// sits them out.
template <typename BA>
void check_boolean_laws() {
	using desc = ba_descriptor<BA, node_t>;
	tref type = desc::type_tree();
	auto one_opt = parsed_literal<BA>(desc::literal_one(type), type);
	auto zero_opt = parsed_literal<BA>(desc::literal_zero(type), type);
	REQUIRE(one_opt.has_value());
	REQUIRE(zero_opt.has_value());
	const BA one = one_opt.value(), zero = zero_opt.value();

	// An algebra's operators may build a term rather than evaluate one -- bv is
	// a cvc5::Term, compared structurally -- so the laws are stated the way
	// core relies on them: equal after normalization.
	auto eq = [](const BA& a, const BA& b) {
		return desc::normalize(a) == desc::normalize(b);
	};
	CHECK(eq(~one, zero));
	CHECK(eq(~zero, one));
	CHECK(eq(one & one, one));
	CHECK(eq(one | one, one));
	CHECK(eq(one & zero, zero));
	CHECK(eq(one | zero, one));
	CHECK(eq(one ^ one, zero));
	CHECK(eq(one & ~one, zero));
	CHECK(eq(one | ~one, one));
	CHECK(eq(~~one, one));
	// De Morgan over the pair
	CHECK(eq(~(one & zero), ~one | ~zero));
	CHECK(eq(~(one | zero), ~one & ~zero));

	// equal values hash equally
	CHECK(std::hash<BA>{}(one) == std::hash<BA>{}(one));

	// normalization fixes what is already normal
	CHECK(desc::normalize(one) == one);
	CHECK(desc::normalize(zero) == zero);
	CHECK(desc::normalize(desc::normalize(one)) == desc::normalize(one));
}

template <typename BA>
void check_ba() {
	using desc = ba_descriptor<BA, node_t>;
	SUBCASE(desc::type_name) {
		check_type_system<BA>();
		check_literals_and_predicates<BA>();
		if constexpr (!requires { requires desc::uses_oracle; })
			check_boolean_laws<BA>();
	}
}

template <typename... BAs>
void check_pack(std::tuple<BAs...>*) {
	(check_ba<BAs>(), ...);
}

} // namespace

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("every algebra of the configured pack") {

	TEST_CASE("satisfies its descriptor's contract") {
		check_pack(static_cast<typename node_t::bas_tuple*>(nullptr));
	}
}
