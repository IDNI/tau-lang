// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Explicit specializations of the base BA dispatcher and constant parser for
// the <bv, Bool> pack.
//
// These used to be defined `inline` in base_ba_dispatcher.tmpl.h, which every
// test translation unit includes.  Their bodies call parse_bv/parse_sbf/
// parse_tau over the pack, so merely *seeing* them instantiated the entire
// <bv, sbf_ba> pipeline in every TU -- measured with clang -ftime-trace at
// 40.6%% of all template-instantiation time in a TU that does not use the pack
// at all, and 1158 KB / 5972 emitted weak symbols per object.
//
// Only four targets actually use this pack (tests/test_sbf_ba_helpers.h and
// tests/integration/solver/test_integration-solver-sbf.cpp), so the
// definitions now live in one translation unit compiled into libTAU and every
// other TU sees declarations only.
//
// plugins_compile_time.md R3; plugins_infra.md R6 wants this glue deleted
// outright once the pack becomes a configure-time choice.

#include "tau.h"

namespace idni::tau_lang {

template <>
std::optional<ba_constants<node<bv, Bool>>::constant_with_type> ba_constants<node<bv, Bool>>::get(
		const std::string& constant_source,	tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (is_bv_type_family<node<bv, Bool>>(type_tree))
		return parse_bv<bv, Bool>(constant_source, type_tree);
	return parse_bool<bv, Bool>(constant_source);
}

template<>
bool base_ba_dispatcher<bv, Bool>::is_syntactic_one(const std::variant<bv, Bool>& elem) {
	return std::holds_alternative<bv>(elem)
		? is_bv_syntactic_one(std::get<bv>(elem))
		: std::get<Bool>(elem).is_one();
}

template<>
bool base_ba_dispatcher<bv, Bool>::is_syntactic_zero(const std::variant<bv, Bool>& elem) {
	return std::holds_alternative<bv>(elem)
		? is_bv_syntactic_zero(std::get<bv>(elem))
		: std::get<Bool>(elem).is_zero();
}

template<>
bool base_ba_dispatcher<bv, Bool>::is_one(const std::variant<bv, Bool>& elem) {
	return std::holds_alternative<bv>(elem)
		? is_bv_syntactic_one(std::get<bv>(elem))
		: std::get<Bool>(elem).is_one();
}

template<>
bool base_ba_dispatcher<bv, Bool>::is_zero(const std::variant<bv, Bool>& elem) {
	return std::holds_alternative<bv>(elem)
		? is_bv_syntactic_zero(std::get<bv>(elem))
		: std::get<Bool>(elem).is_zero();
}

template<>
bool base_ba_dispatcher<bv, Bool>::is_closed(const std::variant<bv, Bool>&) {
	// We return true as bv and Bool are closed (for our purposes)
	return true;
}

template<>
std::vector<std::string> base_ba_dispatcher<bv, Bool>::types() {
	return { "bool", "bv" };
}

template<>
tref base_ba_dispatcher<bv, Bool>::default_type() {
	return bool_type<node<bv, Bool>>();
}

template<>
std::string base_ba_dispatcher<bv, Bool>::one(const tref type_tree) {
	using node_t = node<bv, Bool>;
	if (is_bv_type_family<node_t>(type_tree))
		return make_bitvector_top_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	else return "1";
}

template<>
std::string base_ba_dispatcher<bv, Bool>::zero(const tref type_tree) {
	using node_t = node<bv, Bool>;
	if (is_bv_type_family<node_t>(type_tree))
		return make_bitvector_bottom_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	return "0";
}

template<>
tref base_ba_dispatcher<bv, Bool>::splitter_one(tref) {
	return nullptr; // There is no splitter one for plain Bool or bv
}

template<>
std::variant<bv, Bool> base_ba_dispatcher<bv, Bool>::splitter(const std::variant<bv, Bool>& elem, splitter_type) {
	DBG(assert(!std::holds_alternative<bv>(elem));)
	return elem; // Plain Bool has no BDD-based splitter
}

template<>
tref base_ba_dispatcher<bv, Bool>::unpack_tau_ba(const std::variant<bv, Bool>&) {
	return nullptr; // There is no tau_ba present
}

template<>
std::variant<bv, Bool> base_ba_dispatcher<bv, Bool>::pack_tau_ba(tref) {
	// There is no tau_ba
	return {};
}

template<>
std::variant<bv, Bool> base_ba_dispatcher<bv, Bool>::normalize(const std::variant<bv, Bool>& v) {
	return std::holds_alternative<bv>(v)
		? std::variant<bv, Bool>(normalize_bv(std::get<bv>(v)))
		: v;
}

template<>
tref base_ba_dispatcher<bv, Bool>::simplify_symbol(tref symbol) {
	using node_t = node<bv, Bool>;
	auto ba_type = tau::get(symbol).get_ba_type();
	return is_bv_type_family<node_t>(ba_type) ? simplify_bv_symbol<node_t>(symbol) : symbol;
}

template<>
tref base_ba_dispatcher<bv, Bool>::simplify_term(tref term) {
	using node_t = node<bv, Bool>;
	auto ba_type = tau::get(term).get_ba_type();
	return is_bv_type_family<node_t>(ba_type) ? simplify_bv_term<node_t>(term) : term;
}

} // namespace idni::tau_lang
