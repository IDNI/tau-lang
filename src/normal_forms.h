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

#ifndef __NORMAL_FORMS_H__
#define __NORMAL_FORMS_H__

#include "normalizer2.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

namespace idni::tau {

template<typename...BAs>
nso<BAs...> onf(const nso<BAs...>& nso) {
	// TODO (HIGH) write onf (as executor step, all universal quantyifiers have been removed)
	std::cout << "Not implemented yet." << std::endl;
	return nso;
}

template<typename...BAs>
nso<BAs...> dnf(const nso<BAs...>& nso) {
	// TODO (MEDIUM) write dnf (using rules)
	std::cout << "Not implemented yet." << std::endl;
	return nso;
}

template<typename...BAs>
nso<BAs...> cnf(const nso<BAs...>& nso) {
	// TODO (MEDIUM) write cnf (using rules)
	std::cout << "Not implemented yet." << std::endl;
	return nso;
}

template<typename...BAs>
nso<BAs...> anf(const nso<BAs...>& nso) {
	// TODO (MEDIUM) write anf (using?)
	std::cout << "Not implemented yet." << std::endl;
	return nso;
}

template<typename...BAs>
nso<BAs...> nnf(const nso<BAs...>& nso) {
	// TODO (MEDIUM) write nnf (using?)
	std::cout << "Not implemented yet." << std::endl;
	return nso;
}

template<typename...BAs>
nso<BAs...> pnf(const nso<BAs...>& nso) {
	// TODO (MEDIUM) write pnf (using?)
	std::cout << "Not implemented yet." << std::endl;
	return nso;
}

template<typename...BAs>
nso<BAs...> minterm(const nso<BAs...>& nso) {
	std::cout << "Not implemented yet." << std::endl;
	// TODO (MEDIUM) write minterm (using?)
	return nso;
}

} // namespace idni::tau

#endif // __NORMAL_FORMS_H__