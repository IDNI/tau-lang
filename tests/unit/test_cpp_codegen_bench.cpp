// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Benchmark: compiled tau program vs. real tau interpreter on multiple specs.
//
// Each spec is run through BOTH the interpreter (full solver + normalizer per
// step) and the compiled artifact compile_spec produces -- the same thing
// `tau compile` gives a user, driven through the interpreter's table step
// provider rather than a standalone step() class (an output whose value
// needs runtime witness solving, such as this file's own o1[t]:tau =
// i1[t]:tau, has no other way to run at all). The speedup measures the real
// benefit of ahead-of-time synthesis, not of skipping the interpreter's I/O
// plumbing.
//
// Spec "atomless4" requires an ATOMLESS Boolean algebra: it demands four
// pairwise-distinct outputs, all strictly between {F.}:tau and {T.}:tau,
// that also differ from their own two most-recent past values and from the
// current/past inputs.  In a finite BA (e.g. bv) this is unsatisfiable after
// the first step; only the tau atomless BA has enough elements.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "cpp_codegen.h"
#include "tau_compile.h"
#include "ltl_aba.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

using namespace idni::tau_lang;
using clk = std::chrono::steady_clock;
namespace fs = std::filesystem;

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

// compiled_seconds' outcome: either a real wall-clock measurement, or the
// reason there is none. `refused` distinguishes compile_spec declining the
// spec outright (a legitimate, checked pipeline outcome -- test_codegen_parity
// treats the same res.ok()==false as parity to verify, not a bug) from the
// artifact actually failing to run once built (a real defect).
struct compiled_result {
    double seconds = -1.0;
    bool refused = false;
    std::string error;
};

// Synthesize + build the real compile_spec artifact, then time it running N
// steps fed from a synthetic stdin tape. `seconds` covers the run only (the
// cmake configure+build is not timed).
//
// The artifact reads each input from stdin (console_prompt_input_stream::get
// does one std::getline per call) and stops cleanly at EOF, so "run N steps"
// means feeding it exactly N*input_vars.size() lines: for each of the N
// steps, one value per input var, in the order the artifact's own run_loop
// asks for them. All input vars share the same alternating T./F. value at a
// given step (order among them does not matter since they agree), matching
// interp_seconds' own fill pattern below.
static compiled_result compiled_seconds(const char* formula_str,
                               const strings& input_vars,
                               const std::string& tag,
                               long N) {
    fs::path bdir = fs::temp_directory_path() / ("_tau_bench_" + tag + ".build");
    std::error_code ec;
    fs::remove_all(bdir, ec);

    auto res = compile_spec<node_t>(formula_str, "", bdir.string());
    if (!res.ok()) return { -1.0, true, res.error };

    fs::path tape = fs::temp_directory_path() / ("_tau_bench_" + tag + ".stdin");
    {
        std::ofstream f(tape);
        for (long t = 0; t < N; ++t) {
            const char* v = (t & 1) ? "T." : "F.";
            for (size_t k = 0; k < input_vars.size(); ++k) f << v << "\n";
        }
    }

    fs::path out = fs::temp_directory_path() / ("_tau_bench_" + tag + ".out");
    std::string cmd = "\"" + res.exe_path + "\" < \"" + tape.string()
                     + "\" > \"" + out.string() + "\" 2>/dev/null";
    auto t0 = clk::now();
    int rc = ::system(cmd.c_str());
    auto t1 = clk::now();

    fs::remove_all(bdir, ec);
    fs::remove(tape, ec);
    fs::remove(out, ec);
    if (rc != 0) return { -1.0, false, "artifact exited with a nonzero status" };
    return { std::chrono::duration<double>(t1 - t0).count(), false, "" };
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

    // Opt-in: mirrors TAU_CODEGEN_RUN_SDK_LINK_TEST/TAU_CODEGEN_RUN_PARITY_TEST
    // -- each spec below drives a real cmake configure+build (compile_spec),
    // and every step of both the compiled artifact and the interpreter is a
    // genuine runtime solve over the atomless tau BA rather than a table
    // lookup, so a handful of steps already cost whole seconds. Stays out of
    // the default ctest budget.
    static bool run_bench() {
        const char* v = std::getenv("TAU_CODEGEN_RUN_BENCH");
        return v && *v && std::string(v) != "0";
    }

    TEST_CASE("compiled vs interpreter throughput (real interpreter, multi-spec)") {
        if (!run_bench()) {
            MESSAGE("TAU_CODEGEN_RUN_BENCH not set; skipping the compile+run "
                "throughput comparison (drives cmake configure+build per spec)");
            return;
        }
        if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }

        struct bench_spec {
            const char* name;
            const char* formula;
            strings     input_vars;
            strings     output_vars;
            long        N_compiled;
            long        N_interp;
        };

        // Both N_compiled and N_interp are steps, not millions: every step
        // is a genuine witness solve (see compiled_seconds/interp_seconds),
        // not a baked table lookup, so counts stay small enough to keep the
        // whole test in the minutes range rather than hours.
        bench_spec specs[] = {
            {
                "echo_simple",
                ECHO_FORMULA,
                {"i1"}, {"o1"},
                300L, 50L
            },
            {
                "atomless2",
                ATOMLESS2_FORMULA,
                {"i1", "i2"}, {"o1", "o2"},
                10L, 3L
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
            auto cres = compiled_seconds(s.formula, s.input_vars,
                                         std::string(s.name), s.N_compiled);
            double c_rate = cres.seconds > 0
                ? (double)s.N_compiled / cres.seconds : -1.0;

            MESSAGE("────────────────────────────────────────────────────────────────────");
            MESSAGE("Spec        : " << std::string(s.name));

            // compile_spec declining the spec outright is a legitimate,
            // checked pipeline outcome (test_codegen_parity treats the same
            // res.ok()==false as parity to verify, not a bug) -- report and
            // move on rather than failing the whole suite over it.
            if (cres.refused) {
                MESSAGE("Compiled    : SKIPPED (compile_spec refused: "
                        << cres.error << ")");
                continue;
            }
            if (cres.seconds <= 0) {
                MESSAGE("Compiled    : FAILED (" << cres.error << ")");
                CHECK(false);  // a built artifact must always run
                continue;
            }

            MESSAGE("Compiled    : " << c_rate << " steps/sec"
                    "  (" << s.N_compiled << " steps in " << cres.seconds << "s)");

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


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}