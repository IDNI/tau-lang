// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_ba.h"
#include "splitter.h"

namespace idni::tau_lang {

template <typename... BAs>
tau_ba<BAs...>::tau_ba(rewriter::rules& rec_relations, htree::sp main)
		: nso_rr({ rec_relations, main }) {}
template <typename... BAs>
tau_ba<BAs...>::tau_ba(htree::sp main) : nso_rr({main}) {}

template <typename... BAs>
auto tau_ba<BAs...>::operator<=>(const tau_ba<BAs...>&) const = default;

template <typename... BAs>
tau_ba<BAs...> tau_ba<BAs...>::operator~() const {
	// TODO (HIGH) replace by ...tau... in the future
	tref nmain = tau::build_wff_neg(
			normalizer<tau_ba_t, BAs...>(nso_rr.main.get()));
	auto nrec_relations = nso_rr.rec_relations;
	return tau_ba_t(nrec_relations, nmain);
}

template <typename... BAs>
tau_ba<BAs...> tau_ba<BAs...>::operator&(const tau_ba_t& other) const {
	// TODO (HIGH) replace by ...tau... in the future
	tref nmain = tau::build_wff_and(
			nso_rr.main->get(), other.nso_rr.main->get());
	auto nrec_relations =
		merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	return tau_ba_t(nrec_relations, nmain);
}

template <typename... BAs>
tau_ba<BAs...> tau_ba<BAs...>::operator|(const tau_ba_t& other) const {
	// TODO (HIGH) replace by ...tau... in the future
	tref nmain = tau::build_wff_or(
			normalizer<tau_ba_t, BAs...>(nso_rr.main.get()),
			normalizer<tau_ba_t, BAs...>(other.nso_rr.main.get()));
	auto nrec_relations =
		merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	return tau_ba_t(nrec_relations, nmain);
}

template <typename... BAs>
tau_ba<BAs...> tau_ba<BAs...>::operator+(const tau_ba_t& other) const {
	// TODO (HIGH) replace by ...tau... in the future
	tref nmain = tau::build_wff_xor_from_def(
			normalizer<tau_ba_t, BAs...>(nso_rr.main.get()),
			normalizer<tau_ba_t, BAs...>(other.nso_rr.main.get()));
	auto nrec_relations =
		merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	return tau_ba_t(nrec_relations, nmain);
}

template <typename... BAs>
tau_ba<BAs...> tau_ba<BAs...>::operator^(const tau_ba_t& other) const {
	return *this + other;
}

template <typename... BAs>
bool tau_ba<BAs...>::is_zero() const {
	auto normalized = normalizer<tau_ba_t, BAs...>(nso_rr);
	return !is_tau_formula_sat(normalized);
}

template <typename... BAs>
bool tau_ba<BAs...>::is_one() const {
	auto normalized = normalizer<tau_ba_t, BAs...>(nso_rr);
	return is_tau_impl( tau::_T, normalized);
}

// template <typename... BAs>
// tau_<tau_ba<BAs...>, BAs...> tau_ba<BAs...>::rename(
// 	const tau_<tau_ba_t, BAs...>& form) const
// {
// 	// TODO (MEDIUM) implement properly
// 	return form;
// }

// template <typename... BAs>
// rewriter::depreciating::rule<rr<tau_nso<BAs...>>> tau_ba<BAs...>::rename(
// 	const rewriter::depreciating::rule<tau_nso_t>& rule) const
// {
// 	// TODO (MEDIUM) implement properly
// 	return rule;
// }

template <typename... BAs>
rewriter::rules tau_ba<BAs...>::merge(
	const rewriter::rules& rs1, const rewriter::rules& rs2) const
{
	// TODO (MEDIUM) implement properly calling renaming
	rewriter::rules nrs;
	nrs.insert(nrs.end(), rs1.begin(), rs1.end());
	nrs.insert(nrs.end(), rs2.begin(), rs2.end());
	return nrs;
}

template <typename... BAs>
bool operator==(const tau_ba<BAs...>& other, const bool& b) {
	return b ? other.is_one() : other.is_zero();
}

template <typename... BAs>
bool operator==(const bool& b, const tau_ba<BAs...>& other) {
	return other == b;
}

template <typename... BAs>
bool operator!=(const tau_ba<BAs...>& other, const bool& b) {
	return !(other == b);
}

template <typename... BAs>
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
	using tau = tree<node<BAs...>>;
	return tau::get(fm.nso_rr.main) == tau::_T;
}

