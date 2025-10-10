// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "boolean_algebras/tau_ba.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "tau_ba"

namespace idni::tau_lang {

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba(const rewriter::rules& rec_relations, htref main)
		: nso_rr({ rec_relations, main }) {}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba(const rewriter::rules& rec_relations, tref main)
		: nso_rr({ rec_relations, tau::geth(main) }) {}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba(htref main) : nso_rr({ main }) {}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba(tref main) : nso_rr({ tau::geth(main) }) {}

template <typename... BAs>
requires BAsPack<BAs...>
auto tau_ba<BAs...>::operator<=>(const tau_ba<BAs...>&) const = default;

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator~() const {
	// TODO (HIGH) replace by ...tau... in the future
	auto nmain = tau::geth(tau::build_wff_neg(
			normalizer<node>(nso_rr.main->get())));
	auto nrec_relations = nso_rr.rec_relations;
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator&(const tau_ba<BAs...>& other) const {
	// TODO (HIGH) replace by ...tau... in the future
	auto nmain = tau::geth(tau::build_wff_and(
			nso_rr.main->get(), other.nso_rr.main->get()));
	auto nrec_relations =
		rewriter::merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator|(const tau_ba<BAs...>& other) const {
	// TODO (HIGH) replace by ...tau... in the future
	auto nmain = tau::geth(tau::build_wff_or(
			normalizer<node>(nso_rr.main->get()),
			normalizer<node>(other.nso_rr.main->get())));
	auto nrec_relations = rewriter::merge(nso_rr.rec_relations,
					      other.nso_rr.rec_relations);
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator+(const tau_ba<BAs...>& other) const {
	// TODO (HIGH) replace by ...tau... in the future
	auto nmain = tau::geth(tau::build_wff_xor_from_def(
			normalizer<node>(nso_rr.main->get()),
			normalizer<node>(other.nso_rr.main->get())));
	rewriter::rules nrec_relations = rewriter::merge(nso_rr.rec_relations,
						other.nso_rr.rec_relations);
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator^(const tau_ba<BAs...>& other) const {
	return *this + other;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool tau_ba<BAs...>::is_zero() const {
	tref normalized = normalizer<node>(nso_rr);
	return !is_tau_formula_sat<node>(normalized);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool tau_ba<BAs...>::is_one() const {
	tref normalized = normalizer<node>(nso_rr);
	return is_tau_impl<node>(tau::_T(), normalized);
}

// template <typename... BAs>
// requires BAsPack<BAs...>
// tau_<tau_ba<BAs...>, BAs...> tau_ba<BAs...>::rename(
// 	const tau_<tau_ba<BAs...>, BAs...>& form) const
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
bool operator==(const tau_ba<BAs...>& lhs, const tau_ba<BAs...>& rhs) {
	return lhs.nso_rr.main == rhs.nso_rr.main &&
		lhs.nso_rr.rec_relations == rhs.nso_rr.rec_relations;
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
tau_ba<BAs...> normalize(const tau_ba<BAs...>& fm) {
	tref result =
		apply_rr_to_formula<node<tau_ba<BAs...>, BAs...>>(fm.nso_rr);
	result = simp_tau_unsat_valid<node<tau_ba<BAs...>, BAs...>>(result);
	return tau_ba<BAs...>(tree<node<tau_ba<BAs...>, BAs...>>::geth(result));
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_syntactic_one(const tau_ba<BAs...>& fm) {
	return tree<node<tau_ba<BAs...>, BAs...>>::get(fm.nso_rr.main).equals_T();
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_syntactic_zero(const tau_ba<BAs...>& fm) {
	return tree<node<tau_ba<BAs...>, BAs...>>::get(fm.nso_rr.main).equals_F();
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> splitter(const tau_ba<BAs...>& fm, splitter_type st) {
	tref s = tau_splitter<tau_ba<BAs...>, BAs...>(
		normalizer<node<tau_ba<BAs...>, BAs...>>(fm.nso_rr), st);
	return tau_ba<BAs...>(tree<node<tau_ba<BAs...>, BAs...>>::geth(s));
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_splitter_one() {
	return tau_ba<BAs...>(tau_bad_splitter<tau_ba<BAs...>, BAs...>());
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_closed(const tau_ba<BAs...>& fm) {
	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	using tau = tree<node>;
	tref simp_fm = apply_rr_to_formula<node>(fm.nso_rr);
	if (!simp_fm) return false;
	simp_fm = apply_defs_to_spec<node>(simp_fm);
	if (!simp_fm) return false;
	if (tau::get(simp_fm).find_top(is<node, tau::ref>))
		return false;
	const trefs& vars = get_free_vars<node>(simp_fm);
	for (tref v : vars) {
		const tau& t = tau::get(v);
		if (!(t.child_is(tau::io_var)
			|| t.child_is(tau::uconst_name)))
				return false;
	}
	return true;
}


template <typename... BAs>
requires BAsPack<BAs...>
std::optional<constant_with_type<tau_ba<BAs...>, BAs...>>
	parse_tau(const std::string& src)
{
	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	using tau = tree<node>;

	// parse source
	typename tau::get_options opts{ .parse = {
					.start = tau::tau_constant_source } };
	tref source = tau::get(src, opts);
	if (!source) return {};
	auto nso_rr = get_nso_rr<node>(source);
	if (!nso_rr) return {};
	// compute final result
	return constant_with_type<tau_ba<BAs...>, BAs...>{
		std::variant<tau_ba<BAs...>, BAs...>(
			tau_ba<BAs...>(nso_rr.value().rec_relations,
				       nso_rr.value().main)),
		"tau" };
}

template <typename... BAs>
requires BAsPack<BAs...>
std::string tau_ba_factory<BAs...>::one(std::string&) {
	return "T";
}

template <typename... BAs>
requires BAsPack<BAs...>
std::string tau_ba_factory<BAs...>::zero(std::string&) {
	return "F";
}

template <typename... BAs>
requires BAsPack<BAs...>
std::variant<tau_ba<BAs...>, BAs...>
	tau_ba_factory<BAs...>::splitter_one()
{
	return std::variant<tau_ba<BAs...>, BAs...>(
			tau_splitter_one<BAs...>());
}

template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& operator<<(std::ostream& os, const tau_ba<BAs...>& rs) {
	return print<node<tau_ba<BAs...>, BAs...>>(os, rs.nso_rr);
}

} // namespace idni::tau_lang
