// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file rr.h
 * @brief Recurrence-relation wrapper type `rr<node>`.
 *
 * `rr` bundles a set of named rewriting rules (`rec_relations`) with a main
 * formula (`main`) and provides comparison operators required for use as a
 * map key or in ordered containers.
 */

#ifndef __IDNI__TAU__RR_H__
#define __IDNI__TAU__RR_H__

#include "utility/tree.h"

namespace idni::tau_lang {

/**
 * @brief Recurrence-relation bundle: a set of named rules plus a main formula.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct rr {
	/**
	 * @brief Construct from a set of recurrence rules and a main formula.
	 * @param rec_relations Named rewriting rules.
	 * @param main Handle to the main formula.
	 */
	rr(const rewriter::rules& rec_relations, const htref& main);

	/**
	 * @brief Construct a rule-free wrapper around @p main.
	 * @param main Handle to the main formula.
	 */
	rr(const htref& main);

	/** @brief Default-construct an empty recurrence relation. */
	rr();

	/**
	 * @brief Content three-way comparison: by rule count first, then rule by
	 * rule (head, then body), then main; null handles order first.
	 */
	std::weak_ordering operator<=>(const rr& that) const;
	/** @brief Less-than derived from `<=>`. */
	constexpr bool     operator<  (const rr& that) const;
	/** @brief Less-than-or-equal derived from `<=>`. */
	constexpr bool     operator<= (const rr& that) const;
	/** @brief Greater-than derived from `<=>`. */
	constexpr bool     operator>  (const rr& that) const;
	/** @brief Greater-than-or-equal derived from `<=>`. */
	constexpr bool     operator>= (const rr& that) const;
	/** @brief Equality derived from `<=>`. */
	constexpr auto     operator== (const rr& that) const;
	/** @brief Inequality derived from `<=>`. */
	constexpr auto     operator!= (const rr& that) const;

	rewriter::rules rec_relations; ///< Named rewriting rules.
	htref main;                    ///< Handle to the main formula.
};

/**
 * @brief Content hash of @p r over the 64-bit tree hashes, the same on every platform.
 * @param r Recurrence relation; null handles hash as 0.
 * @return Combined hash of every rule head and body, then of main.
 */
template <NodeType node>
std::uint64_t rr_hash(const rr<node>& r) noexcept;

} // namespace idni::tau_lang

/**
 * @brief `std::hash` specialization for `rr<node>`, enabling use in hash maps.
 * @tparam node Tree node type.
 */
template <idni::tau_lang::NodeType node>
struct std::hash<idni::tau_lang::rr<node>> {
	/** @brief `rr_hash(f)` narrowed to `size_t`. */
	size_t operator()(const idni::tau_lang::rr<node>& f) const
								noexcept;
};

#include "rr.tmpl.h"

#endif // __IDNI__TAU__RR_H__
