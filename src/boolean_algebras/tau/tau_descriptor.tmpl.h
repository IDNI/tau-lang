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

/**
 * @brief Descriptor of the tau wrapper algebra over base algebras
 * @p BaseBAs, seen from the pack node `node<PackBAs...>`.
 *
 * Mandatory surface of ba_descriptor_complete plus the optional
 * hash_constant, constant_size and the component-factoring and
 * decision-pin switches.
 */
template <typename... BaseBAs, typename... PackBAs>
struct ba_descriptor<tau_ba<BaseBAs...>, node<PackBAs...>> {
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;
	using ba_t = tau_ba<BaseBAs...>;

	static constexpr const char* type_name = "tau";
	static constexpr int default_type_priority = 0;

	/** @brief The 64-bit content hash: std::hash<tau_ba> keeps 32 bits on wasm32. */
	static std::uint64_t hash_constant(const ba_t& x) { return rr_hash(x.nso_rr); }
	static constexpr bool atomless = true;
	static constexpr bool non_aba_omcat = false;

	/// True iff @p type_tree is the tau type.
	static bool matches_type(tref type_tree) {
		return is_tau_type<node_t>(type_tree);
	}

	/// The tau type tree.
	static tref type_tree() { return tau_type<node_t>(); }

	/// True iff @p ba_type_id is the id of the tau type.
	static bool owns_type(size_t ba_type_id) {
		return is_tau_type<node_t>(ba_type_id);
	}


	// Runs the full validity decision of is_one(); an undecided or failed
	// decision falls to false here (never a witness of validity).
	static bool is_syntactic_one(const ba_t& x) {
		// TODO (HIGH) dropped error: is_one's decision report -- is_syntactic_one returns bool, which cannot carry it.
		return x.is_one().value_or(false);
	}

	// Runs the full satisfiability decision of is_zero(); an undecided or
	// failed decision falls to true here.
	static bool is_syntactic_zero(const ba_t& x) {
		// TODO (HIGH) dropped error: is_zero's decision report -- is_syntactic_zero returns bool, which cannot carry it.
		return x.is_zero().value_or(true);
	}

	/// Whether @p x is valid; an error when the decision fails.
	static result<bool> is_one(const ba_t& x) { return x.is_one(); }

	// the nodes of the embedded spec: its main and its rules
	static size_t constant_size(const ba_t& x) {
		size_t n = x.nso_rr.main
			? (size_t) node_count<node_t>(x.nso_rr.main->get()) : 0;
		for (const auto& [head, body] : x.nso_rr.rec_relations)
			n += (size_t) node_count<node_t>(head->get())
				+ (size_t) node_count<node_t>(body->get());
		return n;
	}

	/// Whether @p x is unsatisfiable; an error when the decision fails.
	static result<bool> is_zero(const ba_t& x) { return x.is_zero(); }

	/// Whether the spec embedded in @p x is closed (is_tau_closed).
	static result<bool> is_closed(const ba_t& x) { return is_tau_closed<BaseBAs...>(x); }

	/// The literal of the tau one, `T`.
	static std::string literal_one(tref) { return "T"; }

	/// The literal of the tau zero, `F`.
	static std::string literal_zero(tref) { return "F"; }

	/// The normal form of @p x (normalize_tau), or the normalizer's error.
	static result<ba_t> normalize(const ba_t& x) { return normalize_tau(x); }

	// tau_splitter's documented precondition is a normalized formula;
	// establish it here, once, for every caller, the same way
	// tau_ba.tmpl.h's own free splitter() does. Routed through
	// normalize_for_splitter (memoized) rather than a raw normalizer()
	// call, since this is the solver's per-candidate hot path
	// (atomless_choose_value's ladder). A failed normalization is an
	// error in the caller's report, never a silent fallback, and the
	// splitter's own report travels the same way.
	static result<ba_t> splitter(const ba_t& x, splitter_type st) {
		result<ba_t> r;
		TAU_TRY(tref n, normalize_for_splitter(x.nso_rr));
		TAU_TRY(tref s, (tau_splitter<ba_t, BaseBAs...>(n, st)));
		return r.with_value(ba_t(s));
	}

	/// The bf constant holding tau_splitter_one, the splitter of one.
	static tref splitter_one(tref) {
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(tau_splitter_one<BaseBAs...>()),
			tau_type<node_t>()));
	}

	/// The main formula of the spec @p x embeds; @p x must have a main.
	static tref unpack(const ba_t& x) { return x.nso_rr.main->get(); }

	/// The constant embedding the formula @p t; never nullopt.
	static std::optional<ba_t> pack(tref t) { return ba_t{t}; }

	/// @p sym unchanged: tau has no symbol simplification.
	static tref simplify_symbol(tref sym) { return sym; }

	/// @p term unchanged: tau has no term simplification.
	static result<tref> simplify_term(tref term) { return result<tref>{term}; }

	/// Parses @p src as a tau spec (parse_tau); the type tree is unused.
	static result<typename node_t::constant_with_type>
	parse(const std::string& src, tref)
	{
		return parse_tau<BaseBAs...>(src);
	}

	/// Set tau's component-factoring switch, owned by tau_ba.h.
	static void set_ba_component_factoring(bool state) {
		ba_component_factoring = state;
	}

	/// Set the cap on pinned decided rows, owned by tau_ba.h.
	static void set_ba_decision_pins(size_t n) {
		idni::tau_lang::ba_decision_pins = n;
	}

	/// Read the cap on pinned decided rows, owned by tau_ba.h.
	static size_t ba_decision_pins() {
		return idni::tau_lang::ba_decision_pins;
	}

	/// Read tau's component-factoring switch, owned by tau_ba.h.
	static bool ba_component_factoring_enabled() {
		return idni::tau_lang::ba_component_factoring_enabled();
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU__TAU_DESCRIPTOR_TMPL_H__
