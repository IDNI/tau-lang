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

struct tau_bool {
	const char* tb_tgf =
	" @use_char_class space, alpha, alnum, bool. "
	"	ws           => space*. "
	"	true 	   	 => 'T'. "
	"	false 	   	 => 'F'. "
	"	bool         => true | false. "
	"	expr         => or | and | neg | bool. "
	"	or           => expr ws '|' ws expr. "
	"	and          => expr ws '&' ws expr. "
	"	neg          => '~' ws expr. "
	" 	chars        => alpha (alnum)*."
	" 	var          => '?' chars."
	" 	matcher      => mor | mand | mneg | mbool. "
	"	mbool         => bool | var. "
	" 	mor           => matcher ws '|' ws matcher. "
	" 	mand          => matcher ws '&' ws matcher. "
	" 	mneg          => '~' ws matcher. "
	" 	rule		   => matcher ws ':-' ws action ws '.'. "
	" 	action	   => '(' ws matcher* ws ')'."
	" 	start        => ws expr ws '.' ws rule (ws rule)*. "
	;

	tau_bool(): g(tgf<char>::from_string(nts, tb_tgf)), p(g) { }

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
	
	bool evaluate_forest(parser<char>::pforest& f) {
		vector<bool> x;  // intermediate evaluations (nested)
		auto cb_enter = [&x, &f, this](const auto& n) {
			//DBG(cout << "entering: `" << n.first.to_std_string() << "` ["<<n.second[0]<<","<<n.second[1]<<"]\n";)
			if (n.first.nt() && n.first.n() == id("bool"))
				switch (terminals_to_str<char>(f, n)[0]) {
					case 'T': x.push_back(true); break;
					case 'F': x.push_back(false); break;
				}
		};
		auto cb_exit = [&x, this](const auto& n, const auto&) {
			//DBG(cout << "exiting: `" << n.first.to_std_string() << "`\n";)
			if (!n.first.nt()) return;
			const auto& l = n.first.n();
			if      (l == id("neg")) x.back() = !x.back();
			else if (l == id("and")) (x[x.size()-2] = x[x.size()-2] & x.back()),
						x.pop_back();
			else if (l == id("or")) (x[x.size()-2] = x[x.size()-2] | x.back()),
						x.pop_back();
		};
		f.traverse(cb_enter, cb_exit);
		return x.size() ? x.back() : false;
	}
};

int main() {
	tau_bool ba;
	string line;
	while (getline(cin, line)) {
		if (line.size() == 0) continue;
		cout << "> " << line << " = ";
		if (!ba.eval(line)) return 1;
	}
}
