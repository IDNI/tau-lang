// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_ba.h"

namespace idni::tau_lang {

template <typename...BAs>
tau_ba<BAs...>::tau_ba(rules<tau_nso_t>& rec_relations, tau_nso_t& main)
		: nso_rr({ rec_relations, main }) {}
template <typename...BAs>
tau_ba<BAs...>::tau_ba(tau_nso_t& main) : nso_rr({main}) {}

template <typename...BAs>
auto tau_ba<BAs...>::operator<=>(const tau_ba<BAs...>&) const = default;

template <typename...BAs>
tau_ba<BAs...> tau_ba<BAs...>::operator~() const {
	// TODO (HIGH) replace by ...tau... in the future
	tau_nso_t nmain =
		build_wff_neg<tau_ba_t, BAs...>(normalizer(nso_rr.main));
	auto nrec_relations = nso_rr.rec_relations;
	return tau_ba_t(nrec_relations, nmain);
}

template <typename...BAs>
tau_ba<BAs...> tau_ba<BAs...>::operator&(const tau_ba_t& other) const {
	// TODO (HIGH) replace by ...tau... in the future
	tau_nso_t nmain = build_wff_and<tau_ba_t, BAs...>(nso_rr.main,
							other.nso_rr.main);
	rules<tau_nso_t> nrec_relations =
		merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	return tau_ba_t(nrec_relations, nmain);
}

template <typename...BAs>
tau_ba<BAs...> tau_ba<BAs...>::operator|(const tau_ba_t& other) const {
	// TODO (HIGH) replace by ...tau... in the future
	tau_nso_t nmain = build_wff_or<tau_ba_t, BAs...>(normalizer(nso_rr.main),
							normalizer(other.nso_rr.main));
	rules<tau_nso_t> nrec_relations =
		merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	return tau_ba_t(nrec_relations, nmain);
}

template <typename...BAs>
tau_ba<BAs...> tau_ba<BAs...>::operator+(const tau_ba_t& other) const {
	// TODO (HIGH) replace by ...tau... in the future
	tau_nso_t nmain = build_wff_xor_from_def<tau_ba_t, BAs...>(
						normalizer(nso_rr.main),
						normalizer(other.nso_rr.main));
	rules<tau_nso_t> nrec_relations =
		merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	return tau_ba_t(nrec_relations, nmain);
}

template <typename...BAs>
tau_ba<BAs...> tau_ba<BAs...>::operator^(const tau_ba_t& other) const {
	return *this + other;
}

template <typename...BAs>
bool tau_ba<BAs...>::is_zero() const {
	auto normalized = normalizer<tau_ba_t, BAs...>(nso_rr);
	return !is_tau_formula_sat(normalized);
}

template <typename...BAs>
bool tau_ba<BAs...>::is_one() const {
	auto normalized = normalizer<tau_ba_t, BAs...>(nso_rr);
	return is_tau_impl( _T<tau_ba_t, BAs...>, normalized);
}

template <typename...BAs>
tau_depreciating<tau_ba<BAs...>, BAs...> tau_ba<BAs...>::rename(
	const tau_depreciating<tau_ba_t, BAs...>& form) const
{
	// TODO (MEDIUM) implement properly
	return form;
}

template <typename...BAs>
rewriter::rule<rr<tau_nso<BAs...>>> tau_ba<BAs...>::rename(
	const rewriter::rule<tau_nso_t>& rule) const
{
	// TODO (MEDIUM) implement properly
	return rule;
}

template <typename...BAs>
rules<tau_nso<BAs...>> tau_ba<BAs...>::merge(
	const rules<tau_nso_t>& rs1,
	const rules<tau_nso_t>& rs2) const
{
	// TODO (MEDIUM) implement properly calling renaming
	rules<tau_nso<BAs...>> nrs;
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
	auto res = apply_rr_to_formula<tau_ba<BAs...>, BAs...>(fm.nso_rr);
	res = simp_tau_unsat_valid(res);
	return tau_ba<BAs...>(res);
}

template<typename... BAs>
bool is_syntactic_one(const tau_ba<BAs...>& fm) {
	return fm.nso_rr.main == _T<tau_ba<BAs...>, BAs...>;
}

template<typename... BAs>
bool is_syntactic_zero(const tau_ba<BAs...>& fm) {
	return fm.nso_rr.main == _F<tau_ba<BAs...>, BAs...>;
}

template<typename... BAs>
auto splitter (const tau_ba<BAs...>& fm, splitter_type st) {
	auto s = tau_splitter(normalizer(fm.nso_rr), st);
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
	if (!simp_fm) return false;
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
std::optional<tau_nso<BAs...>> tau_ba_factory<BAs...>::parse(const std::string& src) {
	// parse source
	auto source = make_tau_source(src, {
			.start = tau_parser::tau_constant_source });
	if (!source) return std::optional<tau_nso_t>{};
	auto rr = make_nso_rr_using_factory<tau_ba_t, BAs...>(source);
	if (!rr) return std::optional<tau_nso_t>{};
	// cvompute final result
	tau_ba_t t(rr.value().rec_relations, rr.value().main);
	return std::optional<tau_nso_t>{
		rewriter::make_node<tau_sym<tau_ba_t, BAs...>>(t, {}) };
}

template <typename...BAs>
tau_nso<BAs...> tau_ba_factory<BAs...>::binding(const tau_nso_t& n) {
	auto source = n | tau_parser::source
		| optional_value_extractor<tau_nso_t>;
	std::string src = idni::tau_lang::make_string(
		idni::tau_lang::tau_node_terminal_extractor<tau_ba_t, BAs...>,
		source);
	if (auto parsed = parse(src); parsed.has_value())
		return parsed.value();
	return nullptr;
}

template <typename...BAs>
std::variant<tau_ba<BAs...>, BAs...> tau_ba_factory<BAs...>::splitter_one() const {
	auto s = tau_splitter_one<tau_ba_t, BAs...>();
	return std::variant<tau_ba_t, BAs...>(tau_ba_t(s));
}

template <typename...BAs>
std::string tau_ba_factory<BAs...>::one(std::string&) const {
	return "T";
}

template <typename...BAs>
std::string tau_ba_factory<BAs...>::zero(std::string&) const {
	return "F";
}

} // namespace idni::tau_lang

template <typename... BAs>
size_t std::hash<idni::tau_lang::tau_ba<BAs...>>::operator()(const idni::tau_lang::tau_ba<BAs...>& f) const noexcept {
	using namespace idni::tau_lang;
	return hash<rr<tau_depreciating<tau_ba<BAs...>, BAs...>>>{}(f.nso_rr);
}

template <typename... BAs>
std::ostream& operator<<(std::ostream& os, const idni::tau_lang::tau_ba<BAs...>& rs)
{
	return os << rs.nso_rr;
}
