// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "boolean_algebras/cvc5/cvc5.h" // Only for IDE resolution, not really needed.

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

inline Term make_term_not(const Term& t) {
	return cvc5_solver.mkTerm(Kind::NOT, {t});
}

inline Term make_term_and(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::AND, {l, r});
}

inline Term make_term_or(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::OR, {l, r});
}

inline Term make_term_forall(const Term& var, const Term& body) {
	auto vars = cvc5_solver.mkTerm(Kind::VARIABLE_LIST, {var});
	return cvc5_solver.mkTerm(Kind::FORALL, {vars, body});
}

inline Term make_term_forall(const std::vector<Term>& vars, const Term& body) {
	auto cvc5_var_list = cvc5_solver.mkTerm(Kind::VARIABLE_LIST, vars);
	return cvc5_solver.mkTerm(Kind::FORALL, {cvc5_var_list, body});
}

inline Term make_term_exists(const Term& var, const Term& body) {
	auto vars = cvc5_solver.mkTerm(Kind::VARIABLE_LIST, {var});
	return cvc5_solver.mkTerm(Kind::EXISTS, {vars, body});
}

inline Term make_term_exists(const std::vector<Term>& vars, const Term& body) {
	auto cvc5_var_list = cvc5_solver.mkTerm(Kind::VARIABLE_LIST, vars);
	return cvc5_solver.mkTerm(Kind::EXISTS, {cvc5_var_list, body});
}

inline Term make_term_equal(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::EQUAL, {l, r});
}

inline Term make_term_distinct(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::DISTINCT, {l, r});
}

inline Term make_term_less_equal(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_ULE, {l, r});
}

inline Term make_term_greater(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_UGT, {l, r});
}

inline Term make_term_greater_equal(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_UGE, {l, r});
}

inline Term make_term_less(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_ULT, {l, r});
}

inline Term make_bitvector_var(const Sort s, const std::string& name) {
	return cvc5_solver.mkVar(s, name.c_str());
}

inline Term make_bitvector_not(const Term& t) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_NOT, {t});
}

inline Term make_bitvector_and(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_AND, {l, r});
}

inline Term make_bitvector_or(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_OR, {l, r});
}

inline Term make_bitvector_nor(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_NOR, {l, r});
}

inline Term make_bitvector_xor(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_XOR, {l, r});
}

inline Term make_bitvector_xnor(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_XNOR, {l, r});
}

inline Term make_bitvector_nand(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_NAND, {l, r});
}

inline Term make_bitvector_add(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_ADD, {l, r});
}

inline Term make_bitvector_sub(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_SUB, {l, r});
}

inline Term make_bitvector_mul(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_MULT, {l, r});
}

inline Term make_bitvector_div(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_UDIV, {l, r});
}

inline Term make_bitvector_mod(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_SMOD, {l, r});
}

inline Term make_bitvector_shl(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_SHL, {l, r});
}

inline Term make_bitvector_shr(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_LSHR, {l, r});
}

} // namespace idni::tau_lang