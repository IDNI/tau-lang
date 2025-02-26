// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <memory>

#include "queries.h"
#include "splitter_types.h"

namespace idni::tau_lang {

using parse_forest = idni::parser<char, char>::pforest;
using parse_result = idni::parser<char, char>::result;

bitvector_ba::bitvector_ba(expr e, const unsigned& size) : size(size), e(e) {}

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

template <typename...BAs>
std::optional<tau<BAs...>> bitvector_ba_factory<BAs...>::parse(
	const std::string& /*src*/)
{
	// TODO implement
	return {};
}

template <typename...BAs>
tau<BAs...> bitvector_ba_factory<BAs...>::binding(const tau<BAs...>& /*sn*/) {
	// TODO implement
	return nullptr;
}

template <typename...BAs>
std::variant<BAs...> bitvector_ba_factory<BAs...>::splitter_one() const {
	throw std::logic_error("Unsupported");
}

auto normalize (const bitvector_ba& fm) {
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


} // namespace idni::tau_lang

size_t std::hash<idni::tau_lang::bitvector_ba>::operator()(const idni::tau_lang::bitvector_ba& /*f*/) const noexcept {
	// TODO implement
	return 0;
}

std::ostream& operator<<(std::ostream& os, const idni::tau_lang::bitvector_ba& /*x*/) {
	// TODO implement
	return os << "z3::expr";
}