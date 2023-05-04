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
#include <cassert>
#include <string.h>
#include "parser.h"
using namespace std;
using namespace idni;

struct tau_basic_arithmetic {
	const char* ba_tgf =
	"	@use_char_class space, digit. "
	"	ws           => space*. "
	"	integer      => digit+. "
	"	expr1        => addsub | expr2. "
	"	expr2        => muldiv | factor. "
	"	addsub       => add | sub & ~add. "
	"	muldiv       => mul | div & ~mul. "
	"	add          => expr1 ws '+' ws expr2. "
	"	sub          => expr1 ws '-' ws expr2. "
	"	mul          => expr2 ws '*' ws factor. "
	"	div          => expr2 ws '/' ws factor. "
	"	factor       => pos | neg | integer | '(' ws expr1 ws ')'. "
	"       pos          => '+' ws factor. "
	"	neg          => '-' ws factor. "
	"	start        => ws expr1 ws. "
	;
	tau_basic_arithmetic() :
		g(tgf<char>::from_string(nts, ba_tgf)), p(g) { }
	bool eval(const string& s) {
		auto f = p.parse(s.c_str(), s.size());
		if (!f || !p.found()) {
			return cerr << "\ninput text does not seem to be an "
				"expression statement", false; }
		cout << evaluate_forest(*f) << endl;
		return true;
	}
private:
	nonterminals<char> nts{};
	grammar<char> g;
	parser<char> p;
	size_t id(const string& s) { return nts.get(s); }
	int_t evaluate_forest(parser<char>::pforest& f) {
		vector<int_t> x;  // intermediate evaluations (nested)
		auto cb_enter = [&x, &f, this](const auto& n) {
			//DBG(cout << "entering: `" << n.first.to_std_string() << "` ["<<n.second[0]<<","<<n.second[1]<<"]\n";)
			if (n.first.nt() && n.first.n() == id("integer"))
				x.push_back(terminals_to_int<char>(f, n));
		};
		auto cb_exit = [&x, this](const auto& n, const auto&) {
			//DBG(cout << "exiting: `" << n.first.to_std_string() << "`\n";)
			if (!n.first.nt()) return;
			const auto& l = n.first.n();
			if      (l == id("neg")) x.back() = -x.back();
			else if (l == id("add")) (x[x.size()-2] += x.back()),
						x.pop_back();
			else if (l == id("sub")) (x[x.size()-2] -= x.back()),
						x.pop_back();
			else if (l == id("mul")) (x[x.size()-2] *= x.back()),
						x.pop_back();
			else if (l == id("div")) (x[x.size()-2] /= x.back()),
						x.pop_back();
		};
		f.traverse(cb_enter, cb_exit);
		return x.size() ? x.back() : 0;
	}
};

int main() {
	tau_basic_arithmetic ba;
	string line;
	while (getline(cin, line)) {
		if (line.size() == 0) continue;
		cout << "> " << line << " = ";
		if (!ba.eval(line)) return 1;
	}
}
