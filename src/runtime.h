// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __RUNTIME_H__
#define __RUNTIME_H__

#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/sbf_ba.h"
#include "boolean_algebras/tau_ba.h"
#include "boolean_algebras/bv_ba.h"
#include "nso_rr.h"

namespace idni::tau_lang {

// Temporal struct to be used during the refactor
template <typename... BAs>
requires BAsPack<BAs...>
struct base_ba_variants {

	// used in one place
	static 	bool is_syntactic_one(const std::variant<BAs...>& elem) {
		return std::visit(overloaded(
			[](const auto& el) {
				return is_ba_syntactic_one(el);
			}
		), elem);
	}

	// used in one place
	static bool is_syntactic_zero(const std::variant<BAs...>& elem) {
		return std::visit(overloaded(
			[](const auto& el) {
				return is_ba_syntactic_zero(el);
			}
		), elem);
	}

	// used in one place
	static bool is_closed(const std::variant<BAs...>& elem) {
		return std::visit(overloaded(
			[](const auto& el) {
				return is_ba_closed(el);
			}
		), elem);
	}

	static std::variant<BAs...> splitter_ba(const std::variant<BAs...>& elem,
		splitter_type st)
	{
		return std::visit(overloaded(
			[&st](const auto& el) {
				return std::variant<BAs...>(splitter(el, st));
			}
		), elem);
	}

	static std::variant<BAs...> splitter_ba(const std::variant<BAs...>& elem) {
		return splitter_ba(elem, splitter_type::upper);
	}

	static bool is_zero(const std::variant<BAs...>& l) {
		return std::visit(overloaded(
			[](const auto& l) -> bool {
				return l == false;
			}
		), l);
	}

	static bool is_one(const std::variant<BAs...>& l) {
		return std::visit(overloaded(
			[](const auto& l) -> bool {
				return l == true;
			}
		), l);
	}

	static tref simplify_symbol(tref symbol, const std::variant<BAs...>& v) {
		auto f = [&](const auto& ba_type) -> tref {
			return base_ba_symbol_simplification(symbol, ba_type);
		};
		return std::visit(overloaded(f), v);
	}

	static tref simplify_term(tref symbol, const std::variant<BAs...>& v) {
		auto f = [&](const auto& ba_type) -> tref {
			return base_ba_term_simplification(symbol, ba_type);
		};
		return std::visit(overloaded(f), v);
	}
};

template <>
struct nso_factory<bv, sbf_ba>: public base_ba_variants<bv, sbf_ba> {
	using node_t = node<bv, sbf_ba>;
	using tau = tree<node_t>;

	static std::vector<std::string> types();

	static tref default_type();

	static std::string one(const tref type_tree);

	static std::string zero(const tref type_tree);

	static tref splitter_one();

	static tref unpack_tau_ba(const std::variant<bv, sbf_ba>&);

	static std::variant<bv, sbf_ba> pack_tau_ba(tref);

	static std::variant<bv, sbf_ba> to_base_ba_type(tref type_tree);

	static std::variant<bv, sbf_ba> normalize(const std::variant<bv, sbf_ba>& v);

	static tref simplify_symbol(tref symbol);

	static tref simplify_term(tref term);
};

/**
 * @brief NSO factory used in REPL
 */
template<>
struct nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba> : public base_ba_variants<tau_ba<bv, sbf_ba>, bv, sbf_ba> {
	using node_t = node<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
	using tau = tree<node_t>;

	static std::vector<std::string> types();

	static tref default_type();

	static std::string one(tref type_tree = tau_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>());

	static std::string zero(tref type_tree = tau_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>());

	static tref splitter_one(tref type_tree);

	static tref unpack_tau_ba(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& v);

	static std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> pack_tau_ba(tref c);

	static std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> to_base_ba_type(tref type_tree);

	static std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> normalize(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& v);

	static tref simplify_symbol(tref symbol);

	static tref simplify_term(tref term);
};

template <>
inline std::optional<ba_constants<node<bv, sbf_ba>>::constant_with_type> ba_constants<node<bv, sbf_ba>>::get(
		const std::string& constant_source,
		tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (is_bv_type_family<node<bv, sbf_ba>>(type_tree))
		return parse_bv<bv, sbf_ba>(constant_source, type_tree);
	return parse_sbf<bv, sbf_ba>(constant_source);
}

template <>
inline std::optional<typename ba_constants<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>::constant_with_type> ba_constants<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>::get(
		const std::string& constant_source,
		tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (is_sbf_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>(type_tree))
		return parse_sbf<tau_ba<bv, sbf_ba>, bv, sbf_ba>(constant_source);
	if (is_bv_type_family<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>(type_tree))
		return parse_bv<tau_ba<bv, sbf_ba>, bv, sbf_ba>(constant_source, type_tree);
	return parse_tau<bv, sbf_ba>(constant_source);
}

} // namespace idni::tau_lang

#include "runtime.tmpl.h"

#endif // __RUNTIME_H__