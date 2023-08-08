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

using namespace std;
using namespace idni::tau;

namespace idni::tau {

/*struct hbdd_bool_applier {

	hbdd_bool_applier(const callback_applier<hbdd<Bool>>& cb) : cb(cb) {}

	sp_tau_node<hbdd<Bool>> operator()(const sp_tau_node<hbdd<Bool>>& n) {
		if (!is_test(n)) return n;
		return cb(make_hbdd(n));
	}

	callback_applier<hbdd<Bool>>& cb;
private:
	bool is_test(const sp_tau_node<hbdd<Bool>>& n) {
		return n->get_type() == tau_parser::bf_test;
	}

	hbdd<Bool> make_hbdd(const sp_tau_node<hbdd<Bool>>& n) {
		static map<string, hbdd<Bool>> cache;
		auto not_ws = [](const sp_tau_node<BAs...>& n) { return !(n->value.index() == 0 && get<0>(n->value).nt() && get<0>(n->value).n() == tau_parser::ws); };	
		auto bn = make_string_with_skip<decltype(not_ws), hbdd<Bool>>(n, not_ws);
		// TODO get cached hbdd or create a new one.
		return cb
	}
};
} // namespace idni::tau 

// we specialize the following method to inject the creation of bdd_handles.
/*template<>
sp_tau_node<hbdd<Bool>> tau_apply(const rule<tau_sym<hbdd<Bool>>>& r, const sp_tau_node<hbdd<Bool>>& n) {
	// TODO we could also apply only once
	return post_order_traverser(map_transformer(callback_applier<hbdd<Bool>>()))(apply(r,n));
}*/

}

int main(int argc, char** argv) {
	if (argc != 2) return cerr << argv[0] << ": requires 1 argument: formula file\n", 1;
	string formula_file(argv[1]);
	// TODO read formula from file, normalize it and print the output
	return 0;
}