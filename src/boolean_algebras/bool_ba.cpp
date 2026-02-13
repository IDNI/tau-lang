// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <ostream>
#include "bool_ba.h"

namespace idni::tau_lang {

Bool::Bool() : b(false) {}
Bool::Bool(bool b) : b(b) {}

const Bool& Bool::zero() { static Bool b(false); return b; }
const Bool& Bool::one() { static Bool b(true); return b; }

Bool Bool::operator&(const Bool& x) const {
	return (this->b == false) ? zero() : x;
}
Bool Bool::operator|(const Bool& x) const {
	return (this->b == true) ? one() : x;
}
Bool Bool::operator^(const Bool& x) const {
	return (this->b == true) ? ~x : x;
}
Bool Bool::operator+(const Bool& x) const {
	return (this->b == true) ? ~x : x;
}
Bool Bool::operator~() const {
	return (this->b == true) ? zero() : one();
}
bool Bool::is_zero() const {
	return !b;
}
bool Bool::is_one() const {
	return b;
}

Bool ba_normalize(const Bool &b) {
	return b;
}

bool is_ba_syntactic_one(const Bool& b) {
	return b.is_one();
}

bool is_ba_syntactic_zero(const Bool& b) {
	return b.is_zero();
}

bool is_ba_closed(const Bool&) {
	return true;
}

std::ostream& operator<<(std::ostream& os, const Bool& b) {
	return os << (b.b ? 1 : 0);
}

} // namespace idni::tau_lang
