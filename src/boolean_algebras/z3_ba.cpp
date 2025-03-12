// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <memory>

#include "z3_ba.h"
#include "../splitter_types.h"


namespace idni::tau_lang {

using namespace z3;

z3_ba operator&(const z3_ba& x, const z3_ba& y) {
	return std::make_shared<z3::expr>(*x && *y);
}

z3_ba operator|(const z3_ba& x, const z3_ba& y) {
	return std::make_shared<z3::expr>(*x || *y);
}

z3_ba operator^(const z3_ba& x, const z3_ba& y) {
	return std::make_shared<z3::expr>((!*x && *y) || (*x && !*y));
}

z3_ba operator+(const z3_ba& x, const z3_ba& y) {
	return x ^y;
}

z3_ba operator~(const z3_ba& x) {
	return std::make_shared<z3::expr>(!*x);
}

std::strong_ordering operator<=>(const z3_ba& x, const z3_ba& y) {
	return std::addressof(x) <=> std::addressof(y);
}

bool operator==(const z3_ba& x, const z3_ba& y) {
	return *x == *y;
}

bool operator!=(const z3_ba& x, const z3_ba& y) {
	return *x != *y;
}

bool operator==(const z3_ba& x, const bool& b) { return *x == b; }

bool operator==(const bool& b, const z3_ba& x) { return *x == b; }

bool operator!=(const z3_ba& x, const bool& b) { return *x != b; }

bool operator!=(const bool& b, const z3_ba& x) { return *x != b; }

bool is_closed(const z3_ba& /*x*/) { return true; }

z3_ba normalize (const z3_ba& fm) {
	return fm;
}

z3_ba splitter (const z3_ba& /*fm*/, splitter_type& /*st*/) {
	throw std::logic_error("Unsupported");
}

bool is_syntactic_one(const z3_ba& x) {
	return *x == true;
}

bool is_syntactic_zero(const z3_ba& x) {
	return *x == false;
}

} // namespace idni::tau_lang

// Hash for tau_ba using specialization to std::hash
size_t std::hash<idni::tau_lang::z3_ba>::operator()(const idni::tau_lang::z3_ba& x) const noexcept {
	return (*x).hash();
}

std::ostream& operator<<(std::ostream& os, const idni::tau_lang::z3_ba& x) {
	return os << *x;
}