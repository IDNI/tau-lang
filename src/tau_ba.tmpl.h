// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_ba.h"

namespace idni::tau {

// template <typename...BAs>
// tau_ba<BAs...>::

template <typename...BAs>
tau_ba<BAs...>::tau_ba(rules<nso<tau_ba<BAs...>, BAs...>>& rec_relations,
	nso<tau_ba<BAs...>, BAs...>& main)
		: nso_rr({ rec_relations, main }) {}
template <typename...BAs>
tau_ba<BAs...>::tau_ba(nso<tau_ba<BAs...>, BAs...>& main) : nso_rr({main}) {}

template <typename...BAs>
auto tau_ba<BAs...>::operator<=>(const tau_ba<BAs...>&) const = default;


template <typename...BAs>
tau_ba<BAs...> tau_ba<BAs...>::operator~() const {
	// TODO (HIGH) replace by ...tau... in the future
	nso<tau_ba<BAs...>, BAs...> nmain =
		build_wff_neg<tau_ba<BAs...>, BAs...>(normalizer(nso_rr.main));
	auto nrec_relations = nso_rr.rec_relations;
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename...BAs>
tau_ba<BAs...> tau_ba<BAs...>::operator&(const tau_ba<BAs...>& other) const {
	// TODO (HIGH) replace by ...tau... in the future
	nso<tau_ba<BAs...>, BAs...> nmain =
		build_wff_and<tau_ba<BAs...>, BAs...>(nso_rr.main,
						other.nso_rr.main);
	rules<nso<tau_ba<BAs...>, BAs...>>  nrec_relations =
		merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename...BAs>
tau_ba<BAs...> tau_ba<BAs...>::operator|(const tau_ba<BAs...>& other) const {
	// TODO (HIGH) replace by ...tau... in the future
	nso<tau_ba<BAs...>, BAs...> nmain =
		build_wff_or<tau_ba<BAs...>, BAs...>(nso_rr.main,
						other.nso_rr.main);
	rules<nso<tau_ba<BAs...>, BAs...>>  nrec_relations =
		merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	tau_ba<BAs...> nnso_rr(nrec_relations, nmain);
	return nnso_rr;
}

template <typename...BAs>
tau_ba<BAs...> tau_ba<BAs...>::operator+(const tau_ba<BAs...>& other) const {
	// TODO (HIGH) replace by ...tau... in the future
	nso<tau_ba<BAs...>, BAs...> nmain =
		build_wff_xor_from_def<tau_ba<BAs...>, BAs...>(
					nso_rr.main, other.nso_rr.main);
	rules<nso<tau_ba<BAs...>, BAs...>>  nrec_relations =
		merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename...BAs>
tau_ba<BAs...> tau_ba<BAs...>::operator^(const tau_ba<BAs...>& other) const {
	return *this + other;
}

template <typename...BAs>
bool tau_ba<BAs...>::is_zero() const {
	auto normalized = normalizer<tau_ba<BAs...>, BAs...>(nso_rr);
	return !is_tau_formula_sat(normalized);
}

template <typename...BAs>
bool tau_ba<BAs...>::is_one() const {
	auto normalized = normalizer<tau_ba<BAs...>, BAs...>(nso_rr);
	return is_tau_impl( _T<tau_ba<BAs...>, BAs...>, normalized);
}

template <typename...BAs>
nso<tau_ba<BAs...>, BAs...> tau_ba<BAs...>::rename(
	const nso<tau_ba<BAs...>, BAs...>& form) const
{
	// TODO (MEDIUM) implement properly
	return form;
}

template <typename...BAs>
rewriter::rule<rr<nso<tau_ba<BAs...>, BAs...>>> tau_ba<BAs...>::rename(
	const rewriter::rule<nso<tau_ba<BAs...>, BAs...>>& rule) const
{
	// TODO (MEDIUM) implement properly
	return rule;
}

template <typename...BAs>
rules<nso<tau_ba<BAs...>, BAs...>> tau_ba<BAs...>::merge(
	const rules<nso<tau_ba<BAs...>, BAs...>>& rs1,
	const rules<nso<tau_ba<BAs...>, BAs...>>& rs2) const
{
	// TODO (MEDIUM) implement properly calling renaming
	rules<nso<tau_ba<BAs...>, BAs...>> nrs;
	nrs.insert(nrs.end(), rs1.begin(), rs1.end());
	nrs.insert(nrs.end(), rs2.begin(), rs2.end());
	return nrs;
}

template <typename...BAs>
bool operator==(const tau_ba<BAs...>& other, const bool& b) {
	return b ? other.is_one() : other.is_zero();
}

template <typename...BAs>
bool operator==(const bool& b, const tau_ba<BAs...>& other) {
	return other == b;
}

template <typename...BAs>
bool operator!=(const tau_ba<BAs...>& other, const bool& b) {
	return !(other == b);
}

template <typename...BAs>
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

template <typename...BAs>
std::optional<gssotc<BAs...>> tau_ba_factory<BAs...>::parse(const std::string& src) {
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

template <typename...BAs>
gssotc<BAs...> tau_ba_factory<BAs...>::binding(const gssotc<BAs...>& n) {
	auto source = n | tau_parser::source
		| optional_value_extractor<gssotc<BAs...>>;
	std::string src = idni::tau::make_string(
		idni::tau::tau_node_terminal_extractor<
			tau_ba<BAs...>, BAs...>, source);
	if (auto parsed = parse(src); parsed.has_value())
		return parsed.value();
	return nullptr;
}

template <typename...BAs>
std::variant<tau_ba<BAs...>, BAs...> tau_ba_factory<BAs...>::splitter_one() const {
	auto s = tau_splitter_one<tau_ba<BAs...>, BAs...>();
	return std::variant<tau_ba<BAs...>, BAs...>(tau_ba<BAs...>(s));
}

template <typename...BAs>
std::string tau_ba_factory<BAs...>::one(std::string&) const {
	return "T";
}

template <typename...BAs>
std::string tau_ba_factory<BAs...>::zero(std::string&) const {
	return "F";
}

} // namespace idni::tau

template <typename... BAs>
std::ostream& operator<<(std::ostream& os, const idni::tau::tau_ba<BAs...>& rs)
{
	return os << rs.nso_rr;
}