template <typename... BAs>
bool is_syntactic_zero(const tau_ba<BAs...>& fm) {
	using tau = tree<node<BAs...>>;
	return tau::get(fm.nso_rr.main) == tau::_F;
}

template <typename... BAs>
auto splitter (const tau_ba<BAs...>& fm, splitter_type st) {
	auto s = tau_splitter(normalizer(fm.nso_rr), st);
	return tau_ba<BAs...>(s);
}

template <typename... BAs>
auto tau_splitter_one () {
	return tau_bad_splitter<BAs...>();
}

template <typename... BAs>
bool is_closed (const tau_ba<BAs...>& fm) {
	using tau = tree<node<BAs...>>;
	auto simp_fm = apply_rr_to_formula(fm.nso_rr);
	if (!simp_fm) return false;
	if (find_top(simp_fm, is_non_terminal<tau::node, tau::ref>))
		return false;
	auto vars = get_free_vars_from_nso(simp_fm);
	for (const auto& v : vars) {
		const auto& t = tau::get(v);
		if (!(t.only_child_tree().is(tau::io_var)
			|| t.only_child_tree().is(tau::uninterpreted_constant)))
				return false;
	}
	return true;
}

template <typename... BAs>
std::optional<std::variant<tau_ba<BAs...>, BAs...>>
	tau_ba_factory<BAs...>::parse(const std::string& src)
{
	using tau = tree<node<BAs...>>;
	// parse source
	auto source = tau::get(src, tau_parser::parse_options{
					.start = tau::tau_constant_source });
	if (!source) return {};
	auto nso_rr = tau::get_nso_rr(source);
	if (!nso_rr) return {};
	// compute final result
	return { std::variant<tau_ba_t, BAs...>(
			tau_ba_t(nso_rr.value().rec_relations,
				nso_rr.value().main)) };
}

template <typename... BAs>
tref tau_ba_factory<BAs...>::binding(const std::string& source) {
	if (auto p = parse(source); p)
		return ba_constants<tau_ba_t, BAs...>::get(p.value(), "tau");
	return nullptr;
}

template <typename... BAs>
std::variant<tau_ba<BAs...>, BAs...> tau_ba_factory<BAs...>::splitter_one()
	const
{
	return std::variant<tau_ba_t, BAs...>(
		tau_ba<BAs...>(tree<node<BAs...>>::geth(
						tau_splitter_one<BAs...>())));
}

template <typename... BAs>
std::string tau_ba_factory<BAs...>::one(std::string&) const {
	return "T";
}

template <typename... BAs>
std::string tau_ba_factory<BAs...>::zero(std::string&) const {
	return "F";
}

template <typename... BAs>
tau_ba_factory<BAs...>& tau_ba_factory<BAs...>::instance() {
	static tau_ba_factory<BAs...> factory;
	return factory;
}


} // namespace idni::tau_lang

// template <typename... BAs>
// size_t std::hash<idni::tau_lang::tau_ba<BAs...>>::operator()(
// 	const idni::tau_lang::tau_ba<BAs...>& f) const noexcept
// {
// 	return hash<idni::tau_lang::nso_rr>{}(f.nso_rr);
// }

template <typename... BAs>
std::ostream& operator<<(std::ostream& os,
	const idni::tau_lang::tau_ba<BAs...>& rs)
{
	return os << rs.nso_rr;
}
