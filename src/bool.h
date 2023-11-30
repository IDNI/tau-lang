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
#ifndef __BOOL_H__
#define __BOOL_H__
struct Bool {
	Bool() : b(false) {}
	Bool(bool b) : b(b) {}

	static const Bool& zero() { static Bool b(false); return b; }
	static const Bool& one() { static Bool b(true); return b; }

	Bool operator&(const Bool& x) const;
	Bool operator|(const Bool& x) const;
	Bool operator^(const Bool& x) const;
	Bool operator+(const Bool& x) const;
	Bool operator~() const;
	auto operator<=>(const Bool& x) const = default;

	bool is_zero() const {
		return !b;
	}
	bool is_one() const {
		return b;
	}

	bool b;
};

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

#endif
