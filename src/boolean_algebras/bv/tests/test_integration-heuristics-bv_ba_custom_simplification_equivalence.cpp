// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv/bv_ba.h"

#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <chrono>

// ---------------------------------------------------------------------------
// Differential correctness harness for bv_ba_custom_simplification.
//
// For a corpus term T this proves T == S for every input, where
// S = bv_ba_custom_simplification(T), by asking cvc5 whether "T != S" is
// unsat (bv_formula_sat_status on build_bf_neq(T, S)). A known bug can
// SIGABRT/SIGSEGV the process, so each term runs in a forked child; the
// parent classifies the outcome from the child's exit status/signal, with
// a wall-clock watchdog so a hang cannot stall the whole run. No outcome is
// ever silently dropped: every term ends up EQUIVALENT / NOT_EQUIVALENT /
// CRASHED / ORACLE_UNAVAILABLE.
// ---------------------------------------------------------------------------

tref parse_bf(const std::string& sample) {
	static tree<node_t>::get_options opts{ .parse = { .start = tree<node_t>::bf }};
	auto src = tree<node_t>::get(sample, opts);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}

enum class diff_outcome { equivalent, not_equivalent, crashed, oracle_unavailable };

struct corpus_case {
	std::string term;
	std::string shape;             // short label for reporting
	std::vector<std::string> nonblock_ops; // nonblock operators used, if any
};

struct diff_result {
	diff_outcome outcome;
	std::string detail;
};

// Runs in the forked child: computes S and proves T == S via cvc5, writes
// one line "OUTCOME|detail" to out_path. Never throws past this function --
// a cvc5 API exception is an oracle failure, not a crash of the target.
static void run_child(const std::string& sample, const std::filesystem::path& out_path) {
	std::ofstream out(out_path);
	tref src = parse_bf(sample);
	if (!src) { out << "ORACLE_UNAVAILABLE|parse failed for: " << sample << "\n"; return; }

	// Isolated from the oracle's own try/catch below: an exception escaping
	// bv_ba_custom_simplification itself (e.g. a std::bad_alloc some builds
	// throw instead of aborting) is the same UB family as a raw crash, not
	// an oracle failure -- classify it as CRASHED.
	tref simplified = nullptr;
	try {
		simplified = bv_ba_custom_simplification<node_t>(src);
	} catch (const std::exception& e) {
		out << "CRASHED|exception escaped bv_ba_custom_simplification: " << e.what() << " for: " << sample << "\n";
		return;
	} catch (...) {
		out << "CRASHED|unknown exception escaped bv_ba_custom_simplification for: " << sample << "\n";
		return;
	}
	if (!simplified) { out << "ORACLE_UNAVAILABLE|simplification returned null for: " << sample << "\n"; return; }

	try {
		// bv_ba_custom_simplification normally preserves the outer "bf"
		// wrapper (see bv_ba_custom_simplification.tmpl.h:22-34), but wrap
		// defensively so build_bf_neq's precondition always holds.
		tref t_bf = tree<node_t>::get(src).is(tree<node_t>::bf) ? src : tree<node_t>::get(tree<node_t>::bf, src);
		tref s_bf = tree<node_t>::get(simplified).is(tree<node_t>::bf) ? simplified : tree<node_t>::get(tree<node_t>::bf, simplified);

		std::string t_str = tree<node_t>::get(t_bf).to_str();
		std::string s_str = tree<node_t>::get(s_bf).to_str();

		if (tree<node_t>::get(t_bf) == tree<node_t>::get(s_bf)) {
			out << "EQUIVALENT|identical term: " << t_str << "\n";
			return;
		}
		if (tree<node_t>::get(t_bf).get_ba_type() != tree<node_t>::get(s_bf).get_ba_type()) {
			out << "ORACLE_UNAVAILABLE|bv type/width mismatch T=" << t_str << " S=" << s_str << "\n";
			return;
		}

		tref query = build_bf_neq<node_t>(t_bf, s_bf);
		auto status = bv_formula_sat_status<node_t>(query);
		if (!status) {
			out << "ORACLE_UNAVAILABLE|cvc5 translation failed T=" << t_str << " S=" << s_str << "\n";
			return;
		}
		switch (*status) {
			case bv_sat_status::unsat:
				out << "EQUIVALENT|T=" << t_str << " S=" << s_str << "\n"; break;
			case bv_sat_status::sat:
				out << "NOT_EQUIVALENT|T=" << t_str << " S=" << s_str << "\n"; break;
			case bv_sat_status::unknown:
				out << "ORACLE_UNAVAILABLE|cvc5 returned unknown T=" << t_str << " S=" << s_str << "\n"; break;
		}
	} catch (const std::exception& e) {
		out << "ORACLE_UNAVAILABLE|exception: " << e.what() << " for: " << sample << "\n";
	} catch (...) {
		out << "ORACLE_UNAVAILABLE|unknown exception for: " << sample << "\n";
	}
}

