// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// SW-9: guard added -- every sibling helper has one, and a double include
// redefines the inline helpers.
#ifndef __IDNI__TAU__TESTS__TEST_HELPERS_H__
#define __IDNI__TAU__TESTS__TEST_HELPERS_H__

#ifdef DEBUG
// including instead of #include "tau.h" to avoid errors pointing to the generated tau.h
// tau_pack.h brings tau_tree.h and the header of every configured BA, so no BA
// is named here.
#	include "tau_pack.h"
#	include "boolean_algebras/nso_ba.h"
#	include "boolean_algebras/variant_ba.h"
#	include "ba_constants.h"
#	include "base_ba_dispatcher.h"
#	include "api.h"
#else
#	include "tau.h"
#endif // DEBUG

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "testing"

#if !defined(_WIN32) && !defined(__EMSCRIPTEN__)
#include <csignal>
#include <sys/wait.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>

namespace idni::tau_lang {

using strings = std::vector<std::string>;

using node_t = tau_lang::node<bas_pack>;

namespace test_init_detail {
	// The constructor only stores a pointer -- it must never touch the
	// grammar or build a tree node, since static init order across
	// translation units is unordered; main() calls it once that ends.
	struct _tau_init_registrar {
		_tau_init_registrar() { test_tau_init_hook = &tau_init<node_t>; }
	};
	inline _tau_init_registrar _tau_init_registrar_instance;
}

using tau = tree<node_t>;
using tt = tau::traverser;
using bac = ba_constants<node_t>;

inline tau::get_options parse_bf() {
	static tau::get_options opts{ .parse = { .start = tau::bf } };
	return opts;
}

inline tau::get_options parse_wff() {
	static tau::get_options opts{ .parse = { .start = tau::wff } };
	return opts;
}

inline tau::get_options parse_rec_relations() {
	static tau::get_options opts{ .parse = { .start = tau::definitions } };
	return opts;
}

inline std::optional<rr<node_t>> get_bf_nso_rr(const char* rec, const char* sample) {
	auto prr = parse_rec_relations();
	auto rec_r = tau::get(rec, prr);
	// "" is the documented no-extra-relations idiom: definitions is
	// one-or-more, so it always fails to parse; get_rec_relations tolerates
	// a null tref. A non-empty rec that still fails is a real bug in the
	// caller's input, so its report is printed rather than dropped, but the
	// helper stays tolerant (matching pre-refactor behavior) and falls back
	// to an empty rule set either way.
	if (!rec_r.has_value() && rec && *rec) rec_r.print();
	rewriter::rules rrs = get_rec_relations<node_t>(
		rec_r.has_value() ? rec_r.value() : nullptr);
	auto pbf = parse_bf();
	auto main_fm_r = tau::get(sample, pbf);
	if (!main_fm_r.has_value()) return {};
	tref main_fm = main_fm_r.value();
	if (!main_fm) return {};
	return rr<node_t>(rrs, tau::geth(main_fm));
}

inline result<rr<node_t>> get_nso_rr(const char* sample)
{
	// DBG(TAU_LOG_TRACE << "get_nso_rr: " << sample;)
	auto spec_r = tau::get(sample);
	if (!spec_r.has_value()) return {};
	tref spec = spec_r.value();
	if (!spec) return {};
	return get_nso_rr<node_t>(spec);
}

inline bool get_nso_rr_and_check(const char* sample, typename node_t::type nt){
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return false;
	auto x = tt(nso_rr.value().main) | nt;
	return x.has_value();
}

inline bool normalize_and_check(const char* sample,
	typename node_t::type nt, bool expect_fail = false)
{
	using node = node_t;
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return expect_fail;

	DBG(TAU_LOG_DEBUG << "(T) nso_rr: " << to_str<node>(nso_rr.value());)

	auto normalized = normalizer<node>(nso_rr.value());
	if (!normalized.has_value()) return expect_fail;
	tref result = normalized.value();
	if (!result) return expect_fail;

	DBG(TAU_LOG_DEBUG << "(T) Normalized result: " << TAU_LOG_FM(result);)

	return tau::get(result).child_is(nt) != expect_fail;
}

inline bool normalize_and_expect_fail(const char* sample, typename node_t::type nt) {
	return normalize_and_check(sample, nt, true);
}

inline bool matches_to_any_of(const std::string& fm_str, const strings& expected) {
#ifdef DEBUG // report noncanonicality, never gate the result on expected[0]
	if (!expected.empty()) {
		if (fm_str != expected[0]) TAU_LOG_INFO << "expression: "
			<< fm_str << " is not canonical. expected: "
			<< expected[0];
		else TAU_LOG_TRACE << "expression: " << fm_str;
	}
#endif // DEBUG
	for (const auto& e : expected) if (fm_str == e) {
		DBG(TAU_LOG_TRACE << "found in expected: " << fm_str;)
		return true;
	}
	TAU_LOG_ERROR << "not found in expected: " << fm_str;
	return false;
}

inline bool matches_to_str_to_any_of(tref fm, const strings& expected) {
	return matches_to_any_of(tau::get(fm).to_str(), expected);
}

inline bool values_matches_any_of(const strings& values,
	const std::vector<strings>& expected)
{
	if (values.size() != expected.size()) return false;
	for (size_t i = 0; i < values.size(); i++) {
#ifdef DEBUG
		std::stringstream ss; ss << "\nvalues[" << i << "]:\n\t"
			<< values[i] << "\nexpected[" << i << "]:\n";
		for (const auto& e : expected[i]) ss << "\t" << e << "\n";
		TAU_LOG_TRACE << ss.str();
#endif
		if (matches_to_any_of(values[i], expected[i])) {
			DBG(TAU_LOG_TRACE << "found in expected: " << values[i];)
		} else {
			DBG(TAU_LOG_TRACE << "not found in expected: " << values[i];)
			return false;
		}
	}
	return true;
}

/// @brief Conjuncts of @p s, without any leading `always`, sorted.
inline strings sorted_conjuncts(const std::string& s) {
	static const std::string always = "always ", sep = " && ";
	std::string body = s.starts_with(always) ? s.substr(always.size()) : s;
	strings out;
	for (size_t p = 0, q; ; p = q + sep.size()) {
		q = body.find(sep, p);
		if (q == std::string::npos) { out.push_back(body.substr(p)); break; }
		out.push_back(body.substr(p, q - p));
	}
	std::sort(out.begin(), out.end());
	return out;
}

/// @brief `true` when each of @p values carries exactly the conjuncts listed
/// at its position in @p expected, in any order.
inline bool values_match_conjunct_sets(const strings& values,
	const std::vector<strings>& expected)
{
	if (values.size() != expected.size()) return false;
	for (size_t i = 0; i < values.size(); i++) {
		strings want = expected[i];
		std::sort(want.begin(), want.end());
		if (sorted_conjuncts(values[i]) != want) {
			DBG(TAU_LOG_TRACE << "conjuncts differ at " << i << ": "
				<< values[i];)
			return false;
		}
	}
	return true;
}

inline strings split_str(const std::string& s, const std::string& sep) {
	strings parts;
	size_t pos = 0, next;
	while ((next = s.find(sep, pos)) != std::string::npos) {
		parts.push_back(s.substr(pos, next - pos));
		pos = next + sep.size();
	}
	parts.push_back(s.substr(pos));
	return parts;
}

// Sorts s's operands, split on sep, and rejoins them with sep. A no-op
// when s does not contain sep.
inline std::string sort_join(const std::string& s, const std::string& sep) {
	strings parts = split_str(s, sep);
	if (parts.size() < 2) return s;
	std::sort(parts.begin(), parts.end());
	std::string result;
	for (size_t i = 0; i < parts.size(); i++) {
		if (i) result += sep;
		result += parts[i];
	}
	return result;
}

// Sorts fm_str's top-level " && " conjuncts, and within each conjunct the
// space-joined operands of its bf "and" (printed with no operator) on
// either side of its " = ", so two orderings of the same conjunction
// compare equal regardless of which of its commutative operators reordered.
// A leading "always " applies to the whole formula, not to the first
// conjunct, so it is set aside and restored afterwards. A no-op when
// fm_str has no " && ".
inline std::string canonical_conjunct_order(const std::string& fm_str) {
	static const std::string always_prefix = "always ";
	static const std::string and_sep = " && ";
	static const std::string eq_sep = " = ";
	const bool has_always = fm_str.starts_with(always_prefix);
	const std::string body = has_always
		? fm_str.substr(always_prefix.size()) : fm_str;
	strings conjuncts = split_str(body, and_sep);
	if (conjuncts.size() < 2) return fm_str;
	for (auto& conjunct : conjuncts) {
		strings sides = split_str(conjunct, eq_sep);
		for (auto& side : sides) side = sort_join(side, " ");
		std::string rebuilt;
		for (size_t i = 0; i < sides.size(); i++) {
			if (i) rebuilt += eq_sep;
			rebuilt += sides[i];
		}
		conjunct = rebuilt;
	}
	std::sort(conjuncts.begin(), conjuncts.end());
	std::string result = has_always ? always_prefix : "";
	for (size_t i = 0; i < conjuncts.size(); i++) {
		if (i) result += and_sep;
		result += conjuncts[i];
	}
	return result;
}

inline bool matches_to_any_of_canonical_conjuncts(const std::string& fm_str,
	const strings& expected)
{
	const std::string canon = canonical_conjunct_order(fm_str);
	for (const auto& e : expected)
		if (canon == canonical_conjunct_order(e)) return true;
	return false;
}

inline bool values_matches_any_of_canonical_conjuncts(const strings& values,
	const std::vector<strings>& expected)
{
	if (values.size() != expected.size()) return false;
	for (size_t i = 0; i < values.size(); i++)
		if (!matches_to_any_of_canonical_conjuncts(values[i], expected[i]))
			return false;
	return true;
}

// ── tree comparison modulo AND/OR commutativity ─────────────────────────────
//
// canonical_conjunct_order (above) sorts substrings of the *printed* form,
// so it only sees a top-level " && " split and nothing where bf_and prints
// as juxtaposition or "&" (tau_tree_printers.tmpl.h). The helpers below
// compare the *tree* instead, so they reach commutativity nested at any
// depth and printed forms with no delimiter at all.

// True for the associative-commutative Boolean connectives only. Every
// other operator -- crucially bf_eq, whose operand orientation is meant to
// be canonical -- is left untouched by the functions below. bf_eq gets its
// own, narrower exception: see is_bare_variable_operand.
inline bool is_and_or_nt(size_t nt) {
	return nt == tau::wff_and || nt == tau::wff_or
		|| nt == tau::bf_and  || nt == tau::bf_or;
}

// True for a bf_eq operand that is a plain variable -- neither an
// uninterpreted constant, nor an input or output stream variable.
// simplify_using_equality_term_comp (src/heuristics/simplify_using_equality.tmpl.h)
// gives every other category a canonical priority order; only this one
// falls through to tau::subtree_less, a content-hash tie-break.
// simplify_using_equality_dnf::term_comp (normal_forms_bf.tmpl.h)
// implements the same order on the DNF path. op is a bf-typed bf_eq child,
// as build_bf_eq requires.
inline bool is_bare_variable_operand(tref op) {
	const auto& o = tau::get(op);
	if (o.children_size() != 1) return false;
	const auto& v = o[0];
	if (!v.is(tau::variable)) return false;
	if (v.is_input_variable() || v.is_output_variable()) return false;
	if (v.children_size() && v[0].is(tau::uconst_name)) return false;
	return true;
}

// The single-child nonterminal ("wff" or "bf") that wraps every operand of
// nt, and sits between one wff_and/bf_and (etc.) node and the next link of
// the same chain. build_wff_and/build_bf_and (tau_tree_builders.tmpl.h)
// build exactly `get(wrapper, get(nt, l, r))`.
inline size_t and_or_wrapper_nt(size_t nt) {
	return (nt == tau::wff_and || nt == tau::wff_or) ? tau::wff : tau::bf;
}

// Structural key for t. At an and/or node, operands sort at every nesting
// level, since their order carries no meaning. A bf_eq between two bare
// variables (is_bare_variable_operand) sorts its pair the same way, since
// that orientation is only a content-hash tie-break. Every other node keeps
// its child order, since there the orientation is canonical and a
// violation of it must still fail. Post-order, so every child key is ready
// when its parent is visited.
inline std::string canonicalize_tref(tref t) {
	if (!t) return "-";
	std::unordered_map<tref, std::string> key;
	std::unordered_map<tref, strings> chain;
	auto join = [](const strings& v, const char* open, const char* close) {
		std::string r = open;
		for (size_t i = 0; i < v.size(); i++) {
			if (i) r += ",";
			r += v[i];
		}
		return r + close;
	};
	auto visit = [&](tref n) {
		const auto& nd = tau::get(n);
		const size_t nt = nd.value.nt;
		const size_t cs = nd.children_size();
		const std::string tag = std::to_string(nt);
		if (is_and_or_nt(nt)) {
			const size_t wrapper = and_or_wrapper_nt(nt);
			strings ops;
			for (size_t i = 0; i < cs; i++) {
				tref c = nd[i].get();
				const auto& cd = tau::get(c);
				// operands sit under a single-child
				// wrapper; a nested chain of the same
				// connective splices in
				if (cd.value.nt == wrapper
					&& cd.children_size() == 1)
						c = cd.only_child();
				if (auto it = chain.find(c); it != chain.end())
					ops.insert(ops.end(),
						it->second.begin(),
						it->second.end());
				else ops.push_back(key[c]);
			}
			std::sort(ops.begin(), ops.end());
			chain[n] = ops;
			key[n] = tag + join(ops, "[", "]");
			return true;
		}
		if (nt == tau::bf_eq && cs == 2
			&& is_bare_variable_operand(nd[0].get())
			&& is_bare_variable_operand(nd[1].get()))
		{
			strings p{ key[nd[0].get()], key[nd[1].get()] };
			std::sort(p.begin(), p.end());
			key[n] = tag + join(p, "(", ")");
			return true;
		}
		if (cs == 0) { key[n] = tag + ":" + nd.to_str(); return true; }
		strings ch;
		for (size_t i = 0; i < cs; i++) ch.push_back(key[nd[i].get()]);
		key[n] = tag + join(ch, "(", ")");
		return true;
	};
	post_order<node_t>(t).search(visit);
	return key[t];
}

inline bool matches_tree_mod_and_or(tref result, tref expected) {
	return canonicalize_tref(result) == canonicalize_tref(expected);
}

// Parses expected_bf with the bf grammar (as get_bf_nso_rr's sample
// argument does) and compares under matches_tree_mod_and_or.
inline bool matches_bf_mod_and_or(tref result, const char* expected_bf) {
	auto expected_r = tau::get(expected_bf, parse_bf());
	if (!expected_r.has_value()) {
		expected_r.print();
		return false;
	}
	tref expected = expected_r.value();
	if (!expected) {
		TAU_LOG_ERROR << "expected bf does not parse: "
			<< expected_bf;
		return false;
	}
	return matches_tree_mod_and_or(result, expected);
}

// Parses expected_wff with the wff grammar and compares under
// matches_tree_mod_and_or.
inline bool matches_wff_mod_and_or(tref result, const char* expected_wff) {
	auto expected_r = tau::get(expected_wff, parse_wff());
	if (!expected_r.has_value()) {
		expected_r.print();
		return false;
	}
	tref expected = expected_r.value();
	if (!expected) {
		TAU_LOG_ERROR << "expected wff does not parse: "
			<< expected_wff;
		return false;
	}
	return matches_tree_mod_and_or(result, expected);
}

// True if result matches any one of several structurally distinct expected
// shapes, each up to AND/OR commutativity -- for a formula an algorithm may
// legitimately return in more than one equivalent tree shape, only one of
// which the hash order of a given build actually prints.
inline bool matches_wff_mod_and_or_any_of(tref result,
	const strings& expected_wffs)
{
	for (const auto& e : expected_wffs)
		if (matches_wff_mod_and_or(result, e.c_str())) return true;
	return false;
}

// Parses actual_wff (a printed result, e.g. an output stream value) with the
// wff grammar and compares against expected_wff under matches_wff_mod_and_or.
// False if actual_wff fails to parse.
inline bool matches_wff_str_mod_and_or(const std::string& actual_wff,
	const char* expected_wff)
{
	auto actual_r = tau::get(actual_wff.c_str(), parse_wff());
	if (!actual_r.has_value()) {
		actual_r.print();
		return false;
	}
	tref actual = actual_r.value();
	return actual && matches_wff_mod_and_or(actual, expected_wff);
}

inline bool normalize_and_check(const char* sample, const strings& expected) {
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return false;

	auto normalized = normalizer<node_t>(nso_rr.value());
	if (!normalized.has_value()) return false;
	tref result = normalized.value();
	if (!result) return false;

	return matches_to_str_to_any_of(result, expected);
}

inline bool normalize_and_check(const char* sample, const std::string& expected) {
	return normalize_and_check(sample, strings{ expected });
}

inline bool matches_bf_mod_and_or_any_of(tref fm, const strings& expected) {
	for (const auto& e : expected)
		if (matches_bf_mod_and_or(fm, e.c_str())) return true;
	return false;
}

// values_matches_any_of, but comparing trees modulo AND/OR order rather
// than printed strings. The per-position candidate list is kept: it also
// holds genuinely different results, not just reorderings. The values
// arrive printed, so each is parsed back before comparing.
inline bool values_match_mod_and_or(const strings& values,
	const std::vector<strings>& expected)
{
	if (values.size() != expected.size()) return false;
	for (size_t i = 0; i < values.size(); i++) {
		auto v_r = tau::get(values[i].c_str(), parse_wff());
		if (!v_r.has_value()) {
			v_r.print();
			return false;
		}
		tref v = v_r.value();
		if (!v) {
			TAU_LOG_ERROR << "value does not parse: "
				<< values[i];
			return false;
		}
		if (!matches_wff_mod_and_or_any_of(v, expected[i])) return false;
	}
	return true;
}

inline bool normalize_and_check_mod_and_or(const char* sample,
	const char* expected_wff)
{
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return false;
	auto result = normalizer<node_t>(nso_rr.value());
	if (!result.has_value()) return false;
	return matches_wff_mod_and_or(result.value(), expected_wff);
}

// ── portable subprocess, scratch-dir and host-compiler helpers ──────────────
//
// The codegen, blasting and revision suites drive a child process, a scratch
// directory or a host C++ compiler. These wrap the platform differences once,
// so a suite stays free of /tmp, popen and mkdtemp -- none of which MinGW or
// MSVC has.

/// Every directory tau_test_tmp made, removed when the test process exits.
/// Only a tau_test_ name is ever removed.
struct tau_test_tmp_registry {
	std::vector<std::filesystem::path> dirs;
	~tau_test_tmp_registry() {
		std::error_code ec;
		for (const auto& dir : dirs)
			if (dir.filename().string().starts_with("tau_test_"))
				std::filesystem::remove_all(dir, ec);
	}
};

inline tau_test_tmp_registry& tau_test_tmp_dirs() {
	static tau_test_tmp_registry registry;
	return registry;
}

/// A fresh scratch directory under the platform temp directory. The suffix is
/// random, not the pid, so two checkouts sharing a temp directory never
/// collide; a collision retries instead of reusing a live directory.
inline std::filesystem::path tau_test_tmp(const std::string& name) {
	namespace fs = std::filesystem;
	static std::mt19937_64 rng{ std::random_device{}() };
	static unsigned long long counter = 0;
	std::error_code ec;
	fs::path base = fs::temp_directory_path(ec);
	if (ec) base = ".";
	for (int attempt = 0; attempt < 64; ++attempt) {
		const fs::path dir = base / ("tau_test_" + name + "_"
			+ std::to_string(rng()) + "_"
			+ std::to_string(counter++));
		if (fs::create_directory(dir, ec)) {
			tau_test_tmp_dirs().dirs.push_back(dir);
			return dir;
		}
	}
	// Only reachable when the temp directory itself is unusable; every
	// caller then fails on its first write instead of sharing a directory.
	return base / ("tau_test_" + name + "_unavailable");
}

/// The platform's executable suffix, for a path built by hand.
inline const char* tau_test_exe_suffix() {
#ifdef _WIN32
	return ".exe";
#else
	return "";
#endif
}

/// This test binary's own path, baked in by CMake as TAU_TEST_EXE_PATH. A
/// suite that re-executes itself as a worker needs it; empty when the
/// definition is absent, so the suite can fall back to running inline.
inline std::string tau_test_exe_path() {
#ifdef TAU_TEST_EXE_PATH
	return TAU_TEST_EXE_PATH;
#else
	return "";
#endif
}

/// The exit code a std::system() status carries: the raw value on Windows,
/// WEXITSTATUS elsewhere (a signal death maps to the shell's 128+signal).
inline int exit_code_of(int status) {
#if defined(_WIN32) || defined(__EMSCRIPTEN__)
	return status;
#else
	return WIFEXITED(status) ? WEXITSTATUS(status) : 128 + WTERMSIG(status);
#endif
}

/// Outcome of tau_test_run: 0 only when the child exited zero, -1 otherwise.
/// `err` holds the separately captured stderr, and the rendered spawn report
/// when the child never produced a code (not found, killed, non-zero exit).
struct tau_test_run_result {
	std::string out;
	std::string err;
	int exit_code = -1;
	bool timed_out = false;
};

/// Run `argv` directly, with `stdin_data` on its stdin, capturing stdout and
/// stderr. spawn_capture decides the exit code itself, so only the
/// zero/non-zero distinction leaves this helper.
inline tau_test_run_result tau_test_run(const std::vector<std::string>& argv,
	const std::string& stdin_data = "", int timeout_sec = 0)
{
	tau_test_run_result r;
	namespace fs = std::filesystem;
	const fs::path dir = tau_test_tmp("run");
	const fs::path in_path = dir / "stdin";
	const fs::path err_path = dir / "stderr";
	{
		std::ofstream in(in_path, std::ios::binary);
		in.write(stdin_data.data(),
			static_cast<std::streamsize>(stdin_data.size()));
	}
	spawn_options opts;
	opts.stdin_path = in_path.string();
	opts.stderr_path = err_path.string();
	auto res = spawn_capture(argv, timeout_sec,
		[](int c) { return c == 0; }, opts);
	std::error_code ec;
	if (fs::exists(err_path, ec)) {
		std::ifstream err(err_path, std::ios::binary);
		std::ostringstream buf;
		buf << err.rdbuf();
		r.err = buf.str();
	}
#ifdef _WIN32
	r.err.erase(std::remove(r.err.begin(), r.err.end(), '\r'),
		r.err.end());
#endif
	fs::remove_all(dir, ec);
	if (res.has_value()) {
		r.out = std::move(res.value());
		r.exit_code = 0;
		return r;
	}
	// A failed run still produced output: the report carries the captured
	// stdout under label::value. Keeping it is what lets a caller tell two
	// failed runs apart instead of comparing two empty bodies.
	for (const auto& n : res.report().nodes())
		if (auto v = node_attr_text(res.report(), n, label::value)) {
			r.out = *v;
			break;
		}
	if (auto code_v = report_attr_value(res.report(), label::exit_code))
		r.exit_code = static_cast<int>(*code_v);
	std::ostringstream why;
	res.print(why);
	r.err += why.str();
	r.timed_out = report_has_attr(res.report(), label::timeout);
	return r;
}

/// True when `cxx` can be spawned at all. Deliberately not "exits 0": MSVC's
/// cl rejects an unknown flag, and a compiler that runs is what a test needs.
/// Only a not-found report means it is absent.
inline bool tau_test_cxx_available(const std::string& cxx) {
	auto probe = spawn_capture({ cxx, "--version" }, 30,
		[](int) { return true; });
	return probe.has_value()
		|| !report_has_code(probe.report(), code::not_found);
}

/// The host C++ compiler a codegen suite compiles its driver with: the
/// TAU_TEST_CXX override, else the first of g++, clang++ and cl that runs.
inline const std::string& tau_test_cxx() {
	static const std::string cxx = []() -> std::string {
		if (const char* v = std::getenv("TAU_TEST_CXX"); v && *v)
			return v;
		for (const char* name : { "g++", "clang++", "cl" })
			if (tau_test_cxx_available(name)) return name;
		return "g++";
	}();
	return cxx;
}

/// True when the compiler's flags have MSVC's shape (/O2, /Fe:), not gcc's.
inline bool tau_test_cxx_is_msvc(const std::string& cxx) {
	std::string base = cxx;
	if (auto sep = base.find_last_of("/\\"); sep != std::string::npos)
		base = base.substr(sep + 1);
	if (base.size() > 4
		&& (base.ends_with(".exe") || base.ends_with(".EXE")))
		base.resize(base.size() - 4);
	return base == "cl";
}

struct tau_test_compile_result { bool ok = false; std::string out; };

/// Compile and link `sources` into the executable `exe` (the caller supplies
/// the platform suffix) with tau_test_cxx(). The flag table covers g++,
/// clang++ and cl; `ndebug` adds the customer build's -DNDEBUG.
inline tau_test_compile_result tau_test_compile(const std::string& exe,
	const std::vector<std::string>& sources, const std::string& include_dir,
	int std_year = 17, bool ndebug = false, bool lto = false)
{
	const std::string& cxx = tau_test_cxx();
	std::vector<std::string> argv{ cxx };
	if (tau_test_cxx_is_msvc(cxx)) {
		argv.push_back("/nologo");
		argv.push_back("/O2");
		if (lto) argv.push_back("/GL");
		// cl has no /std:c++23; c++latest is its C++23 mode.
		argv.push_back(std_year >= 23
			? std::string("/std:c++latest")
			: "/std:c++" + std::to_string(std_year));
		argv.push_back("/EHsc");
		if (ndebug) argv.push_back("/DNDEBUG");
		if (!include_dir.empty()) argv.push_back("/I" + include_dir);
		argv.push_back("/Fe:" + exe);
		// cl writes .obj beside the source unless /Fo names a directory;
		// the executable's own scratch dir keeps the source tree clean.
		std::filesystem::path exe_dir =
			std::filesystem::path(exe).parent_path();
		if (!exe_dir.empty()) {
			std::string fo = exe_dir.string();
			fo += std::filesystem::path::preferred_separator;
			argv.push_back("/Fo" + fo);
		}
	} else {
		argv.push_back(lto ? "-O3" : "-O2");
		if (lto) argv.push_back("-flto");
		argv.push_back("-std=c++" + std::to_string(std_year));
		if (ndebug) argv.push_back("-DNDEBUG");
		if (!include_dir.empty()) argv.push_back("-I" + include_dir);
		argv.push_back("-o");
		argv.push_back(exe);
	}
	for (const auto& s : sources) argv.push_back(s);
	auto run = tau_test_run(argv);
	return { run.exit_code == 0, run.out + run.err };
}

// Probes spawn_capture() itself (the primitive ltlsynt/ltlfilt use) rather
// than checking the platform, so this tracks real subprocess capability
// wherever it changes.
inline bool can_spawn_subprocess() {
	static const bool available = [] {
		// spawn_capture decides the exit code itself, so a successful
		// probe is a value and every failure is an error. Windows has no
		// `true`, so probe a command every install ships.
#ifdef _WIN32
		return spawn_capture({"cmd", "/c", "exit", "0"}).has_value();
#else
		return spawn_capture({"true"}).has_value();
#endif
	}();
	return available;
}

#if !defined(_WIN32) && !defined(__EMSCRIPTEN__)
// A live assert(false) raises SIGABRT, which doctest's own handler would
// turn into a normal exit; run op() in a fork with the default disposition
// restored, and _exit before any teardown or atexit runs twice.
template<typename F>
inline bool dies_by_sigabrt(F op) {
	pid_t pid = fork();
	if (pid == 0) {
		std::signal(SIGABRT, SIG_DFL);
		op();
		_exit(0);
	}
	int status = 0;
	waitpid(pid, &status, 0);
	return WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT;
}
#endif

} // namespace idni::tau_lang

