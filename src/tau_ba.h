// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __TAU_H__
#define __TAU_H__

#include <iostream>

#include "satisfiability.h"
#include "splitter.h"

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

namespace idni::tau {

// Check https://gcc.gnu.org/bugzilla/show_bug.cgi?id=102609 to follow up on
// the implementation of "Deducing this" on gcc.
// See also (https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0847r7.html)
// and https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/ for how to use
// "Deducing this" on CRTP.

template<typename...BAs>
struct tau_ba;

template<typename...BAs>
bool is_satisfiable(const rr<nso<tau_ba<BAs...>, BAs...>>& tau_spec);

template<typename...BAs>
struct tau_ba {

	tau_ba(rules<nso<tau_ba<BAs...>, BAs...>>& rec_relations,
		nso<tau_ba<BAs...>, BAs...>& main)
			: nso_rr({ rec_relations, main }) {}
	tau_ba(nso<tau_ba<BAs...>, BAs...>& main) : nso_rr({main}) {}

	auto operator<=>(const tau_ba<BAs...>&) const = default;

	tau_ba<BAs...> operator~() const {
		// TODO (HIGH) replace by ...tau... in the future
		nso<tau_ba<BAs...>, BAs...> nmain =
			build_wff_neg<tau_ba<BAs...>, BAs...>(normalizer(nso_rr.main));
		auto nrec_relations = nso_rr.rec_relations;
		return tau_ba<BAs...>(nrec_relations, nmain);
	}

	tau_ba<BAs...> operator&(const tau_ba<BAs...>& other) const {
		// TODO (HIGH) replace by ...tau... in the future
		nso<tau_ba<BAs...>, BAs...> nmain =
			build_wff_and<tau_ba<BAs...>, BAs...>(nso_rr.main,
							other.nso_rr.main);
		rules<nso<tau_ba<BAs...>, BAs...>>  nrec_relations =
			merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
		return tau_ba<BAs...>(nrec_relations, nmain);
	}

	tau_ba<BAs...> operator|(const tau_ba<BAs...>& other) const {
		// TODO (HIGH) replace by ...tau... in the future
		nso<tau_ba<BAs...>, BAs...> nmain =
			build_wff_or<tau_ba<BAs...>, BAs...>(nso_rr.main,
							other.nso_rr.main);
		rules<nso<tau_ba<BAs...>, BAs...>>  nrec_relations =
			merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
		tau_ba<BAs...> nnso_rr(nrec_relations, nmain);
		return nnso_rr;
	}

	tau_ba<BAs...> operator+(const tau_ba<BAs...>& other) const {
		// TODO (HIGH) replace by ...tau... in the future
		nso<tau_ba<BAs...>, BAs...> nmain =
			build_wff_xor_from_def<tau_ba<BAs...>, BAs...>(
						nso_rr.main, other.nso_rr.main);
		rules<nso<tau_ba<BAs...>, BAs...>>  nrec_relations =
			merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
		return tau_ba<BAs...>(nrec_relations, nmain);
	}

	tau_ba<BAs...> operator^(const tau_ba<BAs...>& other) const {
		return *this + other;
	}

	bool is_zero() const {
		auto normalized = normalizer<tau_ba<BAs...>, BAs...>(nso_rr);
		return !is_tau_formula_sat(normalized);
	}

	bool is_one() const {
		auto normalized = normalizer<tau_ba<BAs...>, BAs...>(nso_rr);
		return is_tau_impl( _T<tau_ba<BAs...>, BAs...>, normalized);
	}

	// the type is ewquivalent to tau_spec<BAs...>
	const rr<nso<tau_ba<BAs...>, BAs...>> nso_rr;

	private:

	nso<tau_ba<BAs...>, BAs...> rename(
		const nso<tau_ba<BAs...>, BAs...>& form) const
	{
		// TODO (MEDIUM) implement properly
		return form;
	}

	rewriter::rule<rr<nso<tau_ba<BAs...>, BAs...>>> rename(
		const rewriter::rule<nso<tau_ba<BAs...>, BAs...>>& rule) const
	{
		// TODO (MEDIUM) implement properly
		return rule;
	}

