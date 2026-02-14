// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TESTS__TEST_BOOL_HELPERS_H__
#define __IDNI__TAU__TESTS__TEST_BOOL_HELPERS_H__

// helper types and functions for tau with just Bool BA as tree<node<Bool>>
#define bas_pack bv, Bool
#include "test_helpers.h"

namespace idni::tau_lang {

inline tref bool_type() {
	tref type = tau::get(tau::type, "bool");
	return tau::get(tau::typed, type);
}

inline tref base_ba_symbol_simplification(tref symbol, const Bool&) {
	return symbol;
}

inline tref base_ba_term_simplification(tref term, const Bool&) {
	return term;
}

template<>
struct nso_factory<bv, Bool> : public base_ba_variants<bv, Bool> {
	using node_t = node<bv, Bool>;
	using tau = tree<node_t>;

	static bool is_syntactic_one(const std::variant<bv, Bool>& elem) {
		return std::holds_alternative<bv>(elem)
			? is_bv_syntactic_one(std::get<bv>(elem))
			: std::get<Bool>(elem).is_one();
	}

	static bool is_syntactic_zero(const std::variant<bv, Bool>& elem) {
		return std::holds_alternative<bv>(elem)
			? is_bv_syntactic_zero(std::get<bv>(elem))
			: std::get<Bool>(elem).is_zero();
	}

	static bool is_one(const std::variant<bv, Bool>& elem) {
		return std::holds_alternative<bv>(elem)
			? is_bv_syntactic_one(std::get<bv>(elem))
			: std::get<Bool>(elem).is_one();
	}

	static bool is_zero(const std::variant<bv, Bool>& elem) {
		return std::holds_alternative<bv>(elem)
			? is_bv_syntactic_zero(std::get<bv>(elem))
			: std::get<Bool>(elem).is_zero();
	}

	static std::vector<std::string> types() { return { "bool" }; }

	static tref default_type() { return bool_type(); }

	static std::string one(const tref) { return "1"; }

	static std::string zero(const tref) { return "0"; }

	static std::variant<bv, Bool> splitter_one(const std::string) { return Bool(true);}

	// There is no tau_ba
	static tref unpack_tau_ba(const std::variant<bv, Bool>&) { return nullptr; }

	static std::variant<bv, Bool> pack_tau_ba(tref) { return {}; }

	static std::variant<bv, Bool> normalize(const std::variant<bv, Bool>& v) {
		return std::holds_alternative<bv>(v)
			? std::variant<bv, Bool>(normalize_bv(std::get<bv>(v)))
			: std::variant<bv, Bool>(normalize_bool(std::get<Bool>(v)));
	}

	static tref simplify_symbol(tref symbol) {
		auto ba_type = tau::get(symbol).get_ba_type();
		return is_bv_type_family<node_t>(ba_type) ? simplify_bv_symbol<node_t>(symbol) : symbol;
	}

	static tref simplify_term(tref term) {
		auto ba_type = tau::get(term).get_ba_type();
		return is_bv_type_family<node_t>(ba_type) ? simplify_bv_term<node_t>(term) : term;
	}
};

template<>
struct nso_factory<bv, sbf_ba, Bool> : public base_ba_variants<bv, sbf_ba, Bool> {
	using node_t = node<bv, sbf_ba, Bool>;
	using tau = tree<node_t>;

	static bool is_syntactic_one(const std::variant<bv, sbf_ba, Bool>& elem) {
		if (std::holds_alternative<bv>(elem))
			return is_bv_syntactic_one(std::get<bv>(elem));
		else if (std::holds_alternative<Bool>(elem))
			return std::get<Bool>(elem).is_one();
		else return is_sbf_one(std::get<sbf_ba>(elem));
	}

	static bool is_syntactic_zero(const std::variant<bv, sbf_ba, Bool>& elem) {
		if (std::holds_alternative<bv>(elem))
			return is_bv_syntactic_zero(std::get<bv>(elem));
		else if (std::holds_alternative<Bool>(elem))
			return std::get<Bool>(elem).is_zero();
		else return is_sbf_zero(std::get<sbf_ba>(elem));
	}

	static bool is_one(const std::variant<bv, sbf_ba, Bool>& elem) {
		if (std::holds_alternative<bv>(elem))
			return is_bv_syntactic_one(std::get<bv>(elem));
		else if (std::holds_alternative<Bool>(elem))
			return std::get<Bool>(elem).is_one();
		else return is_sbf_one(std::get<sbf_ba>(elem));
	}

	static bool is_zero(const std::variant<bv, sbf_ba, Bool>& elem) {
		if (std::holds_alternative<bv>(elem))
			return is_bv_syntactic_zero(std::get<bv>(elem));
		else if (std::holds_alternative<Bool>(elem))
			return std::get<Bool>(elem).is_zero();
		else return is_sbf_zero(std::get<sbf_ba>(elem));
	}

	static std::vector<std::string> types() { return { "bool" }; }

	static tref default_type() { return bool_type(); }

	static std::string one(const tref) { return "1"; }

	static std::string zero(const tref) { return "0"; }

	static std::variant<bv, sbf_ba, Bool> splitter_one(const std::string) { return Bool(true);}

	// There is no tau_ba
	static tref unpack_tau_ba(const std::variant<bv, sbf_ba, Bool>&) { return nullptr; }

	static std::variant<bv, sbf_ba, Bool> pack_tau_ba(tref) { return {}; }

	static tref simplify_symbol(tref symbol) {
		auto ba_type = tau::get(symbol).get_ba_type();
		return is_bv_type_family<node_t>(ba_type) ? simplify_bv_symbol<node_t>(symbol) : symbol;
	}

	static tref simplify_term(tref term) {
		auto ba_type = tau::get(term).get_ba_type();
		return is_bv_type_family<node_t>(ba_type) ? simplify_bv_term<node_t>(term) : term;
	}
};

template <>
std::optional<typename ba_constants<node<bv, Bool>>::constant_with_type> ba_constants<node<bv, Bool>>::get(
		const std::string& constant_source,
		[[maybe_unused]] tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (constant_source == "1" || constant_source == "true")
		return ba_constants<node<bv, Bool>>::constant_with_type{ Bool(true), bool_type() };
	if (constant_source == "0" || constant_source == "false")
		return ba_constants<node<bv, Bool>>::constant_with_type{ Bool(false), bool_type() };
	return {};
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TESTS__TEST_BOOL_HELPERS_H__