// Runs sample in a forked child so a crash in bv_ba_custom_simplification
// cannot take down the rest of the corpus. Polls with a wall-clock watchdog
// so a hang is killed and reported instead of stalling the run.
static diff_result run_isolated(const std::string& sample, int index) {
	auto out_path = std::filesystem::temp_directory_path()
		/ ("tau_bv_diff_" + std::to_string(getpid()) + "_" + std::to_string(index) + ".txt");
	std::filesystem::remove(out_path);

	// doctest installs its own SIGABRT/SIGSEGV/etc handlers in the parent;
	// fork() inherits them, so without resetting them a child crash gets
	// caught by doctest's handler (which flushes stdio, replaying the
	// inherited buffer) instead of just terminating the child. Flush now
	// and restore default dispositions in the child.
	std::fflush(stdout);
	std::fflush(stderr);
	pid_t pid = fork();
	if (pid < 0) return { diff_outcome::oracle_unavailable, "fork() failed" };
	if (pid == 0) {
		for (int sig : { SIGABRT, SIGSEGV, SIGFPE, SIGILL, SIGBUS, SIGTRAP })
			std::signal(sig, SIG_DFL);
		run_child(sample, out_path);
		std::_Exit(0);
	}

	constexpr auto per_term_timeout = std::chrono::seconds(10);
	auto start = std::chrono::steady_clock::now();
	int status = 0;
	bool timed_out = false;
	for (;;) {
		pid_t r = waitpid(pid, &status, WNOHANG);
		if (r == pid) break;
		if (std::chrono::steady_clock::now() - start > per_term_timeout) {
			kill(pid, SIGKILL);
			waitpid(pid, &status, 0);
			timed_out = true;
			break;
		}
		usleep(2000);
	}

	if (timed_out) {
		std::filesystem::remove(out_path);
		return { diff_outcome::crashed, "timed out after 10s (killed) for: " + sample };
	}
	if (WIFSIGNALED(status)) {
		int sig = WTERMSIG(status);
		std::filesystem::remove(out_path);
		return { diff_outcome::crashed,
			std::string("terminated by signal ") + strsignal(sig) + " for: " + sample };
	}
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
		std::filesystem::remove(out_path);
		return { diff_outcome::crashed, "abnormal exit status for: " + sample };
	}

	std::ifstream in(out_path);
	std::string line;
	std::getline(in, line);
	std::filesystem::remove(out_path);
	auto sep = line.find('|');
	if (sep == std::string::npos) return { diff_outcome::oracle_unavailable, "no result written for: " + sample };
	std::string tag = line.substr(0, sep);
	std::string detail = line.substr(sep + 1);
	if (tag == "EQUIVALENT") return { diff_outcome::equivalent, detail };
	if (tag == "NOT_EQUIVALENT") return { diff_outcome::not_equivalent, detail };
	if (tag == "CRASHED") return { diff_outcome::crashed, detail };
	return { diff_outcome::oracle_unavailable, detail };
}

// ---------------------------------------------------------------------------
// Corpus generation.
// ---------------------------------------------------------------------------

static const std::vector<std::string> arith_ops   = { "+", "-", "*", "/" };
// binary non-block operators from bv_ba_custom_simplification.tmpl.h:343-345
static const std::vector<std::string> nonblock_ops = { "<<", ">>", "%", "&", "|", "^", "!&", "!|", "!^" };