// ── per-test GC listener ──────────────────────────────────────────────────────
// Optional per-test GC listener. Keep disabled by default: several test cases
// retain trefs across doctest callbacks, so automatic GC before each TEST_CASE
// can invalidate cached trees. Heavy suites should call do_gc() explicitly at
// safe points instead.
#if !defined(IDNI_TAU_TESTS_GC_LISTENER_REGISTERED) && defined(DOCTEST_LIBRARY_INCLUDED) && defined(TAU_ENABLE_TEST_GC_LISTENER)
#define IDNI_TAU_TESTS_GC_LISTENER_REGISTERED

#include <algorithm>
#include <unordered_set>

struct tau_test_gc_listener : doctest::IReporter {
	using tau_ = idni::tau_lang::tau;
	tau_test_gc_listener(const doctest::ContextOptions&) {}
	void report_query(const doctest::QueryData&) override {}
	void test_run_start() override {}
	void test_run_end(const doctest::TestRunStats&) override {}
	void test_case_start(const doctest::TestCaseData&) override {
		std::unordered_set<tref> keep;
		tau_::gc(keep);
	}
	void test_case_reenter(const doctest::TestCaseData&) override {}
	void test_case_end(const doctest::CurrentTestCaseStats&) override {}
	void test_case_exception(const doctest::TestCaseException&) override {}
	void subcase_start(const doctest::SubcaseSignature&) override {}
	void subcase_end() override {}
	void log_assert(const doctest::AssertData&) override {}
	void log_message(const doctest::MessageData&) override {}
	void test_case_skipped(const doctest::TestCaseData&) override {}
};
REGISTER_LISTENER("tau_gc", 1, tau_test_gc_listener);

#endif // IDNI_TAU_TESTS_GC_LISTENER_REGISTERED

#endif // __IDNI__TAU__TESTS__TEST_HELPERS_H__
