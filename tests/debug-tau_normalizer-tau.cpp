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
#include <sstream>
#include <string>

#include "../src/defs.h"
#include "../src/bdd_handle.h"
#include "../src/bool.h"
#include "../src/normalizer2.h"
#include "../src/tau.h"

#include "integration/test_integration_helpers-tau.h"

using namespace std;
using namespace idni::tau;

int main(int argc, char** argv) {
	if (argc != 2) return cerr << argv[0] << ": requires 1 argument: nso_rr file\n", 1;

	string formula_file(argv[1]);
	ifstream file(formula_file);
	string content, line;

	while(!file.eof())
		while(std::getline(file, line))
			content += line + "\n";
	file.close();

	bdd_test_factory<bdd_test> bf;
	tau_factory<bdd_test_factory_t<bdd_test>, bdd_test> fb(bf);
	factory_binder<tau_factory<bdd_test_factory_t<bdd_test>, bdd_test>, tau<bdd_test>, bdd_test> fbinder(fb);
	nso_rr<tau<bdd_test>, bdd_test> nso_rr = make_nso_rr_using_factory<
			factory_binder<tau_factory<bdd_test_factory_t<bdd_test>, bdd_test>,tau<bdd_test>, bdd_test>,
			tau<bdd_test>, bdd_test>(content, fbinder);

	normalizer<tau<bdd_test>, bdd_test>(nso_rr);

	return 0;
}
