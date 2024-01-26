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

#include "../src/bdd_binding.h"

using namespace std;
using namespace idni::tau;

int main(int argc, char** argv) {
	if (argc != 2) return cerr << argv[0] << ": requires 1 argument: nso_rr file\n", 1;

	string formula_file(argv[1]);
	ifstream file(formula_file);
	string content, line;

	while (!file.eof())
		while(std::getline(file, line))
			content += line + "\n";
	file.close();

	bdd_init<Bool>();

	bdd_factory bf;
	tau_factory<bdd_factory, bdd_binding> fb(bf);
	factory_binder<tau_factory<bdd_factory, bdd_binding>,
		tau_ba<bdd_binding>, bdd_binding> fbinder(fb);
	auto nso_rr = make_nso_rr_using_factory<
		factory_binder<tau_factory<bdd_factory, bdd_binding>,
			tau_ba<bdd_binding>, bdd_binding>,
			tau_ba<bdd_binding>, bdd_binding>(
				content, fbinder);

	//if (std::holds_alternative<tau_source_sym>(nso_rr.main->value))
	//	std::cout << "tau_source_sym" << std::endl;

	return 0;
}
