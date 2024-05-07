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

#ifndef __REPL_CHECKS_H__
#define __REPL_CHECKS_H__

#include "normalizer.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace idni::tau {

template<typename... BAs>
bool check_no_rec_relations_present(const nso<BAs...>& form) {
	if (auto check = find_top(form, is_non_terminal<tau_parser::offsets, BAs...>); check)
		return false;
	return true;
}

} // namespace idni::tau
#endif // __REPL_CHECKS_H__