// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "bitvector_ba.h"
#include "../splitter_types.h"

using namespace z3;

namespace idni::tau_lang {

static context ctx;

bitvector_ba::bitvector_ba() : expr(0), size(sizeof(int)) {}
bitvector_ba::bitvector_ba(z3::expr e, const unsigned& size) : expr(e), size(size) {}
bitvector_ba::bitvector_ba(int n) : expr(ctx.bv_val(n, sizeof(int))), size(sizeof(int)) {}
bitvector_ba::bitvector_ba(unsigned n) : expr(ctx.bv_val(n, sizeof(unsigned))), size(sizeof(unsigned)) {}
bitvector_ba::bitvector_ba(int64_t n) : expr(ctx.bv_val(n, sizeof(int64_t))), size(sizeof(int64_t)) {}
bitvector_ba::bitvector_ba(uint64_t n) : expr(ctx.bv_val(n, sizeof(uint64_t))), size(sizeof(uint64_t)) {}

bitvector_ba bitvector_ba::operator&(const bitvector_ba& x) const {
	return bitvector_ba();
}

bitvector_ba bitvector_ba::operator|(const bitvector_ba& x) const {
	return bitvector_ba();
}

bitvector_ba bitvector_ba::operator^(const bitvector_ba& x) const {
	return bitvector_ba();
}

bitvector_ba bitvector_ba::operator+(const bitvector_ba& x) const {
	return bitvector_ba();
}

bitvector_ba bitvector_ba::operator~() const {
	return bitvector_ba(0);
}

//auto bitvector_ba::operator<=>(const bitvector_ba& x) const {
//	return std::addressof(e) <=> std::addressof(x.e);
//}

bitvector_ba bitvector_false() {
	// FIXME maybe we should have an abstract false and one
	return bitvector_ba(0);
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

std::strong_ordering operator<=>(const bitvector_ba& x, const bitvector_ba& y) {
	return std::addressof(x) <=> std::addressof(y);
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

std::ostream& operator<<(std::ostream& os, const idni::tau_lang::bitvector_ba& x) {
	// TODO implement
	return os << "z3::expr" << "[" << x.size << "]";
}