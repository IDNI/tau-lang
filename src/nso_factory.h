// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <optional>
#include <variant>
#include <string>
#include <vector>

#include "tau_tree.h"
#include "splitter_types.h"

#ifndef __IDNI__TAU__NSO_FACTORY_H__
#define __IDNI__TAU__NSO_FACTORY_H__

namespace idni::tau_lang {

/**
 * Factory for creating and manipulating tau objects.
 * - interface" for creating custom BA types by adding nso_factory<YourBA>
 *   template specialization.
 */
template <typename... BAs>
requires BAsPack<BAs...>
struct nso_factory {

	using node = node<BAs...>;

	/**
	 * Returns a vector of available base type names.
	 * @return A vector of base type names.
	 */
	static std::vector<std::string> types();

	/**
	 * Returns the default type.
	 * @return The default type name.
	 */
	static tref default_type();

	/**
	 * Returns a string representation of one for the given type.
	 * @param type_tree The type tree.
	 * @return The string representation of one.
	 */
	static std::string one(tref type_tree);

	/**
	 * Returns a string representation of zero for the given type.
	 * @param type_tree The type tree.
	 * @return The string representation of zero.
	 */
	static std::string zero(tref type_tree);

	/**
	 * Returns a tau object representing a splitter with one.
	 * @param type_tree The type tree.
	 * @return The splitter constant.
	 */
	static tref splitter_one(tref type_tree);

	/**
	 * Returns the tau formula stored in the variant, if present
	 * @param constant Variant for formula extraction
	 * @return Extracted tau_ba spec if present
	 */
	static tref unpack_tau_ba(const std::variant<BAs...>& constant);

	/**
	 * Returns a std::variant holding the tau_ba of the given formula
	 * @param c Formula to wrap inside tau_ba
	 * @return std::variant holding a tau_ba
	 */
	static std::variant<BAs...> pack_tau_ba (tref c);

	/**
	 * Returns a std::variant containing a default base Boolean algebra
	 * element corresponding to the type given by type_tree
	 * @param type_tree Tree representation of type
	 * @return std::variant holding a default corresponding base Boolean
	 * algebra element
	 */
	static std::variant<BAs...> to_base_ba_type(tref type_tree);

	static typename node::constant normalize_ba(const typename node::constant& elem){
		return std::visit(overloaded(
			[](const auto& el) {
				return typename node::constant(normalize(el));
			}
		), elem);
	}

	// used in one place
	static 	bool is_syntactic_one(const std::variant<BAs...>& elem) {
		return std::visit(overloaded(
			[](const auto& el) {
				return is_syntactic_one(el);
			}
		), elem);
	}

	// used in one place
	static bool is_syntactic_zero(const std::variant<BAs...>& elem) {
		return std::visit(overloaded(
			[](const auto& el) {
				return is_syntactic_zero(el);
			}
		), elem);
	}

	// used in one place
	static bool is_closed(const std::variant<BAs...>& elem) {
		return std::visit(overloaded(
			[](const auto& el) {
				return is_closed(el);
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

	static tref base_ba_symbol_simplification(tref symbol, const std::variant<BAs...>& v) {
		auto f = [&](const auto& ba_type) -> tref {
			return base_ba_symbol_simplification<BAs...>(symbol, ba_type);
		};
		return std::visit(overloaded(f), v);
	}

	static tref base_ba_term_simplification(tref symbol, const std::variant<BAs...>& v) {
		auto f = [&](const auto& ba_type) -> tref {
			return base_ba_term_simplification<BAs...>(symbol, ba_type);
		};
		return std::visit(overloaded(f), v);
	}

};


} // namespace idni::tau_lang

#endif // __IDNI__TAU__NSO_FACTORY_H__
