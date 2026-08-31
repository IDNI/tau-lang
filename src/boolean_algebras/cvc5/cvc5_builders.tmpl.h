// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/cvc5/cvc5.h" // Only for IDE resolution, not really needed.

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "cvc5"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

// Definitions of the builders declared in cvc5.h: thin mkTerm wrappers
// around the global cvc5_term_manager, one cvc5 Kind each.

// Boolean connectives over Boolean-sorted terms.
inline Term make_term_not(const Term& t) {
	return cvc5_term_manager.mkTerm(Kind::NOT, {t});
}

inline Term make_term_and(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::AND, {l, r});
}

inline Term make_term_or(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::OR, {l, r});
}

// Quantifiers: wrap the bound variable(s) in a VARIABLE_LIST and bind
// them in `body`. Variables must come from mkVar (make_bitvector_var).
inline Term make_term_forall(const Term& var, const Term& body) {
	auto vars = cvc5_term_manager.mkTerm(Kind::VARIABLE_LIST, {var});
	return cvc5_term_manager.mkTerm(Kind::FORALL, {vars, body});
}

inline Term make_term_forall(const std::vector<Term>& vars, const Term& body) {
	auto cvc5_var_list = cvc5_term_manager.mkTerm(Kind::VARIABLE_LIST, vars);
	return cvc5_term_manager.mkTerm(Kind::FORALL, {cvc5_var_list, body});
}

inline Term make_term_exists(const Term& var, const Term& body) {
	auto vars = cvc5_term_manager.mkTerm(Kind::VARIABLE_LIST, {var});
	return cvc5_term_manager.mkTerm(Kind::EXISTS, {vars, body});
}

inline Term make_term_exists(const std::vector<Term>& vars, const Term& body) {
	auto cvc5_var_list = cvc5_term_manager.mkTerm(Kind::VARIABLE_LIST, vars);
	return cvc5_term_manager.mkTerm(Kind::EXISTS, {cvc5_var_list, body});
}

// Sort-generic (dis)equality.
inline Term make_term_equal(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::EQUAL, {l, r});
}

inline Term make_term_distinct(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::DISTINCT, {l, r});
}

// Comparisons. TRAP: all four are UNSIGNED bitvector comparisons
// (ULE/UGT/UGE/ULT) despite the generic make_term_* names.

// l <= r, unsigned (BITVECTOR_ULE).
inline Term make_term_less_equal(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_ULE, {l, r});
}

// l > r, unsigned (BITVECTOR_UGT).
inline Term make_term_greater(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_UGT, {l, r});
}

// l >= r, unsigned (BITVECTOR_UGE).
inline Term make_term_greater_equal(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_UGE, {l, r});
}

// l < r, unsigned (BITVECTOR_ULT).
inline Term make_term_less(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_ULT, {l, r});
}

// Bound variable of sort `s`: only usable inside a quantifier body.
inline Term make_bitvector_var(const Sort s, const std::string& name) {
	return cvc5_term_manager.mkVar(s, name.c_str());
}

// Bitwise/arithmetic ops: one BITVECTOR_* kind each, operands of equal
// width. Deviations from the obvious kind are noted per function.
inline Term make_bitvector_not(const Term& t) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_NOT, {t});
}

inline Term make_bitvector_and(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_AND, {l, r});
}

inline Term make_bitvector_nand(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_NAND, {l, r});
}

inline Term make_bitvector_or(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_OR, {l, r});
}

inline Term make_bitvector_nor(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_NOR, {l, r});
}

inline Term make_bitvector_xor(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_XOR, {l, r});
}

inline Term make_bitvector_xnor(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_XNOR, {l, r});
}

inline Term make_bitvector_add(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_ADD, {l, r});
}

inline Term make_bitvector_sub(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_SUB, {l, r});
}

inline Term make_bitvector_mul(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_MULT, {l, r});
}

// UNSIGNED division (BITVECTOR_UDIV), not signed SDIV.
inline Term make_bitvector_div(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_UDIV, {l, r});
}

// UNSIGNED remainder (BITVECTOR_UREM), not signed SREM/SMOD.
inline Term make_bitvector_mod(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_UREM, {l, r});
}

inline Term make_bitvector_shl(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_SHL, {l, r});
}

// Logical (zero-filling) right shift (BITVECTOR_LSHR), not ASHR.
inline Term make_bitvector_shr(const Term& l, const Term& r) {
	return cvc5_term_manager.mkTerm(Kind::BITVECTOR_LSHR, {l, r});
}

// Widen `t` by `extra_bits` leading zero bits.
inline Term make_bitvector_zero_extend(const Term& t, size_t extra_bits) {
	auto op = cvc5_term_manager.mkOp(Kind::BITVECTOR_ZERO_EXTEND, {(uint32_t)extra_bits});
	return cvc5_term_manager.mkTerm(op, {t});
}

// Extract bits hi..lo of `t`, inclusive (hi >= lo, both within width).
inline Term make_bitvector_extract(const Term& t, size_t hi, size_t lo) {
	auto op = cvc5_term_manager.mkOp(Kind::BITVECTOR_EXTRACT, {(uint32_t)hi, (uint32_t)lo});
	return cvc5_term_manager.mkTerm(op, {t});
}

// Constants of width `size`; string forms are parsed in `base` and the
// value must fit in `size` bits (cvc5 rejects it otherwise).
inline cvc5::Term make_bitvector_cte(const size_t size,
		const std::string& str, const size_t base) {
	return cvc5_term_manager.mkBitVector(size, str, base);
}

// All-zeros value (the algebra's bottom element).
inline cvc5::Term make_bitvector_bottom_elem(const size_t size) {
	return cvc5_term_manager.mkBitVector(size, 0);
}

// All-ones value (the algebra's top element).
inline cvc5::Term make_bitvector_top_elem(const size_t size) {
	return cvc5_term_manager.mkBitVector(size, std::string(size, '1'), 2);
}

inline cvc5::Term make_bitvector_value(const size_t size, const size_t value) {
	return cvc5_term_manager.mkBitVector(size, value);
}

// `base` defaults to 2 at the declaration in cvc5.h.
inline cvc5::Term make_bitvector_value(const size_t size, const std::string& value, const size_t base) {
	return cvc5_term_manager.mkBitVector(size, value, base);
}

// TRAP: builds a cvc5 BOOLEAN `true` (mkBoolean), not a bitvector.
inline cvc5::Term make_bitvector_true() {
	return cvc5_term_manager.mkBoolean(true);
}

// TRAP: builds a cvc5 BOOLEAN `false` (mkBoolean), not a bitvector.
inline cvc5::Term make_bitvector_false() {
	return cvc5_term_manager.mkBoolean(false);
}

inline cvc5::Term make_bitvector_zero(const size_t size) {
	return cvc5_term_manager.mkBitVector(size, 0);
}

inline cvc5::Term make_bitvector_one(const size_t size) {
	return cvc5_term_manager.mkBitVector(size, 1);
}

} // namespace idni::tau_lang