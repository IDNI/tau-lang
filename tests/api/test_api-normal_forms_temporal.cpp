// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The normal forms of formulas with temporal operators. Every operator, its
// negation and a few nestings go through nnf, dnf, cnf, mnf and the
// normalizer; `valid` then checks that the result is equivalent to the
// input, so the check does not depend on the printed shape. Only outputs
// occur, so `valid` reads every stream as the system's. The negation forms
// (nnf, dnf, cnf) must also leave no negation above an atom: the dualities
// U/R, S/T, G/F and W carry it down to the comparisons.

#include "test_init.h"
#include "test_tau_helpers.h"

using tau_api = api<node_t>;

namespace {

const std::string a = "o1[t] = 1";
const std::string b = "o2[t] = 1";
const std::string c = "o3[t] = 1";

std::string p(const std::string& s) { return "(" + s + ")"; }
std::string bin(const std::string& l, const char* op, const std::string& r) {
	return p(l) + " " + op + " " + p(r);
}
std::string un(const char* op, const std::string& x) {
	return std::string(op) + p(x);
}
std::string neg(const std::string& x) { return "!" + p(x); }

std::vector<std::string> temporal_formulas() {
	std::vector<std::string> fs;
	for (const char* op : { "U", "R", "W", "S", "T" }) {
		fs.push_back(bin(a, op, b));
		fs.push_back(neg(bin(a, op, b)));
	}
	for (const char* op : { "G", "F", "O", "H" }) {
		fs.push_back(un(op, a));
		fs.push_back(neg(un(op, a)));
	}
	fs.push_back(neg(un("G", un("F", a))));
	fs.push_back(neg(un("F", un("G", a))));
	fs.push_back(neg(bin(a, "U", bin(b, "R", c))));
	fs.push_back(un("G", neg(bin(a, "W", b))));
	fs.push_back(neg(bin(bin(a, "U", b), "||", un("G", a))));
	fs.push_back(neg(bin(bin(a, "S", b), "&&", bin(a, "T", c))));
	fs.push_back(neg(bin(un("H", a), "->", un("O", b))));
	fs.push_back(bin(bin(a, "U", b), "<->", un("G", c)));
	fs.push_back(bin(bin(a, "R", b), "^^", un("F", c)));
	fs.push_back(neg(bin(bin(a, "U", b), "->", bin(c, "W", a))));
	fs.push_back(neg(bin(a, "S", bin(b, "U", a))));
	return fs;
}

using nf_fn = result<std::string> (*)(const std::string&);

struct normal_form {
	const char* name;
	nf_fn fn;
	bool negation_at_atoms;
};

const normal_form normal_forms[] = {
	{ "nnf", &tau_api::nnf, true },
	{ "dnf", &tau_api::dnf, true },
	{ "cnf", &tau_api::cnf, true },
	{ "mnf", &tau_api::mnf, false },
	{ "normalize", &tau_api::normalize_formula, false },
};

bool is_temporal_or_connective(tref n) {
	switch (tau::get(n).value.nt) {
	case tau::wff_neg: case tau::wff_and: case tau::wff_or:
	case tau::wff_xor: case tau::wff_imply: case tau::wff_rimply:
	case tau::wff_equiv: case tau::wff_conditional:
	case tau::wff_always: case tau::wff_sometimes:
	case tau::wff_until: case tau::wff_release: case tau::wff_weak_until:
	case tau::wff_since: case tau::wff_trigger:
	case tau::wff_A: case tau::wff_E:
		return true;
	default: return false;
	}
}

// Every negation of fm stands over an atom: below it there is no temporal
// operator and no connective.
bool negations_at_atoms(tref fm) {
	return tau::get(fm).find_top([](tref n) {
		const auto& t = tau::get(n);
		return t.value.nt == tau::wff_neg && t.has_child()
			&& tau::get(t.first()).find_top(
				is_temporal_or_connective) != nullptr;
	}) == nullptr;
}

} // namespace

TEST_SUITE("normal forms of temporal formulas") {

	TEST_CASE("each normal form is equivalent to its input"
		* doctest::skip(!ltlsynt_available()))
	{
		for (const auto& nf : normal_forms)
			for (const auto& f : temporal_formulas()) {
				CAPTURE(nf.name);
				CAPTURE(f);
				auto nf_r = nf.fn(f);
				REQUIRE(nf_r.has_value());
				CAPTURE(nf_r.value());
				auto eq_r = tau_api::valid(
					p(f) + " <-> " + p(nf_r.value()));
				REQUIRE(eq_r.has_value());
				CHECK(eq_r.value());
			}
	}

	TEST_CASE("nnf, dnf and cnf leave negations at the atoms") {
		for (const auto& nf : normal_forms) {
			if (!nf.negation_at_atoms) continue;
			for (const auto& f : temporal_formulas()) {
				CAPTURE(nf.name);
				CAPTURE(f);
				auto fm_r = tau_api::get_formula(f);
				REQUIRE(fm_r.has_value());
				auto nf_r = std::string(nf.name) == "nnf"
					? tau_api::nnf(fm_r.value())
					: std::string(nf.name) == "dnf"
					? tau_api::dnf(fm_r.value())
					: tau_api::cnf(fm_r.value());
				REQUIRE(nf_r.has_value());
				CAPTURE(tau_api::to_str(nf_r.value()));
				CHECK(negations_at_atoms(nf_r.value()));
			}
		}
	}

	// The dualities by name: each negated operator becomes its dual.
	TEST_CASE("nnf turns each negated operator into its dual") {
		struct { std::string f; node_t::type dual; } cases[] = {
			{ neg(bin(a, "U", b)), tau::wff_release },
			{ neg(bin(a, "R", b)), tau::wff_until },
			{ neg(bin(a, "W", b)), tau::wff_release },
			{ neg(bin(a, "S", b)), tau::wff_trigger },
			{ neg(bin(a, "T", b)), tau::wff_since },
			{ neg(un("G", a)), tau::wff_sometimes },
			{ neg(un("F", a)), tau::wff_always },
			// O is T S _, H is !(T S !_)
			{ neg(un("O", a)), tau::wff_trigger },
			{ neg(un("H", a)), tau::wff_since },
			{ un("H", a), tau::wff_trigger },
		};
		for (const auto& cs : cases) {
			CAPTURE(cs.f);
			auto fm_r = tau_api::get_formula(cs.f);
			REQUIRE(fm_r.has_value());
			auto nf_r = tau_api::nnf(fm_r.value());
			REQUIRE(nf_r.has_value());
			CAPTURE(tau_api::to_str(nf_r.value()));
			CHECK(tau_api::contains(nf_r.value(), cs.dual));
			CHECK(negations_at_atoms(nf_r.value()));
		}
	}
}