	rules<nso<tau_ba<BAs...>, BAs...>> merge(
		const rules<nso<tau_ba<BAs...>, BAs...>>& rs1,
		const rules<nso<tau_ba<BAs...>, BAs...>>& rs2) const
	{
		// TODO (MEDIUM) implement properly calling renaming
		rules<nso<tau_ba<BAs...>, BAs...>> nrs;
		nrs.insert(nrs.end(), rs1.begin(), rs1.end());
		nrs.insert(nrs.end(), rs2.begin(), rs2.end());
		return nrs;
	}
};

template<typename...BAs>
bool operator==(const tau_ba<BAs...>& other, const bool& b) {
	return b ? other.is_one() : other.is_zero();
}

template<typename...BAs>
bool operator==(const bool& b, const tau_ba<BAs...>& other) {
	return other == b;
}

template<typename...BAs>
bool operator!=(const tau_ba<BAs...>& other, const bool& b) {
	return !(other == b);
}

template<typename...BAs>
bool operator!=(const bool& b, const tau_ba<BAs...>& other) {
	return !(other == b);
}

template<typename... BAs>
auto normalize (const tau_ba<BAs...>& fm) {
	auto res = normalizer<tau_ba<BAs...>, BAs...>(fm.nso_rr);
	return tau_ba<BAs...>(res);
}

template<typename... BAs>
auto splitter (const tau_ba<BAs...>& fm, splitter_type st) {
	auto s = tau_splitter(fm.nso_rr.main, st);
	return tau_ba<BAs...>(s);
}

template<typename... BAs>
auto tau_splitter_one () {
	return tau_bad_splitter<BAs...>();
}

template<typename... BAs>
bool is_closed (const tau_ba<BAs...>& fm) {
	using p = tau_parser;
	auto simp_fm = apply_rr_to_formula(fm.nso_rr);
	if (find_top(simp_fm, is_non_terminal<tau_parser::ref, tau_ba<BAs...>, BAs...>))
		return false;
	auto vars = get_free_vars_from_nso(simp_fm);
	for (const auto& v : vars) {
		if (!(is_child_non_terminal(p::io_var, v) ||
			is_child_non_terminal(p::uninterpreted_constant, v)))
			return false;
	}
	return true;
}

template<typename...BAs>
using gssotc = nso<tau_ba<BAs...>, BAs...>;

template<typename...BAs>
using tau_spec = rr<gssotc<BAs...>>;


template<typename...BAs>
struct tau_ba_factory {

	std::optional<gssotc<BAs...>> parse(const std::string& src) {
		// parse source
		auto source = make_tau_source(src, {
				.start = tau_parser::tau_constant_source });
		if (!source) return std::optional<gssotc<BAs...>>{};
		auto rr = make_nso_rr_using_factory<tau_ba<BAs...>, BAs...>(
									source);
		if (!rr) return std::optional<gssotc<BAs...>>{};
		// cvompute final result
		tau_ba<BAs...> t(rr.value().rec_relations, rr.value().main);
		return std::optional<gssotc<BAs...>>{
			rewriter::make_node<tau_sym<tau_ba<BAs...>, BAs...>>(
				t, {}) };
	}

	gssotc<BAs...> binding(const gssotc<BAs...>& n) {
		auto source = n | tau_parser::source
			| optional_value_extractor<gssotc<BAs...>>;
		std::string src = idni::tau::make_string(
			idni::tau::tau_node_terminal_extractor<
				tau_ba<BAs...>, BAs...>, source);
		if (auto parsed = parse(src); parsed.has_value())
			return parsed.value();
		return nullptr;
	}

	std::variant<tau_ba<BAs...>, BAs...> splitter_one() const {
		auto s = tau_splitter_one<tau_ba<BAs...>, BAs...>();
		return std::variant<tau_ba<BAs...>, BAs...>(tau_ba<BAs...>(s));
	}

	std::string one(std::string&) const {
		return "T";
	}

	std::string zero(std::string&) const {
		return "F";
	}
};

} // namespace idni::tau

// << for printing tau_ba's form
template <typename... BAs>
std::ostream& operator<<(std::ostream& os, const idni::tau::tau_ba<BAs...>& rs)
{
	return os << rs.nso_rr;
}

#endif // __TAU_H__