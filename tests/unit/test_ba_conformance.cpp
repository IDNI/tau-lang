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

#include <optional>
#include <sstream>
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

// ── Optional capabilities ────────────────────────────────────────────────────
//
// Every block below is probed with `if constexpr (requires { … })` and never by
// name, the way ba_pack_traits.h dispatches. A block that no algebra of the
// configured pack implements compiles to nothing, which is the point: the empty
// case is what a reduced pack exercises.

// Classification is a law, not a convention: an algebra is a Boolean algebra or
// an omega-categorical non-BA theory, never both. The folds core actually asks
// must agree with what the descriptor declares -- that seam is the whole point
// of a capability, and nothing else checks it.
template <typename BA>
void check_classification() {
	using desc = ba_descriptor<BA, node_t>;
	const size_t ba_type = ba_types<node_t>::id(desc::type_tree());

	static_assert(!(desc::atomless && desc::non_aba_omcat),
		"a BA is atomless or non-ABA omcat, never both");

	// doctest cannot decompose && inside its macros, so a predicate compared
	// here is named first rather than written inline
	const bool omcat_fold = pack_type_is_non_aba_omcat<node_t>(ba_type);
	CHECK(omcat_fold == desc::non_aba_omcat);

	constexpr bool declares_arith = requires { requires desc::arith_ops; };
	const bool arith_fold = pack_type_has_arith_ops<node_t>(ba_type);
	CHECK(arith_fold == declares_arith);
}

// Atomlessness is the claim that between any two distinct elements a third
// exists; splitter is its constructive witness, so an algebra declaring the
// first owes a working second. Comparison-based, so an oracle-backed algebra
// sits it out.
template <typename BA>
void check_splitter() {
	using desc = ba_descriptor<BA, node_t>;
	if constexpr (!desc::atomless) return;
	else {
		tref type = desc::type_tree();
		auto one_opt = parsed_literal<BA>(desc::literal_one(type), type);
		auto zero_opt = parsed_literal<BA>(desc::literal_zero(type), type);
		REQUIRE(one_opt.has_value());
		REQUIRE(zero_opt.has_value());
		const BA one = one_opt.value(), zero = zero_opt.value();
		auto eq = [](const BA& a, const BA& b) {
			return desc::normalize(a) == desc::normalize(b);
		};

		for (auto st : { splitter_type::lower, splitter_type::middle,
				splitter_type::upper })
		{
			const BA y = desc::normalize(desc::splitter(one, st));
			// a proper sub-element: neither endpoint, and below one
			CHECK_FALSE(eq(y, zero));
			CHECK_FALSE(eq(y, one));
			CHECK(eq(y | one, one));
			CHECK(eq(y & one, y));
		}

		tref s = desc::splitter_one(type);
		CHECK(s != nullptr);
	}
}

// The constant a term carries, located rather than indexed: how deeply a
// builder wraps it is not part of any contract. Empty when the term carries
// none, which is the ordinary case for a canonicalized zero or one.
template <typename BA>
std::optional<BA> constant_of(tref n) {
	if (n == nullptr) return std::nullopt;
	tref cn = tau::get(n).find_top(
		[](tref x) { return is<node_t>(x, tau::ba_constant); });
	if (cn == nullptr) return std::nullopt;
	auto c = tau::get(cn).get_ba_constant();
	if (!std::holds_alternative<BA>(c)) return std::nullopt;
	return std::get<BA>(c);
}

