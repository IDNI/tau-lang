// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// cvc5 wrapper layer: makes cvc5::Term usable as a Boolean algebra element
// (alias `bv`, content-based hashing, overloaded BA operators) and provides
// the make_term_*/make_bitvector_* builders that every bv term goes through.
// All terms are created in the single global `cvc5_term_manager`.

#ifndef __IDNI__TAU__BACKENDS__CVC5_H__
#define __IDNI__TAU__BACKENDS__CVC5_H__

#include <cvc5/cvc5.h>
#include <string>
#include <unordered_map>

#include "defs.h"
#include "utility/hashing.h"
#include "tau_string_hash.h"

namespace idni::tau_lang {

/// Element of the bv Boolean algebra: a cvc5 term of bitvector sort.
using bv = cvc5::Term;

/// The one term manager every bv term is created in; never destroyed, so
/// terms stay valid until process exit.
inline cvc5::TermManager& cvc5_term_manager = *new cvc5::TermManager();
/// Width of the type `bv` written without an explicit width.
inline size_t default_bv_size = 16;

/**
 * @brief Content-based hash for a bv (cvc5::Term) BA constant held in a tree
 * node.
 *
 * `node::hashit()` feeds a ba_constant's value into the node hash, and node
 * ordering (`bintree::operator<`, hence DNF clause order, hence which
 * satisfiable path the interpreter tries first) compares hashes first. The
 * default `std::hash<cvc5::Term>` is the term's creation id, which would make
 * that order, and the witness picked for an output the spec leaves free,
 * depend on which terms the process created before. This hashes the term's
 * printed form instead, memoized per term id in a process-wide map that only
 * grows with distinct terms. It is bv's `ba_descriptor::hash_constant` (see
 * bv_descriptor.tmpl.h); every other BA falls back to `std::hash<BA>`.
 *
 * @param t The constant; may be a null term.
 * @return 0 for a null term, else the string hash of `t.toString()`.
 */
inline std::uint64_t hash_bv_constant(const cvc5::Term& t) {
	if (t.isNull()) return 0;
	static std::unordered_map<uint64_t, std::uint64_t> memo;
	const uint64_t id = t.getId();
	if (auto it = memo.find(id); it != memo.end()) return it->second;
	return memo.emplace(id,
		tau_string_hash(t.toString())).first->second;
}

/**
 * @brief Bit width of a bitvector value.
 * @param b A bitvector value (DBG-asserted: `b.isBitVectorValue()`).
 * @return The width of the sort of `b`.
 */
size_t get_cvc5_size(const cvc5::Term& b);

/**
 * @brief Simplify a bv term with cvc5; defined in bv_ba.h.
 *
 * Declared here so templates that use it in lambdas find it under two-phase
 * name lookup.
 */
inline cvc5::Term normalize_bv(const cvc5::Term& fm);

//
// Basic Boolean algebra operators
//
// Each operator builds a new term via the matching make_bitvector_* builder
// (see cvc5_builders.tmpl.h); both operands must have the same bitvector
// sort. Kind mapping:
//   |  BITVECTOR_OR      &  BITVECTOR_AND     ^  BITVECTOR_XOR
//   ~  BITVECTOR_NOT     +  BITVECTOR_ADD     -  BITVECTOR_SUB
//   *  BITVECTOR_MULT    /  BITVECTOR_UDIV    %  BITVECTOR_UREM
//   << BITVECTOR_SHL     >> BITVECTOR_LSHR
// `/` and `%` are UNSIGNED; `>>` is the logical (zero-filling) shift.
//


//
// Builders
//
// Thin cvc5_term_manager.mkTerm wrappers; definitions in
// cvc5_builders.tmpl.h. Binary builders require operands of the same sort.
//

// Boolean connectives (NOT/AND/OR) over Boolean-sorted terms.
/// Boolean NOT of a Boolean-sorted term.
inline cvc5::Term make_term_not(const cvc5::Term& operand);
/// Boolean AND of two Boolean-sorted terms.
inline cvc5::Term make_term_and(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Boolean OR of two Boolean-sorted terms.
inline cvc5::Term make_term_or(const cvc5::Term& lhs, const cvc5::Term& rhs);
// Quantifiers: wrap var(s) in a VARIABLE_LIST and bind them in `form`.
// The variables must be bound variables made with make_bitvector_var.
/// FORALL binding the bound variable `var` in `form`.
inline cvc5::Term make_term_forall(const cvc5::Term& var, const cvc5::Term& form);
/// FORALL binding every bound variable of `vars` (non-empty) in `form`.
inline cvc5::Term make_term_forall(const std::vector<cvc5::Term>& vars,
	const cvc5::Term& form);
/// EXISTS binding the bound variable `var` in `form`.
inline cvc5::Term make_term_exists(const cvc5::Term& var, const cvc5::Term& form);
/// EXISTS binding every bound variable of `vars` (non-empty) in `form`.
inline cvc5::Term make_term_exists(const std::vector<cvc5::Term>& vars,
	const cvc5::Term& form);
// EQUAL / DISTINCT (sort-generic).
/// `lhs = rhs` (EQUAL); operands of the same sort.
inline cvc5::Term make_term_equal(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// `lhs != rhs` (DISTINCT); operands of the same sort.
inline cvc5::Term make_term_distinct(const cvc5::Term& lhs, const cvc5::Term& rhs);
// Comparisons. TRAP: despite the generic names, all four compare
// bitvectors with UNSIGNED semantics:
//   make_term_less_equal    -> BITVECTOR_ULE (unsigned)
//   make_term_greater_equal -> BITVECTOR_UGE (unsigned)
//   make_term_less          -> BITVECTOR_ULT (unsigned)
//   make_term_greater       -> BITVECTOR_UGT (unsigned)
// There are no signed (SLE/SLT/...) builders in this layer.
/// Unsigned `lhs <= rhs` (BITVECTOR_ULE).
inline cvc5::Term make_term_less_equal(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Unsigned `lhs >= rhs` (BITVECTOR_UGE).
inline cvc5::Term make_term_greater_equal(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Unsigned `lhs < rhs` (BITVECTOR_ULT).
inline cvc5::Term make_term_less(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Unsigned `lhs > rhs` (BITVECTOR_UGT).
inline cvc5::Term make_term_greater(const cvc5::Term& lhs, const cvc5::Term& rhs);
// Bound variable of sort `s` (mkVar): only usable inside a quantifier.
/// Bound variable `name` of sort `s` (mkVar), for a quantifier body only.
inline cvc5::Term make_bitvector_var(const cvc5::Sort& s, const std::string& name);
// Bitwise/arithmetic ops, one BITVECTOR_* kind each; div/mod are the
// UNSIGNED BITVECTOR_UDIV/BITVECTOR_UREM, shr the logical BITVECTOR_LSHR.
/// Bitwise NOT (BITVECTOR_NOT).
inline cvc5::Term make_bitvector_not(const cvc5::Term& operand);
/// Bitwise AND (BITVECTOR_AND).
inline cvc5::Term make_bitvector_and(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Bitwise NAND (BITVECTOR_NAND).
inline cvc5::Term make_bitvector_nand(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Bitwise OR (BITVECTOR_OR).
inline cvc5::Term make_bitvector_or(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Bitwise NOR (BITVECTOR_NOR).
inline cvc5::Term make_bitvector_nor(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Bitwise XOR (BITVECTOR_XOR).
inline cvc5::Term make_bitvector_xor(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Bitwise XNOR (BITVECTOR_XNOR).
inline cvc5::Term make_bitvector_xnor(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Modular addition (BITVECTOR_ADD).
inline cvc5::Term make_bitvector_add(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Modular subtraction (BITVECTOR_SUB).
inline cvc5::Term make_bitvector_sub(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Modular multiplication (BITVECTOR_MULT).
inline cvc5::Term make_bitvector_mul(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Unsigned division (BITVECTOR_UDIV).
inline cvc5::Term make_bitvector_div(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Unsigned remainder (BITVECTOR_UREM).
inline cvc5::Term make_bitvector_mod(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Left shift of `lhs` by `rhs` bits (BITVECTOR_SHL).
inline cvc5::Term make_bitvector_shl(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Logical right shift of `lhs` by `rhs` bits (BITVECTOR_LSHR).
inline cvc5::Term make_bitvector_shr(const cvc5::Term& lhs, const cvc5::Term& rhs);
// Unsigned min/max; no bvmin/bvmax kind exists, both are ITE over
// BITVECTOR_ULE.
/// Unsigned minimum, as an ITE over BITVECTOR_ULE.
inline cvc5::Term make_bitvector_min(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Unsigned maximum, as an ITE over BITVECTOR_ULE.
inline cvc5::Term make_bitvector_max(const cvc5::Term& lhs, const cvc5::Term& rhs);
// Indexed ops: widen `t` by `extra_bits` zero bits / extract bits
// hi..lo inclusive (hi >= lo; both within the width of `t`).
/// `t` widened by `extra_bits` leading zero bits.
inline cvc5::Term make_bitvector_zero_extend(const cvc5::Term& t, size_t extra_bits);
/// Bits `hi..lo` of `t`, inclusive.
inline cvc5::Term make_bitvector_extract(const cvc5::Term& t, size_t hi, size_t lo);
// Bitvector constants of width `size`. String forms parse `str`/`value`
// in `base`; the value must fit in `size` bits. bottom_elem/zero are the
// all-zeros value, top_elem the all-ones value, one the value 1.
/// Constant of width `size` parsed from `str` in `base`.
inline cvc5::Term make_bitvector_cte(const size_t size, const std::string& str,
	const size_t base);
/// All-zeros value of width `size` (the algebra's bottom).
inline cvc5::Term make_bitvector_bottom_elem(size_t size);
/// All-ones value of width `size` (the algebra's top).
inline cvc5::Term make_bitvector_top_elem(size_t size);
/// Constant `value` of width `size`.
inline cvc5::Term make_bitvector_value(size_t size, uint64_t value);
/// Constant of width `size` parsed from `value` in `base`.
inline cvc5::Term make_bitvector_value(size_t size, const std::string& value, const size_t base = 2);
// TRAP: despite the names, these two build cvc5 BOOLEAN constants
// (mkBoolean(true/false)), not bitvectors of any width.
/// The Boolean constant `true` (not a bitvector).
inline cvc5::Term make_bitvector_true();
/// The Boolean constant `false` (not a bitvector).
inline cvc5::Term make_bitvector_false();
/// The value 0 of width `size`.
inline cvc5::Term make_bitvector_zero(size_t size);
/// The value 1 of width `size`.
inline cvc5::Term make_bitvector_one(size_t size);

} // namespace idni::tau_lang

namespace cvc5 {

// Bitwise and arithmetic operators on Term. In cvc5's namespace, not ours:
// they take a cvc5 type, so this is where argument-dependent lookup finds them.
/// Bitwise OR (make_bitvector_or).
inline cvc5::Term operator|(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Bitwise AND (make_bitvector_and).
inline cvc5::Term operator&(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Bitwise XOR (make_bitvector_xor).
inline cvc5::Term operator^(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Bitwise NOT (make_bitvector_not).
inline cvc5::Term operator~(const cvc5::Term& operand);
/// Modular addition (make_bitvector_add).
inline cvc5::Term operator+(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Modular subtraction (make_bitvector_sub).
inline cvc5::Term operator-(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Modular multiplication (make_bitvector_mul).
inline cvc5::Term operator*(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Unsigned division (make_bitvector_div).
inline cvc5::Term operator/(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Unsigned remainder (make_bitvector_mod).
inline cvc5::Term operator%(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Left shift (make_bitvector_shl).
inline cvc5::Term operator<<(const cvc5::Term& lhs, const cvc5::Term& rhs);
/// Logical right shift (make_bitvector_shr).
inline cvc5::Term operator>>(const cvc5::Term& lhs, const cvc5::Term& rhs);

} // namespace cvc5



#include "backends/cvc5/cvc5_helpers.tmpl.h"
#include "backends/cvc5/cvc5_builders.tmpl.h"
#include "backends/cvc5/cvc5.tmpl.h"

#endif // __IDNI__TAU__BACKENDS__CVC5_H__