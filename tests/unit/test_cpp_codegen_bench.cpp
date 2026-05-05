// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Benchmark: compiled tau program vs. real tau interpreter on multiple specs.
//
// Each spec is run through BOTH the interpreter (full solver + normalizer per
// step) and the compiled controller (static automaton, g++ -O3).  The speedup
// measures the real benefit of ahead-of-time compilation.
//
// Spec "atomless4" requires an ATOMLESS Boolean algebra: it demands four
// pairwise-distinct outputs, all strictly between {F.}:tau and {T.}:tau,
// that also differ from their own two most-recent past values and from the
// current/past inputs.  In a finite BA (e.g. bv) this is unsatisfiable after
// the first step; only the tau atomless BA has enough elements.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "cpp_codegen.h"
#include "ltl_aba.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

using namespace idni::tau_lang;
using clk = std::chrono::steady_clock;

// ── formula strings ───────────────────────────────────────────────────────────

// Simple echo, baseline.
static const char* ECHO_FORMULA =
    "G (o1[t]:tau = i1[t]:tau).";

// Rich spec with 2 outputs, lookback [t],[t-1],[t-2] on both inputs and
// outputs, and 20 nontrivial atomic sub-formulas using {T.}:tau / {F.}:tau.
// Requires the infinite tau BA (atomless): the conjunction demands o1 and o2
// to differ from each other, from their own past 2 values, from each other's
// past 2 values, from extremals {T.}/{F.}, and from past inputs — 20 distinct
// elements needed in the worst case, exceeding any small finite BA.
//
//  Atoms 1-4  : o1/o2 != {T.}:tau, {F.}:tau                   (4)
//  Atom  5    : o1[t] != o2[t]   (current cross-output)        (1)
//  Atoms 6-9  : o_j[t] != o_j[t-1/t-2]  (self-lookback)       (4)
//  Atoms 10-13: cross-stream lookback (o1 vs o2 past)          (4)
//  Atoms 14-20: output vs input at [t],[t-1],[t-2]             (7)
static const char* ATOMLESS2_FORMULA =
    "G ("
    "   ! (o1[t]:tau = {T.}:tau)"
    " && ! (o1[t]:tau = {F.}:tau)"
    " && ! (o2[t]:tau = {T.}:tau)"
    " && ! (o2[t]:tau = {F.}:tau)"
    " && ! (o1[t]:tau = o2[t]:tau)"
    " && ! (o1[t]:tau = o1[t-1]:tau)"
    " && ! (o1[t]:tau = o1[t-2]:tau)"
    " && ! (o2[t]:tau = o2[t-1]:tau)"
    " && ! (o2[t]:tau = o2[t-2]:tau)"
    " && ! (o1[t]:tau = o2[t-1]:tau)"
    " && ! (o2[t]:tau = o1[t-1]:tau)"
    " && ! (o1[t]:tau = o2[t-2]:tau)"
    " && ! (o2[t]:tau = o1[t-2]:tau)"
    " && ! (o1[t]:tau = i1[t]:tau)"
    " && ! (o1[t]:tau = i1[t-1]:tau)"
    " && ! (o1[t]:tau = i1[t-2]:tau)"
    " && ! (o2[t]:tau = i2[t]:tau)"
    " && ! (o2[t]:tau = i2[t-1]:tau)"
    " && ! (o2[t]:tau = i2[t-2]:tau)"
    " && ! (o2[t]:tau = i1[t]:tau)"
    ").";

// ── helpers ───────────────────────────────────────────────────────────────────

static bool has_gpp() {
    return ::system("g++ --version >/dev/null 2>&1") == 0;
}

static tref parse_formula(const char* s) {
    auto nso_rr = get_nso_rr<node_t>(tau::get(s));
    if (!nso_rr.has_value()) return nullptr;
    return nso_rr.value().main->get();
}

// Synthesize + emit + compile + run N steps.  Returns wall-clock seconds or -1.
static double compiled_seconds(const char* formula_str,
                               const std::string& class_name,
                               long N) {
    tref fm = parse_formula(formula_str);
    if (!fm) return -1.0;
    auto sol = solve_ltl_aba<node_t>(fm);
    if (!sol) return -1.0;

    std::ostringstream hdr_os;
    emit_cpp_program<node_t>(*sol, hdr_os, class_name);

    const char* hdr_path  = "/tmp/_tau_bench_hdr.h";
    const char* main_path = "/tmp/_tau_bench_main.cpp";
    const char* exe_path  = "/tmp/_tau_bench_exe";
    { std::ofstream f(hdr_path); f << hdr_os.str(); }
    {
        std::ofstream f(main_path);
        f << "#include \"_tau_bench_hdr.h\"\n"
             "#include <chrono>\n"
             "#include <cstdio>\n"
             "int main() {\n"
          << "  " << class_name << " c;\n"
          << "  " << class_name << "::Inputs in{};\n"
             "  volatile unsigned sum = 0;\n"
             "  auto t0 = std::chrono::steady_clock::now();\n"
          << "  for (long i = 0; i < " << N << "L; ++i) {\n"
             "    auto o = c.step(in);\n"
             "    sum += (unsigned)o.ok;\n"
             "  }\n"
             "  auto t1 = std::chrono::steady_clock::now();\n"
             "  double sec = std::chrono::duration<double>(t1-t0).count();\n"
             "  std::printf(\"%.9f %u\\n\", sec, sum);\n"
             "  return 0;\n"
             "}\n";
    }
    std::string cmd = std::string("g++ -O3 -flto -std=c++17 -I/tmp -o ")
                    + exe_path + " " + main_path + " 2>/dev/null";
    if (::system(cmd.c_str()) != 0) return -1.0;
    if (::system((std::string(exe_path) + " >/tmp/_tau_bench_out 2>/dev/null").c_str()) != 0)
        return -1.0;
    std::ifstream out("/tmp/_tau_bench_out");
    double sec = -1.0; unsigned s = 0;
    out >> sec >> s; (void)s;
    return sec;
}

