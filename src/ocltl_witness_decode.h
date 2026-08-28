// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ocltl_witness_decode.h
 * @brief Runtime counterpart of ocltl_phi_delta_direct: decodes the new
 * coordinate's target mask from atoms that directly constrain it.
 */

#ifndef __IDNI__TAU__OCLTL_WITNESS_DECODE_H__
#define __IDNI__TAU__OCLTL_WITNESS_DECODE_H__

#include <cstddef>
#include <vector>

#include "ocltl_types.h"

namespace idni::tau_lang {

// Sentinel `coord` value: the atom's other side is the algebra's own
// constant (see `const_is_one`), not one of `a`'s coordinates.
inline constexpr size_t ocltl_target_none = static_cast<size_t>(-1);

// One structured atom directly constraining the new coordinate b: either
// "b == a[coord]" or "b == the algebra's unit/zero", each with the polarity
// the edge's D-pattern requires (asserted == the atom must hold).
struct ocltl_target_atom {
	size_t coord = ocltl_target_none;
	bool const_is_one = false; // meaningful only when coord == ocltl_target_none
	bool asserted = true;
};

// Builds the (k+1)-bit-wide Prop-4 target mask ocltl_witness_wide expects.
// "b == value" atoms force full inclusion/exclusion of value's minterms;
// "!=" atoms stay unconstrained, relying on the splitter to differ by default.
inline ocltl_type_mask_wide ocltl_build_target_mask(size_t k,
	const std::vector<ocltl_target_atom>& atoms)
{
	size_t n = ocltl_wide_minterm_count(k);
	ocltl_type_mask_wide tau(2 * n, false);
	for (size_t A = 0; A < n; ++A) {
		bool force_in = false, force_out = false;
		for (const auto& c : atoms) {
			if (!c.asserted) continue; // "!=" atoms: leave split, see above
			bool in_minterm = c.coord == ocltl_target_none
				? c.const_is_one : (((A >> c.coord) & 1) != 0);
			if (in_minterm) force_in = true; else force_out = true;
		}
		DBG(assert(!(force_in && force_out));) // conflicting positive atoms: caller error
		if (force_in) tau[A] = true;
		else if (force_out) tau[n + A] = true;
	}
	return tau;
}

// Decodes the new coordinate b via Prop. 4 / ocltl_witness_wide from the
// concrete, already-committed tuple `a` and the atoms that constrain b
// directly.
template <typename BA, typename Node>
requires ocltl_atomless_ba<BA, Node>
BA ocltl_decode_witness(const std::vector<BA>& a,
	const std::vector<ocltl_target_atom>& atoms,
	splitter_type st = splitter_type::upper);

} // namespace idni::tau_lang

#include "ocltl_witness_decode.tmpl.h"

#endif // __IDNI__TAU__OCLTL_WITNESS_DECODE_H__
