// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.
#include <ostream>
#include "bool.h"

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

std::ostream& operator<<(std::ostream& os, const Bool& b) {
	return os << (b.b ? 1 : 0);
}