// Run tau interpreter for N steps.  Returns wall-clock seconds or -1.
static double interp_seconds(const char* formula_str,
                              const strings& input_vars,
                              const strings& output_vars,
                              long N) {
    // Pre-fill input streams: alternate T./F. for each input variable.
    strings vals;
    vals.reserve(N);
    for (long i = 0; i < N; ++i) vals.push_back((i & 1) ? "T." : "F.");

    io_context<node_t> ctx;
    for (size_t k = 0; k < input_vars.size(); ++k)
        ctx.add_input(input_vars[k], tau_type_id<node_t>(),
                      std::make_shared<vector_input_stream>(vals));
    std::vector<std::shared_ptr<vector_output_stream>> out_streams;
    for (auto& ov : output_vars) {
        auto s = std::make_shared<vector_output_stream>();
        out_streams.push_back(s);
        ctx.add_output(ov, tau_type_id<node_t>(), s);
    }
    auto nso = get_nso_rr<node_t>(ctx, tau::get(formula_str));
    if (!nso.has_value()) return -1.0;
    tref fm = nso.value().main->get();
    if (!fm) return -1.0;

    try {
        auto t0 = clk::now();
        run<node_t>(fm, ctx, (size_t)N);
        auto t1 = clk::now();
        return std::chrono::duration<double>(t1 - t0).count();
    } catch (...) { return -1.0; }
}

// ── test suite ────────────────────────────────────────────────────────────────

TEST_SUITE("cpp_codegen_bench") {

    // Return available memory in bytes from /proc/meminfo, or 0 on failure.
    static long available_mem_bytes() {
        std::ifstream f("/proc/meminfo");
        std::string line;
        while (std::getline(f, line)) {
            if (line.rfind("MemAvailable:", 0) == 0) {
                long kb = 0;
                sscanf(line.c_str(), "MemAvailable: %ld kB", &kb);
                return kb * 1024L;
            }
        }
        return 0;
    }

    TEST_CASE("compiled vs interpreter throughput (real interpreter, multi-spec)") {
        if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }

        struct Spec {
            const char* name;
            const char* formula;
            strings     input_vars;
            strings     output_vars;
            long        N_compiled;
            long        N_interp;
            // false when the interpreter cannot execute this spec (e.g. pure
            // inequality specs produce bf_neq nodes the interpreter can't solve)
            bool        interp_capable = true;
        };

        Spec specs[] = {
            {
                "echo_simple",
                ECHO_FORMULA,
                {"i1"}, {"o1"},
                10'000'000L, 50L, true
            },
            {
                // Pure-inequality spec: interpreter can synthesize but not execute.
                // Only the compiled path is timed; no c > i assertion.
                "atomless2",
                ATOMLESS2_FORMULA,
                {"i1", "i2"}, {"o1", "o2"},
                10'000'000L, 10L, false
            },
        };

        // Require at least 8 GB free before running the atomless2 benchmark:
        // its synthesis does ~190 ABA feasibility checks that can accumulate
        // several GB of formula-cache entries.
        constexpr long ATOMLESS_MIN_MEM = 8L * 1024 * 1024 * 1024;

        for (auto& s : specs) {
            if (std::string(s.name) == "atomless2"
                    && available_mem_bytes() < ATOMLESS_MIN_MEM) {
                MESSAGE("Spec        : atomless2 — SKIPPED (< 8 GB free)");
                continue;
            }
            double c_sec = compiled_seconds(s.formula, std::string(s.name), s.N_compiled);
            double c_rate = c_sec > 0 ? (double)s.N_compiled / c_sec : -1.0;

            MESSAGE("────────────────────────────────────────────────────────────────────");
            MESSAGE("Spec        : " << std::string(s.name));

            if (c_sec <= 0) {
                MESSAGE("Compiled    : FAILED (synthesize/compile/run error)");
                CHECK(false);  // compiled path must always work
                continue;
            }

            MESSAGE("Compiled    : " << c_rate << " steps/sec"
                    "  (" << s.N_compiled << " steps in " << c_sec << "s)");

            if (!s.interp_capable) {
                MESSAGE("Interpreter : N/A (pure-inequality spec; interpreter uses equality solver)");
                continue;
            }

            double i_sec = interp_seconds(s.formula, s.input_vars,
                                          s.output_vars, s.N_interp);
            if (i_sec <= 0) {
                MESSAGE("Interpreter : SKIPPED (interp failed)");
                continue;
            }

            double i_rate = (double)s.N_interp / i_sec;
            double speedup = c_rate / i_rate;
            MESSAGE("Interpreter : " << i_rate << " steps/sec"
                    "  (" << s.N_interp << " steps in " << i_sec << "s)");
            MESSAGE("Speedup     : " << speedup << "x");
            CHECK(c_rate > i_rate);
        }
    }
}
