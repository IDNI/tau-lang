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
#include "defs.h" // for GIT_DESCRIBED
#include "repl_evaluator.h"
#include "parser_instance.h"
#include "normalizer2.h"
#include "bdd_binding.h"

namespace idni {

using namespace idni::tau;
using namespace idni::rewriter;

static void version() {
	cout << "\nTau version: " << GIT_DESCRIBED << "\n";
}

// TODO (LOW) write proper help messages
static void help(size_t nt = tau_parser::help_sym) {
	cout << "\n";
	switch (nt) {
	case tau_parser::help_sym: cout
		<< "Commands:\n"
		<< "  h, help.        print this help\n"
		<< "  v, version.     print version\n"
		<< "  q, quit, exit   exit the repl\n";
		break;
	case tau_parser::version_sym: cout
		<< "Command v, version prints out current Tau commit id.\n";
		break;
	case tau_parser::quit_sym: cout
		<< "Command q, quit, exit exits the repl.\n";
		break;
	case tau_parser::output_sym: cout
		<< "Command o, output ...\n";
		break;
	case tau_parser::selection_sym: cout
		<< "Command s, selection ...\n";
		break;
	case tau_parser::instantiate_sym: cout
		<< "Command i, instantiate ...\n";
		break;
	case tau_parser::substitute_sym: cout
		<< "Command s, substitute ...\n";
		break;
	case tau_parser::normalize_sym: cout
		<< "Command n, normalize ...\n";
		break;
	case tau_parser::file_sym: cout
		<< "Command r, read ...\n";
		break;
	}
}

void normalize_nso_rr(const sp_tau_source_node& /*n*/)
{
	// TODO (HIGH) implement nso_rr normalization
}

void normalize_wff(const sp_tau_source_node& /*n*/)
{
	// TODO (HIGH) implement wff normalization
}

const sp_tau_source_node extract_tau_source(tau_parser::forest_type* f,
	const tau_parser::node_type& n)
{
	return make_node_from_forest<
		tau_parser,
		drop_location_t<
			tau_parser::node_type,
			tau_source_sym>,
		tau_parser::node_type,
		tau_source_sym>(
			drop_location<
				tau_parser::node_type,
				tau_source_sym>,
			f, n);
}

bool is_cli_sym(size_t nt) {
	static std::set<size_t> cli_symbols{
		tau_parser::version_sym,
		tau_parser::quit_sym,
		tau_parser::output_sym,
		tau_parser::selection_sym,
		tau_parser::instantiate_sym,
		tau_parser::substitute_sym,
		tau_parser::normalize_sym,
		tau_parser::file_sym
	};
	return cli_symbols.find(nt) != cli_symbols.end();
}

// traverses a tree and resolves repl commands. runs them with required
// elements extracted from their subtree
// returns non-zero if the repl should quit
int traverse(tau_parser::forest_type* f) {
	int quit = 0;
	struct context { // to keep track of where we are in the tree
		bool
			h = false,         // normalize
			n = false,         // help
			q_nso_rr = false,
			q_wff = false
			;
	} x;
	auto cb_enter = [&x, &f, &quit] (const auto& n) {
		if (!n.first.nt()) return; // skip if terminal
		auto nt = n.first.n(); // get nonterminal id
		// simple commands which do not require any extraction
		if (nt == tau_parser::quit) cout << "\nQuit.", quit = 1;
		else if (nt == tau_parser::version) version();
		// tree nodes for which we need to know we are inside
		// them are set to true in context
		else if (nt == tau_parser::help) x.h = true;
		else if (nt == tau_parser::normalize)
			x.n = true;
		else if (nt == tau_parser::q_nso_rr)
			x.q_nso_rr = true;
		else if (nt == tau_parser::q_wff)
			x.q_wff = true;
	};
	auto cb_exit = [&f, &x] (const auto& n, const auto&) {
		if (!n.first.nt()) return; // skip if terminal
		auto nt = n.first.n(); // get nonterminal id
		if (x.n) {
			if (nt == tau_parser::nso_rr && x.q_nso_rr) {
				// if we finished traversing nso_rr
				// and we are in normalization a q_nso_rr
				x.n = x.q_nso_rr = false,
				normalize_nso_rr(
					extract_tau_source(f, n));
			} else if (nt == tau_parser::wff && x.q_wff) {
				// if we finished traversing wff
				// and we are in normalization a q_wff
				x.n = x.q_wff = false,
				normalize_wff(extract_tau_source(f, n));
			}
		} else if (x.h) {
			if (nt == tau_parser::help) help(), x.h = false;
			else if (is_cli_sym(nt))  help(nt), x.h = false;
		}
	};
	f->traverse(cb_enter, cb_exit);
	return quit;
}

#ifdef DEBUG
std::ostream& print_sp_tau_source_node_tree(std::ostream &os,
	tau::sp_tau_source_node n, bool ws = false, size_t l = 0)
{
	bool enter = true;
	auto indent = [&os, &l]() { for (size_t t = 0; t < l; t++) os << "\t";};
	std::visit(tau::overloaded {
		[&os, &ws, &enter, &indent](const tau::tau_source_sym& v) {
			if (!ws && v.nt() && (v.n() == tau_parser::_ ||
					v.n() == tau_parser::__)) {
				enter = false;
				return;
			}
			indent();
			if (v.nt()) os << parser_instance<tau_parser>()
				.name(v.n()) << "(" << v.n() << ")";
			else if (v.is_null()) os << "null";
			else os << v.t();
		}
	}, n->value);
	if (!enter) return os;
	if (n->child.size()) os << " {\n";
	for (auto& c : n->child) print_sp_tau_source_node_tree(os, c, ws, l+1);
	if (n->child.size()) indent(), os << "}";
	return os << "\n";
}
#endif // DEBUG

int repl_evaluator::eval(const std::string& src) {
	// parse input with cli as a starting symbol
	auto f = parser_instance<tau_parser>()
		.parse(src.c_str(), src.size(), {
			.start = tau_parser::cli });
	DBG(check_parser_result<tau_parser>(src,f.get(),tau_parser::cli);)
	// make tau source tree
	auto n = make_node_from_forest<tau_parser,
		rewriter::drop_location_t<tau_parser::node_type,
			tau::tau_source_sym>,
		tau_parser::node_type,
		tau::tau_source_sym>(
			drop_location<tau_parser::node_type,
				tau::tau_source_sym>, f.get());
	// debug the tree
	DBG(print_sp_tau_source_node_tree(std::cerr<<"\n", n) << "\n";)
	// run forest traversal to resolve commands
	if (traverse(f.get())) return 1;
	return 0;
}

} // idni namespace