static std::string leaf(int i, int width, bool as_const) {
	if (as_const) return "{" + std::to_string(1 + (i % 3)) + "}:bv[" + std::to_string(width) + "]";
	return "v" + std::to_string(1 + (i % 4)) + ":bv[" + std::to_string(width) + "]";
}

static std::vector<corpus_case> build_corpus() {
	std::vector<corpus_case> cases;

	// G0 -- exact reproductions of the confirmed crash/non-crash facts
	// (width 8, plain bf variables instead of io_vars/temporal wrapping).
	struct g0_case { const char* term; const char* shape; const char* op; };
	static const g0_case g0[] = {
		{ "((i1:bv[8]+i2:bv[8])<<{1}:bv[8])+i3:bv[8]", "G0-crash-shl", "<<" },
		{ "((i1:bv[8]+i2:bv[8])<<{1}:bv[8])+((i3:bv[8]+i4:bv[8])<<{1}:bv[8])", "G0-crash-shl-double", "<<" },
		{ "((i1:bv[8]*i2:bv[8])%{3}:bv[8])*i4:bv[8]", "G0-crash-mod", "%" },
		{ "((i1:bv[8]+i2:bv[8])&i3:bv[8])+i4:bv[8]", "G0-nontrigger-and", "&" },
		{ "((i1:bv[8]+i2:bv[8])|i3:bv[8])+i4:bv[8]", "G0-nontrigger-or", "|" },
		{ "((i1:bv[8]+i2:bv[8])^i3:bv[8])+i4:bv[8]", "G0-nontrigger-xor", "^" },
	};
	for (const auto& c : g0) cases.push_back({ c.term, c.shape, { c.op } });

	for (int width : { 2, 4 }) {
		// G1 -- depth 2: a lone arithmetic block as one operand of a
		// non-block operator, no enclosing block.
		for (const auto& arith : arith_ops) {
			for (const auto& nb : nonblock_ops) {
				std::string block = "(" + leaf(0, width, false) + arith + leaf(1, width, false) + ")";
				std::string other = leaf(2, width, true);
				cases.push_back({ "(" + block + ")" + nb + other, "G1-block-nb-leaf(" + arith + nb + ")", { nb } });
				cases.push_back({ other + nb + "(" + block + ")", "G1-leaf-nb-block(" + arith + nb + ")", { nb } });
			}
		}

		// G2 -- depth 3: the confirmed crash shape itself, generalized
		// over every arithmetic operator (in matching outer/inner pairs)
		// crossed with every non-block operator, both operand orders.
		for (const auto& arith : arith_ops) {
			for (const auto& nb : nonblock_ops) {
				std::string inner = "(" + leaf(0, width, false) + arith + leaf(1, width, true) + ")";
				std::string seam = "(" + inner + nb + leaf(2, width, false) + ")";
				std::string tail = leaf(3, width, true);
				cases.push_back({ seam + arith + tail, "G2-arith(nb(arith,leaf),leaf)(" + arith + nb + ")", { nb } });
				cases.push_back({ tail + arith + seam, "G2-arith(leaf,nb(arith,leaf))(" + arith + nb + ")", { nb } });
			}
		}
	}

	// G3 -- depth 4, double seam (mirrors the second confirmed crash
	// example) for the 3 confirmed crashers plus the 3 confirmed-safe
	// bitwise ops, width 4, across all 4 outer arithmetic operators.
	for (const auto& nb : { std::string("<<"), std::string(">>"), std::string("%"),
			std::string("&"), std::string("|"), std::string("^") }) {
		for (const auto& arith : arith_ops) {
			std::string left = "((v1:bv[4]" + arith + "v2:bv[4])" + nb + "v3:bv[4])";
			std::string right = "((v4:bv[4]" + arith + "{2}:bv[4])" + nb + "v3:bv[4])";
			cases.push_back({ left + arith + right, "G3-double-seam(" + arith + nb + ")", { nb, nb } });
		}
	}

	// G4 -- unary complement (bf_neg, postfix ') feeding an arithmetic
	// block inside the crash shape; complement is unary so it never adds
	// a sibling leaf frame -- worth checking it behaves differently.
	for (const auto& nb : nonblock_ops) {
		std::string inner = "(v1:bv[4]'+v2:bv[4])";
		std::string seam = "(" + inner + nb + "v3:bv[4])";
		cases.push_back({ seam + "+v4:bv[4]", "G4-neg-operand(" + nb + ")", { nb } });
	}

	// G5 -- pure arithmetic, no non-block operator at all: control group,
	// expected EQUIVALENT and no crash regardless of the bug above.
	for (int width : { 2, 4 }) {
		for (const auto& a1 : arith_ops) {
			for (const auto& a2 : arith_ops) {
				std::string term = "((" + leaf(0, width, false) + a1 + leaf(1, width, true) + ")"
					+ a2 + leaf(2, width, false) + ")" + a1 + leaf(3, width, true);
				cases.push_back({ term, "G5-pure-arith(" + a1 + a2 + ")", {} });
			}
		}
	}

	return cases;
}

