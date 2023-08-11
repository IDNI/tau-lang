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

using bdd_sym = idni::lit<char, char>;
using sp_bdd_node = sp_node<bdd_sym>;

sp_bdd_node make_bdd_from_string(const std::string source) {
	using parse_lit = idni::lit<char, char>;
	using parse_location = std::array<size_t, 2UL>;
	using parse_symbol = std::pair<parse_lit, parse_location>;

	return make_node_from_string<bdd_parser, decltype(drop_location<parse_symbol, bdd_sym>),
			parse_symbol, bdd_sym>(drop_location<parse_symbol, bdd_sym>, 
			source);
}

int main(int argc, char** argv) {
	if (argc != 2) return cerr << argv[0] << ": requires 1 argument: formula file\n", 1;
	string formula_file(argv[1]);
	// TODO read formula from file, normalize it and print the output
	return 0;
}

// TODO:HIGH add bdd factory for bindings
