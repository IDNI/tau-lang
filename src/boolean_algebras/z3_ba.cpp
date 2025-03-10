// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "z3_ba.h"
#include "../splitter_types.h"

using namespace z3;

namespace idni::tau_lang {

static context ctx;

// TODO (MEDIUM) remove in the future
z3_ba::z3_ba() : expr(z3_bitvector{ctx.bv_val(0, sizeof(int)), sizeof(int)}) {}
// TODO (MEDIUM) this should be converted into sort typed factory methods in the future
z3_ba::z3_ba(int n) : expr(z3_bitvector{ctx.bv_val(n, sizeof(int)), sizeof(int)}) {}
z3_ba::z3_ba(unsigned n) : expr(z3_bitvector{ctx.bv_val(n, sizeof(unsigned)), sizeof(unsigned)}) {}
z3_ba::z3_ba(int64_t n) : expr(z3_bitvector{ctx.bv_val(n, sizeof(int64_t)), sizeof(int64_t)}) {}
z3_ba::z3_ba(uint64_t n) : expr(z3_bitvector{ctx.bv_val(n, sizeof(uint64_t)), sizeof(uint64_t)}) {}

z3_ba z3_ba::operator&(const z3_ba& x) const {
	return z3_ba();
}

z3_ba z3_ba::operator|(const z3_ba& x) const {
	return z3_ba();
}

z3_ba z3_ba::operator^(const z3_ba& x) const {
	return z3_ba();
}

z3_ba z3_ba::operator+(const z3_ba& x) const {
	return z3_ba();
}

z3_ba z3_ba::operator~() const {
	return z3_ba(0);
}

//auto z3_ba::operator<=>(const z3_ba& x) const {
//	return std::addressof(e) <=> std::addressof(x.e);
//}

z3_ba bitvector_false() {
	// FIXME maybe we should have an abstract false and one
	return z3_ba(0);
}

z3_ba normalize (const z3_ba& fm) {
	return fm;
}

z3_ba splitter (const z3_ba& /*fm*/, splitter_type& /*st*/) {
	throw std::logic_error("Unsupported");
	//return fm;
}

bool is_syntactic_one(const z3_ba& /*fm*/) {
	// TODO implement
	return false;
}

bool is_syntactic_zero(const z3_ba& /*fm*/) {
	// TODO implement
	return false;
}

std::strong_ordering operator<=>(const z3_ba& x, const z3_ba& y) {
	return std::addressof(x) <=> std::addressof(y);
}

bool operator==(const z3_ba& x, const z3_ba& y) {
	return x.expr == y.expr;
}

bool operator!=(const z3_ba& x, const z3_ba& y) {
	return !(x.expr != y.expr);
}

bool operator==(const z3_ba& /*x*/, const bool& /*b*/) { return false; }

bool operator==(const bool& /*b*/, const z3_ba& /*x*/) { return false; }

bool operator!=(const z3_ba& /*x*/, const bool& /*b*/) { return false; }

bool operator!=(const bool& /*b*/, const z3_ba& /*x*/) { return false; }

bool is_closed(const z3_ba& /*x*/) { return true; }

} // namespace idni::tau_lang

// Hash for tau_ba using specialization to std::hash
size_t std::hash<idni::tau_lang::z3_ba>::operator()(const idni::tau_lang::z3_ba& /*f*/) const noexcept {
	// TODO implement
	return 0;
}

std::ostream& operator<<(std::ostream& os, const idni::tau_lang::z3_ba& x) {
	// TODO implement
	return os << "z3::expr" << "[" << "TODO" << "]";
}