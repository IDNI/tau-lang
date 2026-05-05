// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md
//
// tau_codegen: standalone CLI that reads a tau spec from a file (or stdin)
// and emits a C++17 program header to stdout or a file.
//
// Usage:
//   tau_codegen [spec_file] [-o output.h] [--class Name]
//
// Usage: `tau_codegen spec.tau -o program.h`;
// then links the resulting header in their C++ project.  The generated
// program is pure-switch-case for propositional specs; for specs with
// data atoms (qlt, bv, etc.) it emits runtime solver call stubs linked
// against libtau at build time.

#ifdef DEBUG
#include "boolean_algebras/bv_ba.h"
#include "boolean_algebras/sbf_ba.h"
#include "boolean_algebras/qint.h"
#include "boolean_algebras/qlt.h"
#include "boolean_algebras/nlang_ba.h"
#include "boolean_algebras/nso_ba.h"
#include "boolean_algebras/tau_ba.h"
#include "boolean_algebras/variant_ba.h"
#include "base_ba_dispatcher.h"
#include "api.h"
#include "ltl_aba.h"
#include "cpp_codegen.h"
#else
#include "tau.h"
#include "ltl_aba.h"
#include "cpp_codegen.h"
#endif

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace idni::tau_lang;

using node_t = node<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>, qint, qlt, nlang_ba, bv, sbf_ba, hsb>;

static void usage() {
	std::cerr <<
	    "Usage: tau_codegen [spec_file | -] [-o output.h] [--class Name]\n"
	    "                  [--open stream1[,stream2,...]]\n"
	    "\n"
	    "Reads a tau LTL specification, invokes the synthesis pipeline, and\n"
	    "emits a C++17 program class that realizes the spec.\n"
	    "\n"
	    "If spec_file is - (or omitted), reads from stdin.\n"
	    "If -o is omitted, writes to stdout.\n"
	    "The default class name is TauProgram; override with --class.\n"
	    "\n"
	    "--open: comma-separated list of output stream names to expose as\n"
	    "        oracle-resolved at runtime (per declare_open_design.md §14).\n"
	    "        V1: registration API only; per-step dispatch is V2.\n"
	    "\n"
	    "The generated header compiles with: g++ -O3 -flto -std=c++17\n";
}

int main(int argc, char** argv) {
	std::string spec_path = "-";
	std::string out_path;
	std::string class_name = "TauProgram";
	std::vector<std::string> open_streams;

	for (int i = 1; i < argc; ++i) {
		std::string a = argv[i];
		if (a == "-h" || a == "--help") { usage(); return 0; }
		if (a == "-o") {
			if (i + 1 >= argc) { usage(); return 2; }
			out_path = argv[++i];
		} else if (a == "--class") {
			if (i + 1 >= argc) { usage(); return 2; }
			class_name = argv[++i];
		} else if (a == "--open") {
			if (i + 1 >= argc) { usage(); return 2; }
			std::string list = argv[++i];
			size_t start = 0;
			while (start < list.size()) {
				size_t comma = list.find(',', start);
				size_t end = (comma == std::string::npos)
					? list.size() : comma;
				std::string name = list.substr(start, end - start);
				// Trim whitespace.
				auto ns = name.find_first_not_of(" \t");
				auto ne = name.find_last_not_of(" \t");
				if (ns != std::string::npos)
					open_streams.push_back(
						name.substr(ns, ne - ns + 1));
				start = (comma == std::string::npos)
					? list.size() : comma + 1;
			}
		} else if (a.size() && a[0] == '-' && a != "-") {
			std::cerr << "unknown option: " << a << "\n"; usage(); return 2;
		} else {
			spec_path = a;
		}
	}

	std::string src;
	if (spec_path == "-") {
		std::ostringstream oss; oss << std::cin.rdbuf(); src = oss.str();
	} else {
		std::ifstream ifs(spec_path);
		if (!ifs) { std::cerr << "cannot open " << spec_path << "\n"; return 1; }
		std::ostringstream oss; oss << ifs.rdbuf(); src = oss.str();
	}
	if (src.empty()) { std::cerr << "empty input\n"; return 1; }

	// Parse the spec as a wff (well-formed formula).  We use the api<node_t>
	// helper which already knows how to drive the parser with simplification.
	using tau_api = api<node_t>;
	tref fm = tau_api::get_formula(src);
	if (!fm) {
		std::cerr << "failed to parse spec (is it a well-formed LTL formula?)\n";
		return 1;
	}

	// Drive the LTL(ABA) synthesis pipeline.  solve_ltl_aba returns
	// std::nullopt on propositional unrealizability; on success we receive
	// the HoaAutomaton strategy.
	auto sol = solve_ltl_aba<node_t>(fm);
	if (!sol) {
		std::cerr << "spec is UNREALIZABLE\n";
		return 3;
	}

	std::ostringstream ss;
	if (open_streams.empty()) {
		emit_cpp_program<node_t>(*sol, ss, class_name);
	} else {
		emit_cpp_program_open<node_t>(*sol, open_streams, ss, class_name);
	}

	if (out_path.empty()) {
		std::cout << ss.str();
	} else {
		std::ofstream ofs(out_path);
		if (!ofs) { std::cerr << "cannot write " << out_path << "\n"; return 1; }
		ofs << ss.str();
	}
	std::cerr << "[tau_codegen] emitted " << class_name << " ("
	          << sol->aut.num_states << " states, "
	          << sol->aut.aps.size() << " atomic propositions)\n";
	return 0;
}
