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
#include <fstream>

#include "defs.h"
#include "cli.h"
#include "repl.h"
#include "normalizer2.h"
#include "parser_instance.h"
#include "bdd_binding.h"

using namespace std;
using namespace idni;
using namespace idni::tau;
using namespace idni::rewriter;

cli::commands tau_commands() {
	cli::commands cmds;
	cmds["help"] = cli::command("help",
		"detailed information about options");
	auto& run = cmds["run"] = cli::command("run",
		"run a tau program");
	run.add_option(cli::option("program", 'p', "@stdin")
		.set_description("program to run"));
	run.add_option(cli::option("evaluate", 'e', "")
		.set_description("program to be evaluated (alternative to -p)"));
	run.add_option(cli::option("help", 'h', false)
		.set_description("detailed information about run options"));
	run.add_option(cli::option("input", 'i', "@null")
		.set_description("program's input"));
	run.add_option(cli::option("output", 'o', "@stdout")
		.set_description("program's output"));
	auto& repl = cmds["repl"] = cli::command("repl", "Tau REPL");
	repl.add_option(cli::option("help", 'h', false)
		.set_description("detailed information about repl options"));
	return cmds;
}

cli::options tau_options() {
	cli::options opts;
	opts["help"] = cli::option("help", 'h', false)
		.set_description("detailed information about options");
	return opts;
}

bool is_stdin(const string& s)  { return s == "@stdin" || s == "-"; }
bool is_stdout(const string& s) { return s == "@stdout"; }
bool is_null(const string& s)   { return s == "@null"; }
int error(const string& s) {
	cerr << s << "\n";
	return 1;
}
// runs tau program or an evaluate string using input and output
int run_tau(const string& program, const string& input, const string& output,
	const string& evaluate = "")
{
	bool eval = evaluate.size();
	if (input == output && !is_null(input))
		return error("input and output cannot be the same");
	if (eval && program.size() && !is_null(program) && !is_stdin(program))
		return error("cannot use both --program and --evaluate");
	if (!eval) {
		if (program == input)
			return error("program and input cannot be the same");
		if (program == output)
			return error("program and output cannot be the same");
	}

	sp_tau_source_node prg_node, in_node, out_node;

	if (eval) prg_node = make_tau_source(evaluate);
	else if (is_null(program)) return error("program cannot be null");
	else if (is_stdin(program)) prg_node = make_tau_source(cin);
	else prg_node = make_tau_source_from_file(program);

	if (is_null(input)) ;
	else if (is_stdin(input)) in_node = make_tau_source(cin);
	else in_node = make_tau_source_from_file(input);

	// read input
	if (in_node) {
		DBG(cout << "input: `" << in_node << "`\n");
	}

	// read program
	if (!prg_node) return error("cannot read program");
	DBG(cout << "program: `" << prg_node << "`\n");

	// run program
	// TODO (MEDIUM) program execution
	out_node = prg_node; // simulate execution by setting out_node to prg

	ostream *out = 0;
	ofstream outf;
	if (is_null(output)) ;
	else if (is_stdout(output)) out = &cout;
	else if (!(outf = ofstream(output)).is_open())
		return error("cannot open output file");
	else out = &outf;

	// write output
	if (out && out_node) {
		DBG(cout << "output: `" << out_node << "`\n");
		*out << out_node;
	}

	return 0;
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

struct repl_evaluator {
	static int eval(const std::string& src) {
		// following are one-word commands which should not require dot
		// ending for convenience and thus these would not be recognized
		// as a tau source so they are handled here
		if (src == "q" || src == "quit" || src == "exit") return 1;
		if (src == "h" || src == "help") return help(), 0;
		if (src == "v" || src == "version") return version(), 0;
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
	// traverses a tree and resolves repl commands. runs them with required
	// elements extracted from their subtree
	// returns non-zero if the repl should quit
	static int traverse(tau_parser::forest_type* f) {
		int quit = 0;
		struct context { // to keep track of where we are in the tree
			bool normalization = false,
				q_nso_rr = false,
				q_wff = false
				;
		} x;
		auto cb_enter = [&x, &f, &quit] (const auto& n) {
			if (!n.first.nt()) return; // skip if terminal
			auto nt = n.first.n(); // get nonterminal id

			// simple commands which do not require any extraction
			if (nt == tau_parser::help) help();
			else if (nt == tau_parser::version) version();
			else if (nt == tau_parser::quit)
				cout << "\nQuit.", quit = 1;
			// tree nodes for which we need to know we are inside
			// them are set to true in context
			else if (nt == tau_parser::normalize)
				x.normalization = true;
			else if (nt == tau_parser::q_nso_rr)
				x.q_nso_rr = true;
			else if (nt == tau_parser::q_wff)
				x.q_wff = true;
		};
		auto cb_exit = [&f, &x] (const auto& n, const auto&) {
			if (!n.first.nt()) return; // skip if terminal
			auto nt = n.first.n(); // get nonterminal id
			if (nt == tau_parser::nso_rr) {
				// if we finished traversing nso_rr
				// and we are in normalization a q_nso_rr
				if (x.normalization && x.q_nso_rr)
					x.normalization = x.q_nso_rr = false,
					normalize_nso_rr(
						extract_tau_source(f, n));
			} else if (nt == tau_parser::wff) {
				// if we finished traversing wff
				// and we are in normalization a q_wff
				if (x.normalization && x.q_wff)
					x.normalization = x.q_wff = false,
					normalize_wff(
						extract_tau_source(f, n));
			}
		};
		f->traverse(cb_enter, cb_exit);
		return quit;
	}
	static void help() {
		cout << "\nCommands:\n"
			<< "  h, help.        print this help\n"
			<< "  v, version.     print version\n"
			<< "  q, quit, exit   exit the repl\n";
	}
	static void version() {
		cout << "\nTau version: " << GIT_DESCRIBED << "\n";
	}
	static const sp_tau_source_node extract_tau_source(
		tau_parser::forest_type* f, const tau_parser::node_type& n)
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
	static void normalize_nso_rr(const sp_tau_source_node& /*n*/)
	{
		// TODO (HIGH) implement nso_rr normalization
	}
	static void normalize_wff(const sp_tau_source_node& /*n*/)
	{
		// TODO (HIGH) implement wff normalization
	}
};

// TODO (MEDIUM) add command to read input file,...
int main(int argc, char** argv) {
	vector<string> args;
	for (int i = 0; i < argc; i++) args.push_back(argv[i]);

	cli cl("tau", args, tau_commands(), "repl", tau_options());
	cl.set_description("Tau language");

	if (cl.process_args() != 0) return cl.status();

	auto opts = cl.get_processed_options();
	auto cmd  = cl.get_processed_command();

	// error if command is invalid
	if (!cmd.ok()) return cl.error("invalid command", true);

	// if --help/-h option is true, print help end exit
	if (cmd.name() == "help" || opts["help"].get<bool>())
		return cl.help(), 0;

	// if cmd's --help/-h option is true, print cmd's help and exit
	if (cmd.get<bool>("help")) return cl.help(cmd), 0;

	// run command
	if (cmd.name() == "run") return run_tau(
		cmd.get<string>("program"),
		cmd.get<string>("input"),
		cmd.get<string>("output"),
		cmd.get<string>("evaluate"));

	// repl command
	if (cmd.name() == "repl") {
		repl<repl_evaluator> re("tau> ", ".tau_history");
		return re.run();
	}

	return 0;
}
