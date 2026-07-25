// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The concrete BA packs' specializations are DECLARED in these headers and
// defined once each in the matching .cpp (see those files for why).  Included
// outside the namespace below because they open their own; an explicit
// specialization must be declared in the namespace its template belongs to.
#include "base_ba_dispatcher_bv_sbf.h"
#include "base_ba_dispatcher_full_pack.h"

namespace idni::tau_lang {

// Parse a plain Bool constant ("0"/"false"/"F" or "1"/"true"/"T").
template <typename... BAs>
requires BAsPack<BAs...>
inline std::optional<typename node<BAs...>::constant_with_type> parse_bool(
		const std::string& src) {
	if (src == "0" || src == "false" || src == "F")
		return typename node<BAs...>::constant_with_type{
			std::variant<BAs...>{ Bool(false) }, bool_type<node<BAs...>>() };
	if (src == "1" || src == "true" || src == "T")
		return typename node<BAs...>::constant_with_type{
			std::variant<BAs...>{ Bool(true) }, bool_type<node<BAs...>>() };
	return {};
}

// ba_constants specialization for <bv, Bool>, used by tests that need
// bv/Bool without the sbf_ba BDD layer (see tests/test_Bool_helpers.h).
template <>
inline std::optional<ba_constants<node<bv, Bool>>::constant_with_type> ba_constants<node<bv, Bool>>::get(
		const std::string& constant_source,	tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (is_bv_type_family<node<bv, Bool>>(type_tree))
		return parse_bv<bv, Bool>(constant_source, type_tree);
	return parse_bool<bv, Bool>(constant_source);
}

// base_ba_dispatcher specialization for <bv, Bool>, used by tests that need
// bv/Bool without the sbf_ba BDD layer (see tests/test_Bool_helpers.h).
template<>
inline bool base_ba_dispatcher<bv, Bool>::is_syntactic_one(const std::variant<bv, Bool>& elem) {
	return std::holds_alternative<bv>(elem)
		? is_bv_syntactic_one(std::get<bv>(elem))
		: std::get<Bool>(elem).is_one();
}

template<>
inline bool base_ba_dispatcher<bv, Bool>::is_syntactic_zero(const std::variant<bv, Bool>& elem) {
	return std::holds_alternative<bv>(elem)
		? is_bv_syntactic_zero(std::get<bv>(elem))
		: std::get<Bool>(elem).is_zero();
}

template<>
inline bool base_ba_dispatcher<bv, Bool>::is_one(const std::variant<bv, Bool>& elem) {
	return std::holds_alternative<bv>(elem)
		? is_bv_syntactic_one(std::get<bv>(elem))
		: std::get<Bool>(elem).is_one();
}

template<>
inline bool base_ba_dispatcher<bv, Bool>::is_zero(const std::variant<bv, Bool>& elem) {
	return std::holds_alternative<bv>(elem)
		? is_bv_syntactic_zero(std::get<bv>(elem))
		: std::get<Bool>(elem).is_zero();
}

template<>
inline bool base_ba_dispatcher<bv, Bool>::is_closed(const std::variant<bv, Bool>&) {
	// We return true as bv and Bool are closed (for our purposes)
	return true;
}

template<>
inline std::vector<std::string> base_ba_dispatcher<bv, Bool>::types() {
	return { "bool", "bv" };
}

template<>
inline tref base_ba_dispatcher<bv, Bool>::default_type() {
	return bool_type<node<bv, Bool>>();
}

template<>
inline std::string base_ba_dispatcher<bv, Bool>::one(const tref type_tree) {
	using node_t = node<bv, Bool>;
	if (is_bv_type_family<node_t>(type_tree))
		return make_bitvector_top_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	else return "1";
}

template<>
inline std::string base_ba_dispatcher<bv, Bool>::zero(const tref type_tree) {
	using node_t = node<bv, Bool>;
	if (is_bv_type_family<node_t>(type_tree))
		return make_bitvector_bottom_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	return "0";
}

template<>
inline tref base_ba_dispatcher<bv, Bool>::splitter_one(tref) {
	return nullptr; // There is no splitter one for plain Bool or bv
}

template<>
inline std::variant<bv, Bool> base_ba_dispatcher<bv, Bool>::splitter(const std::variant<bv, Bool>& elem, splitter_type) {
	DBG(assert(!std::holds_alternative<bv>(elem));)
	return elem; // Plain Bool has no BDD-based splitter
}

template<>
inline tref base_ba_dispatcher<bv, Bool>::unpack_tau_ba(const std::variant<bv, Bool>&) {
	return nullptr; // There is no tau_ba present
}

template<>
inline std::variant<bv, Bool> base_ba_dispatcher<bv, Bool>::pack_tau_ba(tref) {
	// There is no tau_ba
	return {};
}

template<>
inline std::variant<bv, Bool> base_ba_dispatcher<bv, Bool>::normalize(const std::variant<bv, Bool>& v) {
	return std::holds_alternative<bv>(v)
		? std::variant<bv, Bool>(normalize_bv(std::get<bv>(v)))
		: v;
}

template<>
inline tref base_ba_dispatcher<bv, Bool>::simplify_symbol(tref symbol) {
	using node_t = node<bv, Bool>;
	auto ba_type = tau::get(symbol).get_ba_type();
	return is_bv_type_family<node_t>(ba_type) ? simplify_bv_symbol<node_t>(symbol) : symbol;
}

template<>
inline tref base_ba_dispatcher<bv, Bool>::simplify_term(tref term) {
	using node_t = node<bv, Bool>;
	auto ba_type = tau::get(term).get_ba_type();
	return is_bv_type_family<node_t>(ba_type) ? simplify_bv_term<node_t>(term) : term;
}


/*template<>
struct base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba> {};

template<>
struct base_ba_dispatcher<bv, sbf_ba> {};*/

} // namespace idni::tau_lang