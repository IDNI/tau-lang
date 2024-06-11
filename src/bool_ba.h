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
#ifndef __BOOL_BA_H__
#define __BOOL_BA_H__

struct Bool {
	Bool();
	Bool(bool b);

	static const Bool& zero();
	static const Bool& one();

	Bool operator&(const Bool& x) const;
	Bool operator|(const Bool& x) const;
	Bool operator^(const Bool& x) const;
	Bool operator+(const Bool& x) const;
	Bool operator~() const;
	auto operator<=>(const Bool& x) const = default;

	bool is_zero() const;
	bool is_one() const;

	bool b;
};

Bool normalize (const Bool& b);

std::ostream& operator<<(std::ostream& os, const Bool& b);

#endif // __BOOL_BA_H__
