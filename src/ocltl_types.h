// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ocltl_types.h
 * @brief Complete-type machinery for atomless Boolean algebras.
 *
 * A complete k-type over an atomless Boolean algebra is encoded as the zero
 * mask of the 2^k minterms of a k-tuple. Every Boolean algebra whose
 * descriptor says `atomless = true` shares this one type space.
 */

#ifndef __IDNI__TAU__OCLTL_TYPES_H__
#define __IDNI__TAU__OCLTL_TYPES_H__

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include "defs.h"
#include "boolean_algebras/ba_descriptor.h"
#include "boolean_algebras/ba_pack_traits.h"

namespace idni::tau_lang {

// ── T_k: the zero-mask type ─────────────────────────────────────────────────

/// Largest k this word width supports: 2^k minterms must fit in the word.
inline constexpr size_t ocltl_max_k = 6;

/// A complete k-type as its zero mask: bit A is 1 iff minterm m_A(x̄) = 0.
using ocltl_type_mask = uint64_t;

/// Number of minterms of a k-tuple, 2^k; requires k <= ocltl_max_k.
constexpr size_t ocltl_minterm_count(size_t k) {
	DBG(assert(k <= ocltl_max_k);)
	return size_t{1} << k;
}

/// The forbidden all-minterms-zero mask over 2^k bits.
constexpr ocltl_type_mask ocltl_full_mask(size_t k) {
	size_t bits = ocltl_minterm_count(k);
	return bits >= 64 ? ~ocltl_type_mask{0} : (ocltl_type_mask{1} << bits) - 1;
}

/// |T_k| = 2^(2^k) - 1, the valid masks being exactly the integers below the
/// forbidden one.
constexpr ocltl_type_mask ocltl_type_count(size_t k) { return ocltl_full_mask(k); }

/// A mask is a valid k-type iff it does not make every minterm zero.
constexpr bool ocltl_is_valid_type(ocltl_type_mask z, size_t k) {
	return z != ocltl_full_mask(k);
}

// ── Restriction and shift ───────────────────────────────────────────────────

/**
 * @brief Restrict @p z to the coordinates in @p keep: a restricted minterm
 * is zero iff every full minterm above it is zero.
 * @param z A k-type mask.
 * @param k Arity of @p z; at most ocltl_max_k.
 * @param keep Ascending coordinate indices into [0,k).
 * @return The keep.size()-type mask.
 */
inline ocltl_type_mask ocltl_restrict(ocltl_type_mask z, size_t k,
	const std::vector<size_t>& keep)
{
	DBG(assert(k <= ocltl_max_k);)
	size_t new_k = keep.size();
	ocltl_type_mask result = ocltl_full_mask(new_k);
	for (size_t A = 0; A < ocltl_minterm_count(k); ++A) {
		if (((z >> A) & 1) != 0) continue; // m_A already zero: no witness needed
		size_t B = 0;
		for (size_t j = 0; j < new_k; ++j)
			if ((A >> keep[j]) & 1) B |= (size_t{1} << j);
		result &= ~(ocltl_type_mask{1} << B); // m_A nonzero => m_B nonzero
	}
	return result;
}

/**
 * @brief Drop coordinate @p i of @p z: AND-fold its two extensions,
 * renumbering the coordinates above @p i down by one.
 * @return The (k-1)-type mask; requires k >= 1 and i < k.
 */
inline ocltl_type_mask ocltl_drop_coordinate(ocltl_type_mask z, size_t k, size_t i) {
	DBG(assert(k >= 1 && i < k);)
	size_t new_k = k - 1;
	ocltl_type_mask result = 0;
	for (size_t B = 0; B < ocltl_minterm_count(new_k); ++B) {
		size_t low = B & ((size_t{1} << i) - 1);
		size_t high = B >> i;
		size_t a0 = (high << (i + 1)) | low;        // A with coordinate i = 0
		size_t a1 = a0 | (size_t{1} << i);           // A with coordinate i = 1
		if (((z >> a0) & 1) && ((z >> a1) & 1))
			result |= (ocltl_type_mask{1} << B);
	}
	return result;
}

/// The memory type a 3-type determines for the next step: its third coordinate.
inline ocltl_type_mask ocltl_sh_restrict(ocltl_type_mask tau3) {
	return ocltl_restrict(tau3, 3, { 2 });
}

// ── Wide-K restriction ──────────────────────────────────────────────────────
// Same algorithms as above, generalized past ocltl_max_k: the mask is a
// 2^k-length bitset (matching what ocltl_phi_delta_direct already uses for
// sigma/rho) instead of a uint64_t word, so k can reach atomless2.tau's 18.

/// A k-type zero mask as a 2^k-length bitset; bit A as in ocltl_type_mask.
using ocltl_type_mask_wide = std::vector<bool>;

/// Number of minterms of a k-tuple, 2^k, not capped at ocltl_max_k.
inline size_t ocltl_wide_minterm_count(size_t k) { return size_t{1} << k; }

/// The forbidden all-minterms-zero mask over 2^k bits, not capped.
inline ocltl_type_mask_wide ocltl_wide_full_mask(size_t k) {
	return ocltl_type_mask_wide(ocltl_wide_minterm_count(k), true);
}

/// A mask is a valid k-type iff it does not make every minterm zero.
inline bool ocltl_wide_is_valid_type(const ocltl_type_mask_wide& z) {
	for (bool b : z) if (!b) return true;
	return false;
}

/// Restrict `z` to the coordinates in `keep` -- same algorithm and contract
/// as ocltl_restrict, over a 2^k-length bitset (z.size() must be 2^k).
inline ocltl_type_mask_wide ocltl_restrict_wide(const ocltl_type_mask_wide& z,
	size_t k, const std::vector<size_t>& keep)
{
	DBG(assert(z.size() == ocltl_wide_minterm_count(k));)
	size_t new_k = keep.size();
	ocltl_type_mask_wide result(ocltl_wide_minterm_count(new_k), true);
	for (size_t A = 0; A < ocltl_wide_minterm_count(k); ++A) {
		if (z[A]) continue; // m_A already zero: no witness needed
		size_t B = 0;
		for (size_t j = 0; j < new_k; ++j)
			if ((A >> keep[j]) & 1) B |= (size_t{1} << j);
		result[B] = false; // m_A nonzero => m_B nonzero
	}
	return result;
}

/// Drop coordinate `i` -- same algorithm and contract as
/// ocltl_drop_coordinate, over a 2^k-length bitset.
inline ocltl_type_mask_wide ocltl_drop_coordinate_wide(
	const ocltl_type_mask_wide& z, size_t k, size_t i)
{
	DBG(assert(k >= 1 && i < k);)
	DBG(assert(z.size() == ocltl_wide_minterm_count(k));)
	size_t new_k = k - 1;
	ocltl_type_mask_wide result(ocltl_wide_minterm_count(new_k), false);
	for (size_t B = 0; B < ocltl_wide_minterm_count(new_k); ++B) {
		size_t low = B & ((size_t{1} << i) - 1);
		size_t high = B >> i;
		size_t a0 = (high << (i + 1)) | low;        // A with coordinate i = 0
		size_t a1 = a0 | (size_t{1} << i);           // A with coordinate i = 1
		result[B] = z[a0] && z[a1];
	}
	return result;
}

/// The memory type a 3-type determines for the next step: its third coordinate.
inline ocltl_type_mask_wide ocltl_sh_restrict_wide(const ocltl_type_mask_wide& tau3) {
	return ocltl_restrict_wide(tau3, 3, { 2 });
}

// Conversions between the capped (k <= ocltl_max_k) and wide representations,
// for cross-checking the two implementations against each other.
/// The k-type mask @p z as a 2^k-length bitset.
inline ocltl_type_mask_wide ocltl_to_wide_mask(ocltl_type_mask z, size_t k) {
	ocltl_type_mask_wide w(ocltl_wide_minterm_count(k));
	for (size_t A = 0; A < w.size(); ++A) w[A] = ((z >> A) & 1) != 0;
	return w;
}

/// The wide mask @p w as a word mask; w.size() must be at most 2^ocltl_max_k.
inline ocltl_type_mask ocltl_from_wide_mask(const ocltl_type_mask_wide& w) {
	DBG(assert(w.size() <= ocltl_minterm_count(ocltl_max_k));)
	ocltl_type_mask z = 0;
	for (size_t A = 0; A < w.size(); ++A) if (w[A]) z |= (ocltl_type_mask{1} << A);
	return z;
}

// ── Witness construction ─────────────────────────────────────────────────────

/// An atomless Boolean algebra whose constants carry meet, join and complement.
template <typename BA, typename Node>
concept ocltl_atomless_ba =
	ba_has_descriptor_v<Node, BA>
 && ba_descriptor<BA, Node>::atomless
 && requires(const BA& x, const BA& y) {
        { x & y } -> std::same_as<BA>;
        { x | y } -> std::same_as<BA>;
        { ~x }    -> std::same_as<BA>;
    };

/// The unit element, parsed from the algebra's own one-literal.
template <typename BA, typename Node>
requires ocltl_atomless_ba<BA, Node>
BA ocltl_unit();

/// m_A(ā): the meet of the literals a_i, complemented where bit i of A is 0.
/// The empty tuple has one minterm (A = 0), the unit element.
template <typename BA, typename Node>
requires ocltl_atomless_ba<BA, Node>
BA ocltl_minterm(const std::vector<BA>& a, size_t A);

/**
 * @brief tp(ā): the concrete k-type of tuple @p a, as a zero mask.
 * @param a Tuple of at most ocltl_max_k elements.
 * @return The zero mask, or the error of a failed `is_zero` decision.
 */
template <typename BA, typename Node>
requires ocltl_atomless_ba<BA, Node>
result<ocltl_type_mask> ocltl_type_of(const std::vector<BA>& a);

/**
 * @brief A witness b extending @p a to the requested type, whose new
 * coordinate is the mask's highest bit: each nonzero minterm of @p a
 * contributes all of itself, nothing, or a proper part of itself.
 * @param a The committed tuple; a.size() + 1 <= ocltl_max_k.
 * @param tau A (k+1)-type mask that extends tp(@p a).
 * @param st Splitter used for a proper part.
 * @return The witness b, or the error of a failed `is_zero` or `splitter`.
 */
template <typename BA, typename Node>
requires ocltl_atomless_ba<BA, Node>
result<BA> ocltl_witness(const std::vector<BA>& a, ocltl_type_mask tau,
	splitter_type st = splitter_type::upper);

/// Same as ocltl_witness, over a 2^(k+1)-length bitset instead of a capped
/// uint64_t mask, so a.size()+1 can exceed ocltl_max_k.
template <typename BA, typename Node>
requires ocltl_atomless_ba<BA, Node>
result<BA> ocltl_witness_wide(const std::vector<BA>& a, const ocltl_type_mask_wide& tau,
	splitter_type st = splitter_type::upper);

} // namespace idni::tau_lang

#include "ocltl_types.tmpl.h"

#endif // __IDNI__TAU__OCLTL_TYPES_H__
