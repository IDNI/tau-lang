// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_ba.h"

namespace idni::tau_lang {

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba(rewriter::rules& rec_relations, htree::sp main)
		: nso_rr({ rec_relations, main }) {}
template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba(htree::sp main) : nso_rr({main}) {}

template <typename... BAs>
requires BAsPack<BAs...>
auto tau_ba<BAs...>::operator<=>(const tau_ba<BAs...>&) const = default;

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator~() const {
	// TODO (HIGH) replace by ...tau... in the future
	auto nmain = tau::geth(tau::build_wff_neg(
			normalizer<tau_ba_node>(nso_rr.main->get())));
	auto nrec_relations = nso_rr.rec_relations;
	return tau_ba_t(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator&(const tau_ba_t& other) const {
	// TODO (HIGH) replace by ...tau... in the future
	auto nmain = tau::geth(tau::build_wff_and(
			nso_rr.main->get(), other.nso_rr.main->get()));
	auto nrec_relations =
		rewriter::merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	return tau_ba_t(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator|(const tau_ba_t& other) const {
	// TODO (HIGH) replace by ...tau... in the future
	auto nmain = tau::geth(tau::build_wff_or(
			normalizer<tau_ba_node>(nso_rr.main->get()),
			normalizer<tau_ba_node>(other.nso_rr.main->get())));
	auto nrec_relations =
		rewriter::merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	return tau_ba_t(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator+(const tau_ba_t& other) const {
	// TODO (HIGH) replace by ...tau... in the future
	auto nmain = tau::geth(tau::build_wff_xor_from_def(
			normalizer<tau_ba_node>(nso_rr.main->get()),
			normalizer<tau_ba_node>(other.nso_rr.main->get())));
	rewriter::rules nrec_relations =
		rewriter::merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	return tau_ba_t(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator^(const tau_ba_t& other) const {
	return *this + other;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool tau_ba<BAs...>::is_zero() const {
	auto normalized = normalizer<tau_ba_node>(nso_rr);
	return !is_tau_formula_sat<node>(normalized);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool tau_ba<BAs...>::is_one() const {
	auto normalized = normalizer<tau_ba_node>(nso_rr);
	return is_tau_impl<node>(tau::_T(), normalized);
}

// template <typename... BAs>
// requires BAsPack<BAs...>
// tau_<tau_ba<BAs...>, BAs...> tau_ba<BAs...>::rename(
// 	const tau_<tau_ba_t, BAs...>& form) const
// {
// 	// TODO (MEDIUM) implement properly
// 	return form;
// }

// template <typename... BAs>
// requires BAsPack<BAs...>
// rewriter::rule<rr<tau_nso<BAs...>>> tau_ba<BAs...>::rename(
// 	const rewriter::rule<tau_nso_t>& rule) const
// {
// 	// TODO (MEDIUM) implement properly
// 	return rule;
// }

template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const tau_ba<BAs...>& other, const bool& b) {
	return b ? other.is_one() : other.is_zero();
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const bool& b, const tau_ba<BAs...>& other) {
	return other == b;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const tau_ba<BAs...>& other, const bool& b) {
	return !(other == b);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const bool& b, const tau_ba<BAs...>& other) {
	return !(other == b);
}

template <typename... BAs>
requires BAsPack<BAs...>
auto normalize(const tau_ba<BAs...>& fm) {
	tref res = apply_rr_to_formula<node<tau_ba<BAs...>, BAs...>>(fm.nso_rr);
	res = simp_tau_unsat_valid<node<BAs...>>(res);
	return tau_ba<BAs...>(tree<node<BAs...>>::geth(res));
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_syntactic_one(const tau_ba<BAs...>& fm) {
	return tree<node<BAs...>>::get(fm.nso_rr.main) == tree<node<BAs...>>::_T();
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_syntactic_zero(const tau_ba<BAs...>& fm) {
	return tree<node<BAs...>>::get(fm.nso_rr.main) == tree<node<BAs...>>::_F();
}

template <typename... BAs>
requires BAsPack<BAs...>
auto splitter(const tau_ba<BAs...>& fm, splitter_type st) {
	auto s = tau_splitter<BAs...>(normalizer<node<BAs...>>(fm.nso_rr), st);
	return tau_ba<BAs...>(tree<node<BAs...>>::geth(s));
}

template <typename... BAs>
requires BAsPack<BAs...>
auto tau_splitter_one() {
	return tau_bad_splitter<BAs...>();
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_closed(const tau_ba<BAs...>& fm) {
	using tau = tree<node<BAs...>>;
	auto simp_fm = apply_rr_to_formula<node<BAs...>>(fm.nso_rr);
	if (!simp_fm) return false;
	if (tau::get(simp_fm).find_top(is<node<BAs...>, tau::ref>))
		return false;
	auto vars = get_free_vars_from_nso<node<BAs...>>(simp_fm);
	for (const auto& v : vars) {
		const auto& t = tau::get(v);
		if (!(t.only_child_tree().is(tau::io_var)
			|| t.only_child_tree().is(tau::uconst_name)))
				return false;
	}
	return true;
}

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<std::variant<tau_ba<BAs...>, BAs...>>
	tau_ba_factory<BAs...>::parse(const std::string& src)
{
	using tau = tree<node<BAs...>>;
	using parse_options = typename tau::parse_options;
	// parse source
	parse_options opts; opts.start = tau::tau_constant_source;
	auto source = tau::get(src, opts);
	if (!source) return {};
	auto nso_rr = get_nso_rr<node<BAs...>>(source);
	if (!nso_rr) return {};
	// compute final result
	return { std::variant<tau_ba_t, BAs...>(
			tau_ba_t(nso_rr.value().rec_relations,
				nso_rr.value().main)) };
}

template <typename... BAs>
requires BAsPack<BAs...>
tref tau_ba_factory<BAs...>::binding(const std::string& source) {
	if (auto p = parse(source); p)
		return ba_constants_binder<tau_ba_t, BAs...>::instance()
							.bind(p.value(), "tau");
	return nullptr;
}

template <typename... BAs>
requires BAsPack<BAs...>
std::variant<tau_ba<BAs...>, BAs...> tau_ba_factory<BAs...>::splitter_one()
	const
{
	return std::variant<tau_ba_t, BAs...>(
		tau_ba<BAs...>(tree<node<BAs...>>::geth(
						tau_splitter_one<BAs...>())));
}

template <typename... BAs>
requires BAsPack<BAs...>
std::string tau_ba_factory<BAs...>::one(std::string&) const {
	return "T";
}

template <typename... BAs>
requires BAsPack<BAs...>
std::string tau_ba_factory<BAs...>::zero(std::string&) const {
	return "F";
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba_factory<BAs...>& tau_ba_factory<BAs...>::instance() {
	static tau_ba_factory<BAs...> factory;
	return factory;
}

template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& operator<<(std::ostream& os, const tau_ba<BAs...>& rs) {
	return print<node<BAs...>>(os, rs.nso_rr);
}

} // namespace idni::tau_lang
