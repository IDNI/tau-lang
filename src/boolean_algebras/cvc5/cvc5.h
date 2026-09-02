// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// cvc5 wrapper layer: makes cvc5::Term usable as a Boolean algebra element
// (alias `bv`, content-based hashing, overloaded BA operators) and provides
// the make_term_*/make_bitvector_* builders that every bv term goes through.
// All terms are created in the single global `cvc5_term_manager`.

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__CVC5_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__CVC5_H__

#include <cvc5/cvc5.h>
#include <string>
#include <unordered_map>

#include "defs.h"

namespace idni::tau_lang {

using bv = cvc5::Term;

inline cvc5::TermManager& cvc5_term_manager = *new cvc5::TermManager();

/**
 * @brief Content-based hash for a BA constant held in a tree node.
 *
 * `node::hashit()` feeds a ba_constant's value into the node hash, and node
 * ordering (`bintree::operator<`, hence DNF clause order, hence which
 * satisfiable path the interpreter tries first) compares hashes first. For
 * `bv` the default `std::hash<cvc5::Term>` is the term's creation id, so the
 * order of two clauses -- and the witness picked for an output the spec
 * leaves free -- depended on which terms the process had created before
 * (GitHub #89: same spec, different value per driver and per run). The
 * `cvc5::Term` specialization hashes the term's printed form instead,
 * memoized per term id (ids are stable within a process; the map only
 * grows with distinct terms). Every other BA keeps its `std::hash`, which
 * is already content-derived (`tau_ba` goes through `rr`'s tree hash).
 */
template <typename T>
struct ba_constant_hash {
	size_t operator()(const T& v) const { return std::hash<T>{}(v); }
};

template <>
struct ba_constant_hash<cvc5::Term> {
	size_t operator()(const cvc5::Term& t) const {
		if (t.isNull()) return 0;
		static std::unordered_map<uint64_t, size_t> memo;
		const uint64_t id = t.getId();
		if (auto it = memo.find(id); it != memo.end()) return it->second;
		return memo.emplace(id,
			std::hash<std::string>{}(t.toString())).first->second;
	}
};

// Bit width of the sort of bitvector value `b`. DBG-asserts
// b.isBitVectorValue(). Defined in cvc5_helpers.tmpl.h.
size_t get_cvc5_size(const cvc5::Term& b);

// Forward declaration — defined in bv_ba.h; declared here for use in lambdas
// inside templates where two-phase name lookup applies.
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

inline cvc5::Term operator|(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term operator&(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term operator^(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term operator~(const cvc5::Term& operand);
inline cvc5::Term operator+(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term operator-(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term operator*(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term operator/(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term operator%(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term operator<<(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term operator>>(const cvc5::Term& lhs, const cvc5::Term& rhs);

//
// Builders
//
// Thin cvc5_term_manager.mkTerm wrappers; definitions in
// cvc5_builders.tmpl.h. Binary builders require operands of the same sort.
//

// Boolean connectives (NOT/AND/OR) over Boolean-sorted terms.
inline cvc5::Term make_term_not(const cvc5::Term& operand);
inline cvc5::Term make_term_and(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_term_or(const cvc5::Term& lhs, const cvc5::Term& rhs);
// Quantifiers: wrap var(s) in a VARIABLE_LIST and bind them in `form`.
// The variables must be bound variables made with make_bitvector_var.
inline cvc5::Term make_term_forall(const cvc5::Term& var, const cvc5::Term& form);
inline cvc5::Term make_term_forall(const std::vector<cvc5::Term>& vars,
	const cvc5::Term& form);
inline cvc5::Term make_term_exists(const cvc5::Term& var, const cvc5::Term& form);
inline cvc5::Term make_term_exists(const std::vector<cvc5::Term>& vars,
	const cvc5::Term& form);
// EQUAL / DISTINCT (sort-generic).
inline cvc5::Term make_term_equal(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_term_distinct(const cvc5::Term& lhs, const cvc5::Term& rhs);
// Comparisons. TRAP: despite the generic names, all four compare
// bitvectors with UNSIGNED semantics:
//   make_term_less_equal    -> BITVECTOR_ULE (unsigned)
//   make_term_greater_equal -> BITVECTOR_UGE (unsigned)
//   make_term_less          -> BITVECTOR_ULT (unsigned)
//   make_term_greater       -> BITVECTOR_UGT (unsigned)
// There are no signed (SLE/SLT/...) builders in this layer.
inline cvc5::Term make_term_less_equal(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_term_greater_equal(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_term_less(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_term_greater(const cvc5::Term& lhs, const cvc5::Term& rhs);
// Bound variable of sort `s` (mkVar): only usable inside a quantifier.
inline cvc5::Term make_bitvector_var(const cvc5::Sort& s, const std::string& name);
// Bitwise/arithmetic ops, one BITVECTOR_* kind each; div/mod are the
// UNSIGNED BITVECTOR_UDIV/BITVECTOR_UREM, shr the logical BITVECTOR_LSHR.
inline cvc5::Term make_bitvector_not(const cvc5::Term& operand);
inline cvc5::Term make_bitvector_and(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_bitvector_nand(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_bitvector_or(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_bitvector_nor(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_bitvector_xor(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_bitvector_xnor(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_bitvector_add(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_bitvector_sub(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_bitvector_mul(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_bitvector_div(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_bitvector_mod(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_bitvector_shl(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_bitvector_shr(const cvc5::Term& lhs, const cvc5::Term& rhs);
// Unsigned min/max; no bvmin/bvmax kind exists, both are ITE over
// BITVECTOR_ULE.
inline cvc5::Term make_bitvector_min(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_bitvector_max(const cvc5::Term& lhs, const cvc5::Term& rhs);
// Indexed ops: widen `t` by `extra_bits` zero bits / extract bits
// hi..lo inclusive (hi >= lo; both within the width of `t`).
inline cvc5::Term make_bitvector_zero_extend(const cvc5::Term& t, size_t extra_bits);
inline cvc5::Term make_bitvector_extract(const cvc5::Term& t, size_t hi, size_t lo);
// Bitvector constants of width `size`. String forms parse `str`/`value`
// in `base`; the value must fit in `size` bits. bottom_elem/zero are the
// all-zeros value, top_elem the all-ones value, one the value 1.
inline cvc5::Term make_bitvector_cte(const size_t size, const std::string& str,
	const size_t base);
inline cvc5::Term make_bitvector_bottom_elem(size_t size);
inline cvc5::Term make_bitvector_top_elem(size_t size);
inline cvc5::Term make_bitvector_value(size_t size, size_t value);
inline cvc5::Term make_bitvector_value(size_t size, const std::string& value, const size_t base = 2);
// TRAP: despite the names, these two build cvc5 BOOLEAN constants
// (mkBoolean(true/false)), not bitvectors of any width.
inline cvc5::Term make_bitvector_true();
inline cvc5::Term make_bitvector_false();
inline cvc5::Term make_bitvector_zero(size_t size);
inline cvc5::Term make_bitvector_one(size_t size);

} // namespace idni::tau_lang

#include "boolean_algebras/cvc5/cvc5_helpers.tmpl.h"
#include "boolean_algebras/cvc5/cvc5_builders.tmpl.h"
#include "boolean_algebras/cvc5/cvc5.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__CVC5_H__