// What a term-producing capability owes: what it hands back must read back as
// the same value.
//
// The shape it hands back varies and that is core's business, not the
// algebra's: a syntactically zero or one constant is canonicalized into a
// *typed* bf_f/bf_t (hooks_bf.tmpl.h, cte), so bv's zero carries no ba_constant
// at all while qlt's {0} -- not qlt's zero, a dense linear order having no
// bottom -- stays one. serialize_constant renders both as the type's own
// literal, so the round trip is one statement covering both.
template <typename BA>
void check_term_round_trip(tref term, size_t ba_type, tref type) {
	using desc = ba_descriptor<BA, node_t>;
	using tt = typename tau::traverser;

	std::stringstream ss;
	REQUIRE(serialize_constant<node_t>(ss, term, ba_type));
	const std::string src = ss.str();
	REQUIRE(src.size() > 0);

	auto back = parsed_literal<BA>(src, type);
	REQUIRE(back.has_value());

	if (auto carried = constant_of<BA>(term))
		CHECK(desc::normalize(back.value())
			== desc::normalize(carried.value()));
	else if (tt(term) | tau::bf_f) CHECK(desc::is_zero(back.value()));
	else if (tt(term) | tau::bf_t) CHECK(desc::is_one(back.value()));
}

// zero_constant is the term core initializes an output of this type to
// (interpreter.tmpl.h, emit_default_zero), which for an algebra with no bottom
// is not its zero -- so what is asserted is the round trip, not the value.
template <typename BA>
void check_constant_builders() {
	using desc = ba_descriptor<BA, node_t>;
	tref type = desc::type_tree();
	const size_t ba_type = ba_types<node_t>::id(type);

	if constexpr (requires { desc::zero_constant(ba_type); }) {
		tref zt = desc::zero_constant(ba_type);
		REQUIRE(zt != nullptr);
		CHECK(is<node_t>(zt, tau::bf));
		if (auto z = constant_of<BA>(zt)) CHECK(desc::is_closed(z.value()));
		check_term_round_trip<BA>(zt, ba_type, type);
	}

	if constexpr (requires { desc::value_constant(ba_type, size_t{0}); }) {
		tref vt = desc::value_constant(ba_type, 0);
		REQUIRE(vt != nullptr);
		CHECK(is<node_t>(vt, tau::bf));
		if (auto v = constant_of<BA>(vt)) CHECK(desc::is_closed(v.value()));
		check_term_round_trip<BA>(vt, ba_type, type);
	}
}

// How an algebra renders, when its own operator<< is not what Tau should print,
// and whether the algebra offered as the Boolean carrier can really carry one.
template <typename BA>
void check_rendering() {
	using desc = ba_descriptor<BA, node_t>;
	tref type = desc::type_tree();

	if constexpr (requires { desc::print_constant(std::declval<BA>()); }) {
		auto one = parsed_literal<BA>(desc::literal_one(type), type);
		auto zero = parsed_literal<BA>(desc::literal_zero(type), type);
		REQUIRE(one.has_value());
		REQUIRE(zero.has_value());
		const std::string po = desc::print_constant(one.value());
		const std::string pz = desc::print_constant(zero.value());
		CHECK(po.size() > 0);
		CHECK(pz.size() > 0);
		CHECK(po != pz);
	}

	if constexpr (requires { requires desc::can_host_bool; }) {
		// the carrier's own type, which need not be type_tree()
		tref carrier = type;
		if constexpr (requires { desc::bool_carrier_type(); })
			carrier = desc::bool_carrier_type();
		REQUIRE(carrier != nullptr);
		// a carrier must render and parse back a plain 1 and 0
		auto one = parsed_literal<BA>(desc::literal_one(carrier), carrier);
		auto zero = parsed_literal<BA>(desc::literal_zero(carrier), carrier);
		REQUIRE(one.has_value());
		REQUIRE(zero.has_value());
		CHECK(desc::is_one(one.value()));
		CHECK(desc::is_zero(zero.value()));
	}
}

template <typename BA>
void check_ba() {
	using desc = ba_descriptor<BA, node_t>;
	SUBCASE(desc::type_name) {
		check_type_system<BA>();
		check_literals_and_predicates<BA>();
		check_classification<BA>();
		check_constant_builders<BA>();
		check_rendering<BA>();
		if constexpr (!requires { requires desc::uses_oracle; }) {
			check_boolean_laws<BA>();
			check_splitter<BA>();
		}
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
