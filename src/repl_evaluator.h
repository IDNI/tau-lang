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

#ifndef __REPL_EVALUATOR_H__
#define __REPL_EVALUATOR_H__

#include <string>
#include <vector>
#include "bdd_binding.h"

namespace idni::tau {

struct repl_evaluator {
	int eval(const std::string& src);
	using outputs_mem = std::vector<std::variant<
		nso<tau_ba<bdd_binding>, bdd_binding>,
		rr<nso<tau_ba<bdd_binding>, bdd_binding>>>>;
private:
	outputs_mem m;
};

} // idni::tau namespace
#endif // __REPL_EVALUATOR_H__
