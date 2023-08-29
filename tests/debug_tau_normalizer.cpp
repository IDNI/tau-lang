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

#include <iostream>

#include "../src/bdd_handle.h"
#include "../src/bool.h"
#include "../src/normalizer2.h"
#include "../parser/bdd_parser.generated.h"

using namespace std;
using namespace idni::tau;

namespace idni::tau::debug_normalization {

using bdd_source_sym = idni::lit<char, char>;
using bdd_source_node = tau_source_node;
using sp_bdd_source_node = sp_tau_source_node;
using bdd_sym = tau_sym<hbdd<Bool>>;
using bdd_node = tau_node<hbdd<Bool>>;
using sp_bdd_node = sp_tau_node<hbdd<Bool>>;

struct bdd_var_binder {

	sp_tau_node<hbdd<Bool>> bind(const sp_tau_node<hbdd<Bool>>& n) const {
		auto bn = make_string_with_skip<
			decltype(tau_node_terminal_extractor<hbdd<Bool>>),
			decltype(not_whitespace_predicate<hbdd<Bool>>), 
			sp_tau_node<hbdd<Bool>>>(tau_node_terminal_extractor<hbdd<Bool>>, 
				not_whitespace_predicate<hbdd<Bool>>, n);
		if (auto s = bs.find(bn); s != bs.end()) {
			tau_sym<hbdd<Bool>> ts = s->second;
			return make_node<tau_sym<hbdd<Bool>>>(ts, {});
		}
		auto idx = bs.emplace(bn, ++index).first->second;
		return make_node<tau_sym<hbdd<Bool>>>(bdd_handle<Bool>::bit(true, idx), {});
	}

	static size_t index;
	static std::map<std::string, size_t> bs;
};

} // namespace idni::debug_normalization

int main(int argc, char** argv) {
	if (argc != 2) return cerr << argv[0] << ": requires 1 argument: formula file\n", 1;
	string formula_file(argv[1]);
	// TODO read formula from file, normalize it and print the output
	return 0;
}

