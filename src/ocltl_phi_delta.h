// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ocltl_phi_delta.h
 * @brief The feasibility relation phi_delta between a 3-type's sigma/rho
 * projections and its data atoms.
 *
 * A 3-type tau of (m, x, y) over K = d_m + d_x + d_y coordinates is encoded,
 * per ocltl_types.h, as a zero mask of 2^K minterms. phi_delta(sigma, rho, D)
 * relates sigma (tau restricted to m u x), rho (tau restricted to y) and D
 * (which data atoms hold of tau). ocltl_phi_delta_direct evaluates it at one
 * point without enumerating the 3-type space.
 */

#ifndef __IDNI__TAU__OCLTL_PHI_DELTA_H__
#define __IDNI__TAU__OCLTL_PHI_DELTA_H__

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "defs.h"
#include "tau_diagnostics.h"
#include "backends/bdds/bdd_handle.h"

namespace idni::tau_lang {

/// @brief The coordinate layout of a 3-type tau = (m, x, y): [0, d_m) is
/// memory, [d_m, d_m+d_x) is input, [d_m+d_x, k()) is output.
struct ocltl_phi_delta_dims {
	/// Number of memory, input and output coordinates.
	size_t d_m = 0, d_x = 0, d_y = 0;
	/// Total number of coordinates K.
	size_t k() const { return d_m + d_x + d_y; }
};

// ── Quantifier-free Boolean-algebra terms over tau's coordinates ───────────

/// @brief The kind of a term over K coordinates: a coordinate, a constant,
/// or a meet/join of two subterms/complement of one.
enum class ocltl_term_kind { coordinate, zero, one, meet, join, complement };

struct ocltl_delta_term;
/// @brief Shared, immutable handle to a term; subterms may be shared.
using ocltl_delta_term_ptr = std::shared_ptr<const ocltl_delta_term>;

/// @brief A quantifier-free Boolean-algebra term over tau's coordinates.
struct ocltl_delta_term {
	/// Which constructor built the term.
	ocltl_term_kind kind;
	/// The coordinate, valid iff kind == coordinate.
	size_t coordinate = 0;             // valid iff kind == coordinate
	/// Subterms: meet/join use both; complement uses left.
	ocltl_delta_term_ptr left, right;  // meet/join use both; complement uses left
};

/// @brief Return the term for coordinate @p p (an index below K).
ocltl_delta_term_ptr ocltl_term_coordinate(size_t p);
/// @brief Return the constant zero term.
ocltl_delta_term_ptr ocltl_term_zero();
/// @brief Return the constant one term.
ocltl_delta_term_ptr ocltl_term_one();
/// @brief Return the meet of @p a and @p b (both non-null).
ocltl_delta_term_ptr ocltl_term_meet(ocltl_delta_term_ptr a, ocltl_delta_term_ptr b);
/// @brief Return the join of @p a and @p b (both non-null).
ocltl_delta_term_ptr ocltl_term_join(ocltl_delta_term_ptr a, ocltl_delta_term_ptr b);
/// @brief Return the complement of @p a (non-null).
ocltl_delta_term_ptr ocltl_term_complement(ocltl_delta_term_ptr a);

/// @brief The minterm support of a term.
/// @param t Term whose coordinates are all below @p K.
/// @param K Number of coordinates.
/// @return A vector of 2^K bits: bit A is set iff t's characteristic formula
/// evaluates true at minterm index A (coordinate p is bit p of A). Time and
/// memory are O(2^K) per distinct subterm.
std::vector<bool> ocltl_term_support(const ocltl_delta_term_ptr& t, size_t K);

/// @brief A data atom: `term == 0` (negate == false) or `term != 0`
/// (negate == true), over tau's coordinates.
struct ocltl_delta_atom {
	/// The atom's term.
	ocltl_delta_term_ptr term;
	/// `true` for `term != 0`.
	bool negate = false;
};

/// @brief Return the atom `coordinate p == coordinate q`, as a
/// term-equals-zero atom over their symmetric difference.
ocltl_delta_atom ocltl_atom_coordinate_eq(size_t p, size_t q);
/// @brief Return the atom `coordinate p == c`, as a term-equals-zero atom
/// (@p c true = the algebra's unit, false = its zero).
ocltl_delta_atom ocltl_atom_coordinate_const(size_t p, bool c);

// ── Direct, BDD-free evaluation of phi_delta ────────────────────────────────

/// @brief Evaluate phi_delta(sigma, rho, D) at one point, with no BDD.
///
/// Atom i's support is forced all-set when bit i of @p D is set, flipped for
/// a negated atom. A cell A = B | (C << (d_m + d_x)) is a candidate when
/// sigma[B] and rho[C] are both unset and no forced support contains A. The
/// relation holds iff some cell is a candidate, every unset row of @p sigma
/// and every unset column of @p rho holds one, and the support of every
/// atom not forced meets a candidate cell.
/// @param dims Coordinate layout of the 3-type.
/// @param atoms The data atoms; bit i of @p D refers to `atoms[i]`.
/// @param sigma 2^(d_m+d_x) bits indexed by the row B.
/// @param rho 2^d_y bits indexed by the column C.
/// @param D Bitmask of the data atoms that hold of tau.
/// @return `true` iff phi_delta(sigma, rho, D) holds.
bool ocltl_phi_delta_direct(const ocltl_phi_delta_dims& dims,
	const std::vector<ocltl_delta_atom>& atoms,
	const std::vector<bool>& sigma, const std::vector<bool>& rho, size_t D);

// ── phi_delta's own BDD instantiation ───────────────────────────────────────

/// @brief Options of a dedicated BDD instantiation for phi_delta, with
/// node-id and variable-id widths wide enough for the tau-bit counts this
/// relation needs, isolated from the solver's own tables so neither competes
/// with nor corrupts them.
inline constexpr auto ocltl_phi_delta_bdd_options = bdd_options<>::create(38, 24);
/// @brief Handle to a BDD of phi_delta's dedicated instantiation.
using ocltl_phi_delta_bdd = hbdd<Bool, ocltl_phi_delta_bdd_options>;

/// @brief Initialize phi_delta's dedicated BDD instantiation; a no-op if
/// already initialized. Call before building any `ocltl_phi_delta_bdd`.
inline void ocltl_phi_delta_bdd_init() {
	bdd_init<Bool, ocltl_phi_delta_bdd_options>();
}

/// @brief Return the number of distinct decision nodes reachable from @p f
/// (shared nodes counted once; the T/F terminals are not counted).
size_t ocltl_bdd_node_count(const ocltl_phi_delta_bdd& f);

} // namespace idni::tau_lang

#include "ocltl_phi_delta.tmpl.h"

#endif // __IDNI__TAU__OCLTL_PHI_DELTA_H__
