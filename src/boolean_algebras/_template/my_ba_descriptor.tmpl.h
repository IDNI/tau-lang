// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file my_ba_descriptor.tmpl.h
 * @brief The descriptor core reaches `my_ba` through.
 *
 * Every member below is mandatory: `ba_descriptor_complete` (in
 * ba_descriptor.h) lists them one per line, so omitting one names the missing
 * member in the diagnostic. Optional capabilities are added by simply
 * declaring them — core probes with `requires`, never by BA name.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS___TEMPLATE__MY_BA_DESCRIPTOR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS___TEMPLATE__MY_BA_DESCRIPTOR_TMPL_H__

#include "boolean_algebras/ba_descriptor.h"
#include "ba_types.h"

namespace idni::tau_lang {

template <typename... PackBAs>
struct ba_descriptor<my_ba, node<PackBAs...>> {
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;

	/** @brief The name used in type annotations: `x:my_ba`. */
	static constexpr const char* type_name = "my_ba";

	/** @brief Lower wins when core picks a pack's default type. */
	static constexpr int default_type_priority = 50;

	/** @brief An atomless Boolean algebra? A two-element one is not. */
	static constexpr bool atomless = false;

	/** @brief An ω-categorical theory that is not a Boolean algebra? */
	static constexpr bool non_aba_omcat = false;

	// ── type system ─────────────────────────────────────────────────────
	static bool matches_type(tref type_tree) {
		return tau::get(type_tree)[0].get_string() == type_name;
	}

	static tref type_tree() { return my_ba_type<node_t>(); }

	static bool owns_type(tref type_tree) { return matches_type(type_tree); }

	static bool owns_type(size_t ba_type_id) {
		return matches_type(ba_types<node_t>::type_tree(ba_type_id));
	}

	/** @brief Return the subtype for a parameterized type (`bv[8]`). */
	static std::optional<unsigned short> type_param(tref) {
		return std::nullopt;
	}

	static size_t type_id_for(unsigned short) {
		return ba_types<node_t>::id(type_tree());
	}

	static tref type_tree_for(unsigned short) { return type_tree(); }

	// ── constants ───────────────────────────────────────────────────────
	static bool is_syntactic_one(const my_ba& x) { return x.value; }
	static bool is_syntactic_zero(const my_ba& x) { return !x.value; }
	static bool is_one(const my_ba& x) { return x.value; }
	static bool is_zero(const my_ba& x) { return !x.value; }

	/** @brief `false` for a BA whose elements may contain free variables. */
	static bool is_closed(const my_ba&) { return true; }

	static std::string literal_one(tref) { return "1"; }
	static std::string literal_zero(tref) { return "0"; }

	// ── normalization and splitting ─────────────────────────────────────
	static my_ba normalize(const my_ba& x) { return x; }

	/** @brief Atomless BAs split an element; a finite one cannot. */
	static my_ba splitter(const my_ba& x, splitter_type) { return x; }

	static tref splitter_one(tref) { return nullptr; }

	// ── tree/value bridge, for a BA that embeds Tau trees ───────────────
	static tref unpack(const my_ba&) { return nullptr; }
	static std::optional<my_ba> pack(tref) { return std::nullopt; }

	// ── term rewriting owned by this BA ─────────────────────────────────
	static tref simplify_symbol(tref sym) { return sym; }
	static tref simplify_term(tref term) { return term; }

	// ── parsing ─────────────────────────────────────────────────────────
	static std::optional<typename node_t::constant_with_type>
	parse(const std::string& src, tref)
	{
		return parse_my_ba<PackBAs...>(src);
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS___TEMPLATE__MY_BA_DESCRIPTOR_TMPL_H__
