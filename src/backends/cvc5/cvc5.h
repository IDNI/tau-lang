// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BACKENDS__CVC5_H__
#define __IDNI__TAU__BACKENDS__CVC5_H__

#include <cvc5/cvc5.h>
#include <string>
#include <unordered_map>

#include "defs.h"

namespace idni::tau_lang {

using bv = cvc5::Term;

inline cvc5::TermManager& cvc5_term_manager = *new cvc5::TermManager();
inline size_t default_bv_size = 16;

/**
 * @brief Content-based hash for a bv (cvc5::Term) BA constant held in a tree
 * node.
 *
 * `node::hashit()` feeds a ba_constant's value into the node hash, and node
 * ordering (`bintree::operator<`, hence DNF clause order, hence which
 * satisfiable path the interpreter tries first) compares hashes first. The
 * default `std::hash<cvc5::Term>` is the term's creation id, so the order of
 * two clauses -- and the witness picked for an output the spec leaves free --
 * depended on which terms the process had created before (GitHub #89: same
 * spec, different value per driver and per run). This hashes the term's
 * printed form instead, memoized per term id (ids are stable within a
 * process; the map only grows with distinct terms). It is bv's
 * `ba_descriptor::hash_constant` (see bv_descriptor.tmpl.h); every other BA
 * declares nothing and falls back to `std::hash<BA>`, which is already
 * content-derived for them (`tau_ba` goes through `rr`'s tree hash).
 */
inline size_t hash_bv_constant(const cvc5::Term& t) {
	if (t.isNull()) return 0;
	static std::unordered_map<uint64_t, size_t> memo;
	const uint64_t id = t.getId();
	if (auto it = memo.find(id); it != memo.end()) return it->second;
	return memo.emplace(id,
		std::hash<std::string>{}(t.toString())).first->second;
}

size_t get_cvc5_size(const cvc5::Term& b);

// Forward declaration — defined in bv_ba.h; declared here for use in lambdas
// inside templates where two-phase name lookup applies.
inline cvc5::Term normalize_bv(const cvc5::Term& fm);

//
// Basic Boolean algebra operatiors
//


//
// Builders
//

inline cvc5::Term make_term_not(const cvc5::Term& operand);
inline cvc5::Term make_term_and(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_term_or(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_term_forall(const cvc5::Term& var, const cvc5::Term& form);
inline cvc5::Term make_term_forall(const std::vector<cvc5::Term>& vars,
	const cvc5::Term& form);
inline cvc5::Term make_term_exists(const cvc5::Term& var, const cvc5::Term& form);
inline cvc5::Term make_term_exists(const std::vector<cvc5::Term>& vars,
	const cvc5::Term& form);
inline cvc5::Term make_term_equal(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_term_distinct(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_term_less_equal(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_term_greater_equal(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_term_less(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_term_greater(const cvc5::Term& lhs, const cvc5::Term& rhs);
inline cvc5::Term make_bitvector_var(const cvc5::Sort& s, const std::string& name);
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
inline cvc5::Term make_bitvector_zero_extend(const cvc5::Term& t, size_t extra_bits);
inline cvc5::Term make_bitvector_extract(const cvc5::Term& t, size_t hi, size_t lo);
inline cvc5::Term make_bitvector_cte(const size_t size, const std::string& str,
	const size_t base);
inline cvc5::Term make_bitvector_bottom_elem(size_t size);
inline cvc5::Term make_bitvector_top_elem(size_t size);
inline cvc5::Term make_bitvector_value(size_t size, size_t value);
inline cvc5::Term make_bitvector_value(size_t size, const std::string& value, const size_t base = 2);
inline cvc5::Term make_bitvector_true();
inline cvc5::Term make_bitvector_false();
inline cvc5::Term make_bitvector_zero(size_t size);
inline cvc5::Term make_bitvector_one(size_t size);

} // namespace idni::tau_lang

namespace cvc5 {

// Bitwise and arithmetic operators on Term. In cvc5's namespace, not ours:
// they take a cvc5 type, so this is where argument-dependent lookup finds them.
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

} // namespace cvc5



#include "backends/cvc5/cvc5_helpers.tmpl.h"
#include "backends/cvc5/cvc5_builders.tmpl.h"
#include "backends/cvc5/cvc5.tmpl.h"

#endif // __IDNI__TAU__BACKENDS__CVC5_H__