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

// The coordinate layout of a 3-type tau = (m, x, y): [0, d_m) is memory,
// [d_m, d_m+d_x) is input, [d_m+d_x, k()) is output.
struct ocltl_phi_delta_dims {
	size_t d_m = 0, d_x = 0, d_y = 0;
	size_t k() const { return d_m + d_x + d_y; }
};

// ── Quantifier-free Boolean-algebra terms over tau's coordinates ───────────

// A term over K coordinates: a coordinate, a constant, or a meet/join of two
// subterms/complement of one.
enum class ocltl_term_kind { coordinate, zero, one, meet, join, complement };

struct ocltl_delta_term;
using ocltl_delta_term_ptr = std::shared_ptr<const ocltl_delta_term>;

struct ocltl_delta_term {
	ocltl_term_kind kind;
	size_t coordinate = 0;             // valid iff kind == coordinate
	ocltl_delta_term_ptr left, right;  // meet/join use both; complement uses left
};

ocltl_delta_term_ptr ocltl_term_coordinate(size_t p);
ocltl_delta_term_ptr ocltl_term_zero();
ocltl_delta_term_ptr ocltl_term_one();
ocltl_delta_term_ptr ocltl_term_meet(ocltl_delta_term_ptr a, ocltl_delta_term_ptr b);
ocltl_delta_term_ptr ocltl_term_join(ocltl_delta_term_ptr a, ocltl_delta_term_ptr b);
ocltl_delta_term_ptr ocltl_term_complement(ocltl_delta_term_ptr a);

// The minterm support of `t` over K coordinates: bit A is set iff t's
// characteristic formula evaluates true at minterm index A.
std::vector<bool> ocltl_term_support(const ocltl_delta_term_ptr& t, size_t K);

// A data atom: `term == 0` (negate == false) or `term != 0` (negate == true),
// over tau's coordinates.
struct ocltl_delta_atom {
	ocltl_delta_term_ptr term;
	bool negate = false;
};

// coordinate p == coordinate q, as a term-equals-zero atom.
ocltl_delta_atom ocltl_atom_coordinate_eq(size_t p, size_t q);
// coordinate p == the constant c (true = the algebra's unit, false = its zero).
ocltl_delta_atom ocltl_atom_coordinate_const(size_t p, bool c);

// ── Direct, BDD-free evaluation of phi_delta ────────────────────────────────

// True iff phi_delta(sigma, rho, D) holds; sigma[B]/rho[C] index rows/columns, no BDD involved.
bool ocltl_phi_delta_direct(const ocltl_phi_delta_dims& dims,
	const std::vector<ocltl_delta_atom>& atoms,
	const std::vector<bool>& sigma, const std::vector<bool>& rho, size_t D);

// ── phi_delta's own BDD instantiation ───────────────────────────────────────

// A dedicated BDD instantiation for phi_delta, with node-id and variable-id
// widths wide enough for the tau-bit counts this relation needs, isolated
// from the solver's own tables so neither competes with nor corrupts them.
inline constexpr auto ocltl_phi_delta_bdd_options = bdd_options<>::create(38, 24);
using ocltl_phi_delta_bdd = hbdd<Bool, ocltl_phi_delta_bdd_options>;

// Initializes phi_delta's dedicated BDD instantiation; a no-op if already
// initialized.
inline void ocltl_phi_delta_bdd_init() {
	bdd_init<Bool, ocltl_phi_delta_bdd_options>();
}

// Number of distinct decision nodes reachable from `f` (shared nodes counted
// once; the T/F terminals are not counted).
size_t ocltl_bdd_node_count(const ocltl_phi_delta_bdd& f);

} // namespace idni::tau_lang

#include "ocltl_phi_delta.tmpl.h"

#endif // __IDNI__TAU__OCLTL_PHI_DELTA_H__