// ---------------------------------------------------------------------------
// Harness test case.
// ---------------------------------------------------------------------------

// Confirmed crash triggers per the investigation this harness follows up on;
// used only to flag CRASHED results as "expected" vs "new finding" in the
// summary -- every crash is still reported either way.
static bool is_confirmed_crasher(const std::vector<std::string>& ops) {
	for (const auto& op : ops) if (op == "<<" || op == ">>" || op == "%") return true;
	return false;
}

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}

	TEST_CASE("logging") {
		logging::trace();
	}
}

TEST_SUITE("bv_ba_custom_simplification differential correctness") {

	TEST_CASE("corpus is equivalent to its input under cvc5") {
		auto corpus = build_corpus();
		size_t n_equiv = 0, n_not_equiv = 0, n_crash = 0, n_unavailable = 0;
		std::vector<std::string> not_equiv_report, crash_report, unavailable_report;
		std::vector<std::string> unexpected_crash_report;

		int idx = 0;
		for (const auto& c : corpus) {
			auto r = run_isolated(c.term, idx++);
			switch (r.outcome) {
				case diff_outcome::equivalent:
					++n_equiv;
					break;
				case diff_outcome::not_equivalent:
					++n_not_equiv;
					not_equiv_report.push_back("[" + c.shape + "] " + c.term + " :: " + r.detail);
					break;
				case diff_outcome::crashed: {
					++n_crash;
					std::string line = "[" + c.shape + "] " + c.term + " :: " + r.detail;
					crash_report.push_back(line);
					if (!is_confirmed_crasher(c.nonblock_ops)) unexpected_crash_report.push_back(line);
					break;
				}
				case diff_outcome::oracle_unavailable:
					++n_unavailable;
					unavailable_report.push_back("[" + c.shape + "] " + c.term + " :: " + r.detail);
					break;
			}
		}

		std::cout << "\n=== bv_ba_custom_simplification differential summary ===\n"
			<< "corpus size:        " << corpus.size() << "\n"
			<< "EQUIVALENT:         " << n_equiv << "\n"
			<< "NOT_EQUIVALENT:     " << n_not_equiv << "\n"
			<< "CRASHED:            " << n_crash << "\n"
			<< "ORACLE_UNAVAILABLE: " << n_unavailable << "\n";

		std::cout << "--- NOT_EQUIVALENT (correctness bug) ---\n";
		for (const auto& l : not_equiv_report) std::cout << l << "\n";
		std::cout << "--- CRASHED (all) ---\n";
		for (const auto& l : crash_report) std::cout << l << "\n";
		std::cout << "--- CRASHED on operators NOT in the confirmed {<<,>>,%} set (new finding) ---\n";
		for (const auto& l : unexpected_crash_report) std::cout << l << "\n";
		std::cout << "--- ORACLE_UNAVAILABLE (oracle could not decide) ---\n";
		for (const auto& l : unavailable_report) std::cout << l << "\n";
		std::cout << "=== end summary ===\n\n";

		// The central question: bv_ba_custom_simplification must never
		// return a non-equivalent formula. A crash is a known, separate
		// bug (not fixed here) and is reported above, not asserted on.
		CHECK(n_not_equiv == 0);
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
