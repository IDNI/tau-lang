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
#include "../parser/bdd_parser.generated.h"
#include "integration/test_integration_helpers.h"

using namespace std;
using namespace idni::tau;

int main(int argc, char** argv) {
	if (argc != 2) return cerr << argv[0] << ": requires 1 argument: formula file\n", 1;

	string formula_file(argv[1]);
	ifstream file(formula_file);
	string content, line;

	while(!file.eof())
		while(std::getline(file, line))
			content += line + "\n";
	file.close();

	bdd_test_factory bf;
	factory_binder<bdd_test_factory, bdd_test> fb(bf);
	auto formula = make_formula_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(content, fb);

	normalizer<bdd_test>(formula);

	return 0;
}
