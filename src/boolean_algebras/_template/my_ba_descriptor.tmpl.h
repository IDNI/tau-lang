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

#include "boolean_algebras/ba_descriptor_named.h"
#include "ba_types.h"

namespace idni::tau_lang {

template <typename... PackBAs>
struct ba_descriptor<my_ba, node<PackBAs...>>
	: ba_descriptor_named<ba_descriptor<my_ba, node<PackBAs...>>,
		my_ba, node<PackBAs...>>
{
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
	// matches_type, type_tree and owns_type come from ba_descriptor_named,
	// derived from type_name above, so the name is written once. So does
	// is_closed, `true`; declare it here for an algebra whose constants
	// may contain free variables.

	// ── constants ───────────────────────────────────────────────────────
	static bool is_syntactic_one(const my_ba& x) { return x.value; }
	static bool is_syntactic_zero(const my_ba& x) { return !x.value; }
	static result<bool> is_one(const my_ba& x) { return result<bool>{x.value}; }
	static result<bool> is_zero(const my_ba& x) { return result<bool>{!x.value}; }

	static std::string literal_one(tref) { return "1"; }
	static std::string literal_zero(tref) { return "0"; }

	// ── normalization and splitting ─────────────────────────────────────
	static result<my_ba> normalize(const my_ba& x) {
		return result<my_ba>{x};
	}

	// ── term rewriting owned by this BA ─────────────────────────────────
	static tref simplify_symbol(tref sym) { return sym; }
	static result<tref> simplify_term(tref term) { return result<tref>{term}; }

	// ── parsing ─────────────────────────────────────────────────────────
	static result<typename node_t::constant_with_type>
	parse(const std::string& src, tref)
	{
		return parse_my_ba<PackBAs...>(src);
	}
};

} // namespace idni::tau_lang

// Optional rewrite hooks; drop this include together with the file if the
// algebra defines none.
#include "boolean_algebras/_template/my_ba_ba_hooks_ext.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS___TEMPLATE__MY_BA_DESCRIPTOR_TMPL_H__
