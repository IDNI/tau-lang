// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file nlang_descriptor.tmpl.h
 * @brief Descriptor through which core reaches the nlang Boolean algebra.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_DESCRIPTOR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_DESCRIPTOR_TMPL_H__

#include "boolean_algebras/nlang/parser/nlang_parser.generated.h"
#include "boolean_algebras/ba_descriptor.h"
#include "ba_types.h"

namespace idni::tau_lang {

template <typename... PackBAs>
struct ba_descriptor<nlang_ba, node<PackBAs...>> {
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;

	static constexpr const char* type_name = "nlang";
	static constexpr int default_type_priority = 50;
	static constexpr bool atomless = true;
	static constexpr bool non_aba_omcat = false;
	// answers come from an LLM oracle, so comparing two constants can leave
	// the process and need not be reproducible
	static constexpr bool uses_oracle = true;

	/**
	 * @brief No past output can leave the current constraint unsatisfiable.
	 *
	 * There is no operation here by which history exhausts the admissible
	 * outputs: `o = p` is met by emitting `p` whatever came before, and being
	 * atomless, even a chain such as `o[t] < o[t-1]` can always be continued.
	 * So satisfiable at one step means satisfiable at every step, and core may
	 * answer a formula no input constrains by feasibility alone instead of the
	 * safety fixpoint. A bounded or finite algebra must not declare this --
	 * `bv[8]` runs out of room at `o[t] = o[t-1] + 1`.
	 */
	static constexpr bool output_always_satisfiable_by_system = true;

	static bool matches_type(tref type_tree) {
		return ba_types_detail::type_tree_name_is<nlang_ba, node_t>(
			type_tree, type_name);
	}

	static tref type_tree() {
		return ba_types_detail::make_syntactic_type_tree<node_t>(
			type_name);
	}

	static bool owns_type(tref type_tree) { return matches_type(type_tree); }

	static bool owns_type(size_t ba_type_id) {
		return ba_types_detail::type_tree_name_is<nlang_ba, node_t>(
			ba_type_id, type_name);
	}

	/** @brief nlang takes no subtype, so a type never carries a parameter. */
	static std::optional<unsigned short> type_param(tref) {
		return std::nullopt;
	}

	static size_t type_id_for(unsigned short) {
		static const size_t id = ba_types<node_t>::id(type_tree());
		return id;
	}

	static tref type_tree_for(unsigned short) { return type_tree(); }

	static bool is_syntactic_one(const nlang_ba& x) { return is_nlang_one(x); }

	static bool is_syntactic_zero(const nlang_ba& x) { return is_nlang_zero(x); }

	static bool is_one(const nlang_ba& x) { return is_nlang_one(x); }

	static bool is_zero(const nlang_ba& x) { return is_nlang_zero(x); }

	static bool is_closed(const nlang_ba&) { return true; }

	static std::string literal_one(tref) { return "everything"; }

	static std::string literal_zero(tref) { return "nothing"; }

	static nlang_ba normalize(const nlang_ba& x) { return normalize_nlang(x); }

	static nlang_ba splitter(const nlang_ba& x, splitter_type st) {
		return nlang_splitter(x, st);
	}

	static tref splitter_one(tref) {
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(nlang_splitter_one()),
			type_tree()));
	}

	/** @brief nlang holds no Tau spec, so there is nothing to unpack. */
	static tref unpack(const nlang_ba&) { return nullptr; }

	static std::optional<nlang_ba> pack(tref) { return std::nullopt; }

	static tref simplify_symbol(tref sym) {
		return simplify_nlang_symbol(sym);
	}

	static tref simplify_term(tref term) { return simplify_nlang_term(term); }

	static std::optional<typename node_t::constant_with_type>
	parse(const std::string& src, tref)
	{
		return parse_nlang<PackBAs...>(src);
	}
	/**
	 * @brief `true` when @p src is a truncated nlang literal, not a bad one.
	 *
	 * Distinct from `parse` failing, which cannot tell the two apart; the REPL
	 * keeps reading on truncation and stops on a genuine syntax error.
	 */
	static bool literal_incomplete(const std::string& src) {
		auto result = nlang_parser::instance()
			.parse(src.c_str(), src.size());
		return !result.found && result.parse_error
			.to_str(nlang_parser::error::info_lvl::INFO_BASIC)
			.find("Unexpected end of file") != std::string::npos;
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_DESCRIPTOR_TMPL_H__
