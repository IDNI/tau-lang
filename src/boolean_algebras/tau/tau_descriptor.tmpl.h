// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file tau_descriptor.tmpl.h
 * @brief Descriptor through which core reaches the tau wrapper algebra.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU__TAU_DESCRIPTOR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU__TAU_DESCRIPTOR_TMPL_H__

#include "boolean_algebras/ba_descriptor.h"
#include "boolean_algebras/ba_pack_traits.h"

namespace idni::tau_lang {

/** @brief tau_ba is the wrapper algebra embedding a whole Tau spec. */
template <typename... BAs>
struct is_tau_ba<tau_ba<BAs...>> : std::true_type {};

template <typename... BaseBAs, typename... PackBAs>
struct ba_descriptor<tau_ba<BaseBAs...>, node<PackBAs...>> {
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;
	using ba_t = tau_ba<BaseBAs...>;

	static constexpr const char* type_name = "tau";
	static constexpr int default_type_priority = 0;
	static constexpr bool atomless = true;
	static constexpr bool non_aba_omcat = false;

	static bool matches_type(tref type_tree) {
		return is_tau_type<node_t>(type_tree);
	}

	static tref type_tree() { return tau_type<node_t>(); }

	static bool owns_type(tref type_tree) { return matches_type(type_tree); }

	static bool owns_type(size_t ba_type_id) {
		return is_tau_type<node_t>(ba_type_id);
	}

	/** @brief tau takes no subtype, so a type never carries a parameter. */
	static std::optional<unsigned short> type_param(tref) {
		return std::nullopt;
	}

	static size_t type_id_for(unsigned short) { return tau_type_id<node_t>(); }

	static tref type_tree_for(unsigned short) { return type_tree(); }

	static bool is_syntactic_one(const ba_t& x) { return x.is_one(); }

	static bool is_syntactic_zero(const ba_t& x) { return x.is_zero(); }

	static bool is_one(const ba_t& x) { return x.is_one(); }

	static bool is_zero(const ba_t& x) { return x.is_zero(); }

	static bool is_closed(const ba_t& x) { return is_tau_closed<BaseBAs...>(x); }

	static std::string literal_one(tref) { return "T"; }

	static std::string literal_zero(tref) { return "F"; }

	static ba_t normalize(const ba_t& x) { return normalize_tau(x); }

	// tau_splitter's documented precondition is a normalized formula;
	// establish it here, once, for every caller, the same way
	// tau_ba.tmpl.h's own free splitter() does. Routed through
	// normalize_for_splitter (memoized) rather than a raw normalizer()
	// call, since this is the solver's per-candidate hot path
	// (atomless_choose_value's ladder).
	static ba_t splitter(const ba_t& x, splitter_type st) {
		return ba_t(tau_splitter<ba_t, BaseBAs...>(
			normalize_for_splitter(x.nso_rr), st));
	}

	static tref splitter_one(tref) {
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(tau_splitter_one<BaseBAs...>()),
			tau_type<node_t>()));
	}

	static tref unpack(const ba_t& x) { return x.nso_rr.main->get(); }

	static std::optional<ba_t> pack(tref t) { return ba_t{t}; }

	static tref simplify_symbol(tref sym) { return sym; }

	static tref simplify_term(tref term) { return term; }

	static std::optional<typename node_t::constant_with_type>
	parse(const std::string& src, tref)
	{
		return parse_tau<BaseBAs...>(src);
	}

	/// Set tau's component-factoring switch, owned by tau_ba.h.
	static void set_ba_component_factoring(bool state) {
		ba_component_factoring = state;
	}

	/// Read tau's component-factoring switch, owned by tau_ba.h.
	static bool ba_component_factoring_enabled() {
		return idni::tau_lang::ba_component_factoring_enabled();
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU__TAU_DESCRIPTOR_TMPL_H__
