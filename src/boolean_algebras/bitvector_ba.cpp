// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "bitvector_ba.h"
#include "../splitter_types.h"

using namespace z3;

namespace idni::tau_lang {

static context ctx;

bitvector_ba::bitvector_ba(z3::expr e, const unsigned& size) : size(size), e(e) {}
bitvector_ba::bitvector_ba(int n, const unsigned& sz) : size(sz), e(ctx.bv_val(n, sz)) {}
bitvector_ba::bitvector_ba(unsigned int n, const unsigned& sz) : size(sz), e(ctx.bv_val(n, sz)) {}
bitvector_ba::bitvector_ba(long n, const unsigned& sz) : size(sz), e(ctx.bv_val(n, sz)) {}
bitvector_ba::bitvector_ba(unsigned long n, const unsigned& sz) : size(sz), e(ctx.bv_val(n, sz)) {}

bitvector_ba bitvector_ba::operator&(const bitvector_ba& x) const {
	return bitvector_ba(e & x.e, size);
}

bitvector_ba bitvector_ba::operator|(const bitvector_ba& x) const {
	return bitvector_ba(e | x.e, size);
}

bitvector_ba bitvector_ba::operator^(const bitvector_ba& x) const {
	return bitvector_ba(e ^x.e, size);
}

bitvector_ba bitvector_ba::operator+(const bitvector_ba& x) const {
	return bitvector_ba(e ^ x.e, size);
}

bitvector_ba bitvector_ba::operator~() const {
	return bitvector_ba(!e, size);
}

//auto bitvector_ba::operator<=>(const bitvector_ba& x) const {
//	return std::addressof(e) <=> std::addressof(x.e);
//}

bitvector_ba bitvector_false() {
	// FIXME maybe we should have an abstract false and one
	return bitvector_ba(0, 32);
}

bitvector_ba normalize (const bitvector_ba& fm) {
	return fm;
}

bitvector_ba splitter (const bitvector_ba& /*fm*/, splitter_type& /*st*/) {
	throw std::logic_error("Unsupported");
	//return fm;
}

bool is_syntactic_one(const bitvector_ba& /*fm*/) {
	// TODO implement
	return false;
}

bool is_syntactic_zero(const bitvector_ba& /*fm*/) {
	// TODO implement
	return false;
}

auto operator<=>(const bitvector_ba& x, const bitvector_ba& y) {
	return x.size <=> y.size;
}

bool operator==(const bitvector_ba& x, const bitvector_ba& y) {
	return x.size == y.size;
}

bool operator!=(const bitvector_ba& x, const bitvector_ba& y) {
	return x.size != y.size;
}

bool operator==(const bitvector_ba& /*x*/, const bool& /*b*/) { return false; }

bool operator==(const bool& /*b*/, const bitvector_ba& /*x*/) { return false; }

bool operator!=(const bitvector_ba& /*x*/, const bool& /*b*/) { return false; }

bool operator!=(const bool& /*b*/, const bitvector_ba& /*x*/) { return false; }

bool is_closed(const bitvector_ba& /*x*/) { return true; }

} // namespace idni::tau_lang

// Hash for tau_ba using specialization to std::hash
size_t std::hash<idni::tau_lang::bitvector_ba>::operator()(const idni::tau_lang::bitvector_ba& /*f*/) const noexcept {
	// TODO implement
	return 0;
}

std::ostream& operator<<(std::ostream& os, const idni::tau_lang::bitvector_ba& /*x*/) {
	// TODO implement
	return os << "z3::expr";
}