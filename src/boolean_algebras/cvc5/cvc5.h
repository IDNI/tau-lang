// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__CVC5_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__CVC5_H__

#include <cvc5/cvc5.h>

#include "defs.h"

namespace idni::tau_lang {

using bv = cvc5::Term;

static cvc5::TermManager cvc5_term_manager;
static size_t default_bv_size = 16;

// -----------------------------------------------------------------------------
// Basic Boolean algebra infrastructure

size_t get_cvc5_size(const cvc5::Term& b);
cvc5::Term normalize(const cvc5::Term& fm) { return fm; }
bool is_syntactic_zero(const cvc5::Term&) { return false; }
bool is_syntactic_one(const cvc5::Term&) { return false; }
bool is_closed(const cvc5::Term&) { return false; }

// -----------------------------------------------------------------------------
// Basic Boolean algebra operatiors

cvc5::Term operator|(const cvc5::Term& lhs, const cvc5::Term& rhs);
cvc5::Term operator&(const cvc5::Term& lhs, const cvc5::Term& rhs);
cvc5::Term operator^(const cvc5::Term& lhs, const cvc5::Term& rhs);
cvc5::Term operator~(const cvc5::Term& operand);
cvc5::Term operator+(const cvc5::Term& lhs, const cvc5::Term& rhs);
cvc5::Term operator-(const cvc5::Term& lhs, const cvc5::Term& rhs);
cvc5::Term operator*(const cvc5::Term& lhs, const cvc5::Term& rhs);
cvc5::Term operator/(const cvc5::Term& lhs, const cvc5::Term& rhs);
cvc5::Term operator%(const cvc5::Term& lhs, const cvc5::Term& rhs);
cvc5::Term operator<<(const cvc5::Term& lhs, const cvc5::Term& rhs);
cvc5::Term operator>>(const cvc5::Term& lhs, const cvc5::Term& rhs);

bool operator==(const cvc5::Term& lhs, const bool& rhs);
bool operator==(const bool& lhs, const cvc5::Term& rhs);
bool operator!=(const cvc5::Term& lhs, const bool& rhs);
bool operator!=(const bool& lhs, const cvc5::Term& rhs);

// -----------------------------------------------------------------------------
// Builders

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
inline cvc5::Term make_bitvector_cte(const size_t size, const std::string& str,
	const size_t base);
inline cvc5::Term make_bitvector_zero(const size_t size = default_bv_size);
inline cvc5::Term make_bitvector_one(const size_t size = default_bv_size);


// -----------------------------------------------------------------------------
// Queries

} // namespace idni::tau_lang

#include "boolean_algebras/cvc5/cvc5_helpers.tmpl.h"
#include "boolean_algebras/cvc5/cvc5_builders.tmpl.h"
#include "boolean_algebras/cvc5/cvc5.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__CVC5_H__