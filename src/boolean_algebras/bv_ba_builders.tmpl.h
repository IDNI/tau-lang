// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "bv_ba.h" // Only for IDE resolution, not really needed.

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

inline bv make_term_not(const bv& t) {
	return bv_solver.mkTerm(Kind::NOT, {t});
}

inline bv make_term_and(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::AND, {l, r});
}

inline bv make_term_or(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::OR, {l, r});
}

inline bv make_term_forall(const bv& var, const bv& body) {
	auto vars = bv_solver.mkTerm(Kind::VARIABLE_LIST, {var});
	return bv_solver.mkTerm(Kind::FORALL, {vars, body});
}

inline bv make_term_forall(const bvs& vars, const bv& body) {
	auto cvc5_var_list = bv_solver.mkTerm(Kind::VARIABLE_LIST, vars);
	return bv_solver.mkTerm(Kind::FORALL, {cvc5_var_list, body});
}

inline bv make_term_exists(const bv& var, const bv& body) {
	auto vars = bv_solver.mkTerm(Kind::VARIABLE_LIST, {var});
	return bv_solver.mkTerm(Kind::EXISTS, {vars, body});
}

inline bv make_term_exists(const bvs& vars, const bv& body) {
	auto cvc5_var_list = bv_solver.mkTerm(Kind::VARIABLE_LIST, vars);
	return bv_solver.mkTerm(Kind::EXISTS, {cvc5_var_list, body});
}

inline bv make_term_equal(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::EQUAL, {l, r});
}

inline bv make_term_distinct(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::DISTINCT, {l, r});
}

inline bv make_term_less_equal(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_ULE, {l, r});
}

inline bv make_term_greater(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_UGT, {l, r});
}

inline bv make_term_greater_equal(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_UGE, {l, r});
}

inline bv make_bv_var(const sort s, const std::string& name) {
	return bv_solver.mkVar(s, name.c_str());
}

inline bv make_term_less(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_ULT, {l, r});
}

inline bv make_bv_not(const bv& t) {
	return bv_solver.mkTerm(Kind::BITVECTOR_NOT, {t});
}

inline bv make_bv_and(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_AND, {l, r});
}

inline bv make_bv_or(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_OR, {l, r});
}

inline bv make_bv_nor(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_NOR, {l, r});
}

inline bv make_bv_xor(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_XOR, {l, r});
}

inline bv make_bv_xnor(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_XNOR, {l, r});
}

inline bv make_bv_nand(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_NAND, {l, r});
}

inline bv make_bv_add(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_ADD, {l, r});
}

inline bv make_bv_sub(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_SUB, {l, r});
}

inline bv make_bv_mul(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_MULT, {l, r});
}

inline bv make_bv_div(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_UDIV, {l, r});
}

inline bv make_bv_mod(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_SMOD, {l, r});
}

inline bv make_bv_shl(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_SHL, {l, r});
}

inline bv make_bv_shr(const bv& l, const bv& r) {
	return bv_solver.mkTerm(Kind::BITVECTOR_LSHR, {l, r});
}

} // namespace idni::tau_lang