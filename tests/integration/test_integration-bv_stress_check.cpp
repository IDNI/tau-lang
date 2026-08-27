// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * Bitvector stress check.
 *
 * C++ port of the `load_test_bitvectors.py` load tester. That script drives a
 * Tau process running the specification `always u[t] = i0[t]` and, step after
 * step, feeds an ever more complex bitvector rule through the `i0` stream while
 * answering the `i1`..`i16` prompts with random bitvector values. Here the same
 * rules are generated and fed to `run()` through an `io_context` of vector
 * streams, so the whole load test runs in-process with no REPL involved.
 *
 * The generator is a faithful port: same base patterns, same seven rule
 * templates, same per-iteration expression selection and same lookback
 * rewriting. The only deliberate differences are that the random engine is
 * seeded explicitly (the tests must be reproducible) and that random stream
 * values are drawn from at most 64 bits, since a `bv[256]` value is not
 * representable in a `uint64_t` and the width of the value does not change what
 * is being exercised.
 */

#include <cstdlib>
#include <fstream>
#include <random>
#include <regex>
#include <set>
#include <string>
#include <vector>

#include "test_integration-interpreter_helper.h"

namespace {

// -------- Base patterns for bitvector expressions --------

constexpr unsigned short default_bv_width = 64;
const std::vector<unsigned short> random_bv_widths =
	{ 1, 2, 4, 8, 16, 32, 64, 128, 256 };

const strings ops_bv = { "+", "-", "!&", "!|", "!^" };
const strings comparisons_bv =
	{ "<", "!<", "<=", "!<=", ">", "!>", ">=", "!>=", "=", "!=" };

/// Number of input streams the generated rules may refer to.
constexpr size_t inputs_n = 16;
/// Highest output stream index a template can write (`o1`..`o10`).
constexpr size_t outputs_n = 10;

using rng_t = std::mt19937_64;

/// @brief Largest value representable in @p width bits, saturated at 64 bits.
uint64_t max_value_of(unsigned short width) {
	return width >= 64 ? std::numeric_limits<uint64_t>::max()
			   : ((uint64_t(1) << width) - 1);
}

/// @brief Uniform integer in [lo, hi], mirroring python's `random.randint`.
uint64_t rand_int(rng_t& rng, uint64_t lo, uint64_t hi) {
	return std::uniform_int_distribution<uint64_t>(lo, hi)(rng);
}

/// @brief Uniformly pick one element of @p v, mirroring `random.choice`.
template <typename T>
const T& rand_choice(rng_t& rng, const std::vector<T>& v) {
	return v[rand_int(rng, 0, v.size() - 1)];
}

/// @brief Sample @p k distinct elements of @p v, mirroring `random.sample`.
strings rand_sample(rng_t& rng, const strings& v, size_t k) {
	strings pool = v;
	std::shuffle(pool.begin(), pool.end(), rng);
	pool.resize(k);
	return pool;
}

std::string bv_const(uint64_t value, unsigned short width) {
	return "{ " + std::to_string(value) + " }:bv[" + std::to_string(width)
		+ "]";
}

std::string input_ref(size_t index, unsigned short width,
	const std::string& time_ref = "t")
{
	return "i" + std::to_string(index) + "[" + time_ref + "]:bv["
		+ std::to_string(width) + "]";
}

std::string replace_all(std::string s, const std::string& from,
	const std::string& to)
{
	for (size_t p = s.find(from); p != std::string::npos;
		p = s.find(from, p + to.size()))
			s.replace(p, from.size(), to);
	return s;
}

strings build_base_patterns_bv(unsigned short width) {
	strings in;
	for (size_t i = 1; i <= inputs_n; ++i) in.push_back(input_ref(i, width));
	uint64_t max_value = max_value_of(width);
	auto k = [&](uint64_t v) { return bv_const(std::min(v, max_value), width); };
	std::string one = k(1), two = k(2), seven = k(7), forty_two = k(42),
		sixty_six = k(66), one_seventy = k(170);
	return {
		"(" + in[0] + " + " + in[1] + ")",
		"(" + in[2] + " - " + in[3] + ")",
		"(" + in[4] + " + " + forty_two + ")",
		"(" + in[6] + " - " + seven + ")",
		"(" + in[8] + " + (" + in[9] + " << " + one + "))",
		"(" + in[0] + " !& " + in[1] + ")",
		"(" + in[2] + " !| " + in[3] + ")",
		"(" + in[4] + " !^ " + in[5] + ")",
		"(" + in[6] + " !& " + in[6] + ")",
		"(0 - " + in[12] + ")",
		"(" + in[13] + " << " + two + ")",
		"(" + in[14] + " >> " + one + ")",
		"((" + in[0] + " + " + in[1] + ") !& (" + in[2] + " - "
			+ one_seventy + "))",
		"((0 - (" + in[3] + " + " + sixty_six + ")) !| (" + in[4]
			+ " !^ " + in[5] + "))",
		"((" + in[6] + " << " + two + ") + (" + in[7] + " >> " + one
			+ ") - " + in[8] + ")",
	};
}

/// @brief Expressions used at iteration @p idx: one pattern at first, then
/// growing prefixes, then random samples.
strings choose_exprs_for_iteration(rng_t& rng, size_t idx,
	const strings& base_patterns)
{
	size_t count = base_patterns.size();
	if (idx <= count) return { base_patterns[idx - 1] };
	if (idx <= 2 * count) return strings(base_patterns.begin(),
		base_patterns.begin() + std::min(idx - count + 1, count));
	size_t rand_len = rand_int(rng, std::max<size_t>(3, count / 3), count);
	return rand_sample(rng, base_patterns, rand_len);
}

std::string build_folded_expr(rng_t& rng, const strings& exprs) {
	std::string acc = exprs[0];
	for (size_t i = 1; i < exprs.size(); ++i)
		acc = "(" + acc + " " + rand_choice(rng, ops_bv) + " "
			+ exprs[i] + ")";
	return acc;
}

/// @brief Rewrite a random subset of `iN[t]` references into `iN[t-k]`.
std::string apply_lookback(rng_t& rng, const std::string& rule,
	size_t max_lookback)
{
	if (max_lookback == 0) return rule;
	static const std::regex re(R"(i(\d+)\[t\](:bv\[\d+\])?)");
	std::string out;
	auto begin = std::sregex_iterator(rule.begin(), rule.end(), re);
	auto end = std::sregex_iterator();
	size_t last = 0;
	for (auto it = begin; it != end; ++it) {
		const std::smatch& m = *it;
		out.append(rule, last, m.position() - last);
		uint64_t k = rand_int(rng, 0, max_lookback);
		if (k == 0) out.append(m.str());
		else out.append("i" + m[1].str() + "[t-" + std::to_string(k)
			+ "]" + m[2].str());
		last = m.position() + m.length();
	}
	out.append(rule, last, std::string::npos);
	return out;
}

/// Number of rule templates the load tester cycles through.
constexpr size_t templates_n = 7;

/// @brief Build rule template @p template_no (1-based) of iteration @p idx from
/// @p exprs. The load tester always uses `(idx - 1) % 7 + 1` as the template;
/// it is a separate parameter here so a test can exercise a chosen subset.
std::string build_template_rule(rng_t& rng, strings exprs, size_t idx,
	unsigned short width, const strings& base_patterns, size_t template_no)
{
	while (exprs.size() < 4) exprs.push_back(
		rand_choice(rng, base_patterns));

	std::string out_a = "1";
	std::string out_b = std::to_string(2 + (idx % 4));
	std::string out_c = std::to_string(6 + (idx % 5));
	uint64_t max_value = max_value_of(width);

	auto k = [&](uint64_t v) { return bv_const(std::min(v, max_value), width); };
	auto sample_constant = [&](const std::vector<uint64_t>& candidates) {
		std::set<uint64_t> bounded;
		for (uint64_t c : candidates)
			bounded.insert(std::min(c, max_value));
		std::vector<uint64_t> v(bounded.begin(), bounded.end());
		return bv_const(rand_choice(rng, v), width);
	};

	uint64_t widest_shift = width > 1 ? uint64_t(width) - 1 : 1;
	std::string shift_a = bv_const(rand_int(rng, 1,
		std::min<uint64_t>(4, widest_shift)), width);
	std::string shift_b = bv_const(rand_int(rng, 1,
		std::min<uint64_t>(5, widest_shift)), width);
	std::string shift_c = k(2);
	std::string threshold_a = sample_constant(
		{ 1, 2, 4, 8, 16, 32, 64, 96, 128, 160, 192 });
	std::string threshold_b = sample_constant(
		{ 1, 2, 3, 4, 8, 16, 24, 48, 72, 96, 120 });
	std::string low_limit = sample_constant(
		{ 1, 2, 4, 8, 16, 32, 64, 96, 128, 160 });
	std::string high_limit = sample_constant(
		{ 2, 4, 8, 16, 32, 64, 128, 256, 384, 512, 768 });
	std::string time_cutoff = sample_constant(
		{ 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192 });
	std::string auth_token = sample_constant({ 1, 3, 7, 11, 42, 99, 255 });

	std::string folded = build_folded_expr(rng, exprs);
	strings alt_src(exprs.begin(), exprs.begin()
		+ std::max<size_t>(2, std::min<size_t>(6, exprs.size())));
	std::reverse(alt_src.begin(), alt_src.end());
	std::string alt_folded = build_folded_expr(rng, alt_src);

	const std::string& primary = exprs[0];
	const std::string& secondary = exprs[1];
	const std::string& tertiary = exprs[2];
	const std::string& quaternary = exprs[3];
	std::string comparator = rand_choice(rng, comparisons_bv);

	std::string w = std::to_string(width);
	std::string prev = replace_all(primary, "[t]", "[t-1]");

	switch ((template_no - 1) % templates_n) {
	case 0: // threshold gate
		return "always ( ((" + primary + " + " + secondary + ") >= "
			+ threshold_a + " && o" + out_a + "[t] = " + k(1) + ")"
			" || ((" + primary + " + " + secondary + ") < "
			+ threshold_a + " && o" + out_a + "[t] = " + k(0)
			+ ") ).";
	case 1: // feature extractor
		return "always ( (o" + out_a + "[t]:bv[" + w + "] = ("
			+ primary + " >> " + shift_a + ") + (" + secondary
			+ " << " + shift_b + "))"
			" && (o" + out_b + "[t]:bv[" + w + "] = (" + tertiary
			+ " - (" + primary + " >> " + k(1) + "))) ).";
	case 2: // temporal classifier
		return "always ( ((" + primary + " > " + prev + " && o" + out_a
			+ "[t] = " + k(1) + ")"
			" || (" + primary + " <= " + prev + " && o" + out_a
			+ "[t] = " + k(0) + "))"
			" && (o" + out_b + "[t]:bv[" + w + "] = (" + secondary
			+ " + (" + tertiary + " >> " + k(1) + "))) ).";
	case 3: // multi output
		return "always ( (o" + out_a + "[t]:bv[" + w + "] = " + folded
			+ ")"
			" && (o" + out_b + "[t]:bv[" + w + "] = (" + secondary
			+ " + " + tertiary + " - " + quaternary + "))"
			" && (o" + out_c + "[t]:bv[" + w + "] = (" + primary
			+ " !^ " + secondary + ")) ).";
	case 4: // network style locals
		return "always ( ex s1 ex s2 ex h1 ex h2 ("
			" (s1 = ((" + primary + " << " + k(1) + ") + " + primary
			+ " + (" + secondary + " << " + k(1) + ")))"
			" && ((s1 >= " + threshold_a + " && h1 = " + k(1)
			+ ") || (s1 < " + threshold_a + " && h1 = " + k(0)
			+ "))"
			" && (s2 = ((" + tertiary + " << " + shift_c + ") + "
			+ tertiary + " + (" + k(0) + " - " + quaternary + ")))"
			" && ((s2 >= " + threshold_b + " && h2 = " + k(1)
			+ ") || (s2 < " + threshold_b + " && h2 = " + k(0)
			+ "))"
			" && (((h2 = " + k(1) + ") && o" + out_a + "[t] = "
			+ k(200) + ")"
			" || ((h2 = " + k(0) + ") && ((h1 = " + k(1) + " && o"
			+ out_a + "[t] = " + k(50) + ")"
			" || (h1 = " + k(0) + " && o" + out_a + "[t] = " + k(0)
			+ "))))"
			" && (o" + out_b + "[t]:bv[" + w + "] = s1 + s2)"
			" ) ).";
	case 5: // temporary spending limit
		return "always ( (" + secondary + " " + comparator + " "
			+ alt_folded + ")"
			" -> ((i5[t]:bv[" + w + "] < " + time_cutoff + " && "
			+ primary + " > " + high_limit + ") ? o" + out_a
			+ "[t] = " + k(0) + " :"
			" ((!(i5[t]:bv[" + w + "] < " + time_cutoff + ") && "
			+ primary + " > " + low_limit + ") ? o" + out_a
			+ "[t] = " + k(0) + " :"
			" o" + out_a + "[t] = " + k(1) + ")) ).";
	default: // 2fa gate
		return "always ( ((" + folded + " > " + high_limit
			+ " && !(i6[t]:bv[" + w + "] = " + auth_token + "))"
			" ? o" + out_a + "[t] = " + k(0) + " : o" + out_a
			+ "[t] = " + k(1) + ")"
			" && (o" + out_b + "[t]:bv[" + w + "] = (" + secondary
			+ " + (" + tertiary + " << " + k(1) + "))) ).";
	}
}

std::string build_rule_for_iteration(rng_t& rng, size_t idx,
	size_t max_lookback, unsigned short width, size_t template_no)
{
	strings base_patterns = build_base_patterns_bv(width);
	strings exprs = choose_exprs_for_iteration(rng, idx, base_patterns);
	return apply_lookback(rng, build_template_rule(rng, exprs, idx, width,
		base_patterns, template_no), max_lookback);
}

std::string build_rule_for_iteration(rng_t& rng, size_t idx,
	size_t max_lookback, unsigned short width)
{
	return build_rule_for_iteration(rng, idx, max_lookback, width,
		(idx - 1) % templates_n + 1);
}

// -------- Driver --------

struct stress_config {
	size_t iterations = 12;
	unsigned short width = 8;
	size_t max_lookback = 0;
	uint64_t seed = 20260807;
	/// Templates to cycle through, 1-based. The load tester cycles through
	/// all seven; template 5 is left out of the default because it does not
	/// normalize in a bounded time (see the test suite below).
	std::vector<size_t> templates = { 1, 2, 3, 4, 6, 7 };
	/// Rules to feed instead of generating them. Empty (the default) keeps
	/// the generator; non-empty replaces it entirely, and `templates`,
	/// `max_lookback` and `seed` then only affect the random stream values.
	/// Only the first `iterations` entries are fed; a shorter list is padded
	/// with `T`, which leaves the running spec untouched. Declared last so
	/// designated initialisers can name it after `iterations` and `width`.
	strings rules = {};
};

struct stress_result {
	bool started = false;      ///< The interpreter was built.
	size_t steps_executed = 0; ///< Value of `time_point` when `run` returned.
	strings rules;             ///< The rules fed through `i0`, in order.
	strings o1_values;         ///< Whatever `o1` received.
};

/// @brief Feed `cfg.iterations` generated rules through `i0` of the
/// specification `u[t] = i0[t].`, answering `i1`..`i16` with random bitvector
/// values, exactly as the load tester drives the REPL.
stress_result run_stress(const stress_config& cfg) {
	bdd_init<Bool>();
	// the type of a stream name is global to the process, so without this a
	// case running at one width inherits `i1:bv[N]` from the case before it
	// and every rule it feeds fails to parse
	definitions<node_t>::instance().clear();
	rng_t rng(cfg.seed);

	stress_result res;
	if (cfg.rules.empty()) {
		REQUIRE( !cfg.templates.empty() );
		for (size_t idx = 1; idx <= cfg.iterations; ++idx)
			res.rules.push_back(build_rule_for_iteration(rng, idx,
				cfg.max_lookback, cfg.width,
				cfg.templates[(idx - 1) % cfg.templates.size()]));
	} else {
		res.rules = cfg.rules;
		if (res.rules.size() > cfg.iterations)
			res.rules.resize(cfg.iterations);
	}

	// the interpreter may read one value past the last executed step
	const size_t values_n = cfg.iterations + 2;
	// random values are drawn from at most 64 bits, see the file comment
	const unsigned short value_bits = std::min<unsigned short>(cfg.width, 64);

	io_context<node_t> ctx;
	size_t bv_type = bv_type_id<node_t>(cfg.width);

	strings i0_values = res.rules;
	i0_values.resize(values_n, "T"); // `T` leaves the running spec untouched
	ctx.add_input("i0", tau_type_id<node_t>(),
		std::make_shared<vector_input_stream>(i0_values));

	for (size_t i = 1; i <= inputs_n; ++i) {
		strings values;
		for (size_t s = 0; s < values_n; ++s) values.push_back(
			std::to_string(rand_int(rng, 0,
				max_value_of(value_bits))));
		ctx.add_input("i" + std::to_string(i), bv_type,
			std::make_shared<vector_input_stream>(values));
	}

	auto o1 = std::make_shared<vector_output_stream>();
	ctx.add_output("o1", bv_type, o1);
	for (size_t o = 2; o <= outputs_n; ++o)
		ctx.add_output("o" + std::to_string(o), bv_type,
			std::make_shared<vector_output_stream>());
	// `u` must be a tau output for `run` to apply the pointwise revision
	ctx.add_output("u", tau_type_id<node_t>(),
		std::make_shared<vector_output_stream>());

	auto spec = create_spec("u[t] = i0[t].");
	auto interpreter = run<node_t>(spec, ctx, cfg.iterations);
	if (!interpreter.has_value()) return res;

	res.started = true;
	res.steps_executed = interpreter.value().time_point;
	res.o1_values = o1->get_values();
	return res;
}

/// @brief Run a single generated rule, so a failing template is reported on its
/// own instead of taking the whole sequence down with it.
stress_result run_single_rule(const std::string& rule, unsigned short width) {
	bdd_init<Bool>();
	definitions<node_t>::instance().clear(); // see run_stress
	rng_t rng(20260807);

	io_context<node_t> ctx;
	size_t bv_type = bv_type_id<node_t>(width);
	const unsigned short value_bits = std::min<unsigned short>(width, 64);

	strings i0_values = { rule, "T", "T" };
	ctx.add_input("i0", tau_type_id<node_t>(),
		std::make_shared<vector_input_stream>(i0_values));
	for (size_t i = 1; i <= inputs_n; ++i) {
		strings values;
		for (size_t s = 0; s < i0_values.size(); ++s)
			values.push_back(std::to_string(rand_int(rng, 0,
				max_value_of(value_bits))));
		ctx.add_input("i" + std::to_string(i), bv_type,
			std::make_shared<vector_input_stream>(values));
	}
	auto o1 = std::make_shared<vector_output_stream>();
	ctx.add_output("o1", bv_type, o1);
	for (size_t o = 2; o <= outputs_n; ++o)
		ctx.add_output("o" + std::to_string(o), bv_type,
			std::make_shared<vector_output_stream>());
	ctx.add_output("u", tau_type_id<node_t>(),
		std::make_shared<vector_output_stream>());

	auto spec = create_spec("u[t] = i0[t].");
	auto interpreter = run<node_t>(spec, ctx, 2);

	stress_result res;
	res.rules = { rule };
	if (!interpreter.has_value()) return res;
	res.started = true;
	res.steps_executed = interpreter.value().time_point;
	res.o1_values = o1->get_values();
	return res;
}

} // namespace

TEST_SUITE("bv stress check: rule generation") {

	TEST_CASE("every generated rule parses") {
		bdd_init<Bool>();
		for (unsigned short width : random_bv_widths) {
			rng_t rng(20260807);
			for (size_t idx = 1; idx <= 21; ++idx) {
				auto rule = build_rule_for_iteration(rng, idx,
					0, width);
				INFO("bv[", width, "] rule ", idx, ": ", rule);
				CHECK( tau::get(rule) != nullptr );
			}
		}
	}

	TEST_CASE("every generated rule parses with lookback") {
		bdd_init<Bool>();
		rng_t rng(20260807);
		for (size_t idx = 1; idx <= 21; ++idx) {
			auto rule = build_rule_for_iteration(rng, idx, 3,
				default_bv_width);
			INFO("rule ", idx, ": ", rule);
			CHECK( tau::get(rule) != nullptr );
		}
	}
}

/// @brief Generate the rule of template @p no and run it on its own.
static void check_template(size_t no) {
	rng_t rng(20260807 + no);
	auto rule = build_rule_for_iteration(rng, no, 0, 8, no);
	INFO("rule: ", rule);
	auto res = run_single_rule(rule, 8);
	CHECK( res.started );
	CHECK( res.steps_executed == 2 );
}

// One test case per rule template, at the narrowest width that still exercises
// the arithmetic, so a template that breaks is named by the failing test
// instead of taking the rest of the sequence down with it.
TEST_SUITE("bv stress check: single rule execution") {

	TEST_CASE("template 1: threshold gate")           { check_template(1); }
	TEST_CASE("template 2: feature extractor")        { check_template(2); }
	TEST_CASE("template 3: temporal classifier")      { check_template(3); }
	TEST_CASE("template 4: multi output")             { check_template(4); }
	TEST_CASE("template 6: temporary spending limit") { check_template(6); }
	TEST_CASE("template 7: 2fa gate")                 { check_template(7); }

	// Template 5 declares four existential bv locals and binds two of them
	// to large additive terms containing shifts and `!|`/`!^`. Normalizing
	// one such rule used to blow up, driven by term size rather than
	// bitvector width -- not a defect of the block simplification this
	// suite otherwise covers. Now completes in ~2s.
	TEST_CASE("template 5: network style locals") {
		check_template(5);
	}
}

// The load tester's actual load pattern: rule after rule fed to one running
// interpreter, each one revising the spec the next one runs against, so the
// spec the interpreter carries grows with every iteration.
//
// How far this could be pushed used to be bounded by the same normalization
// blowup that made template 5 unusable, under the old preprocessing=true
// default. Measured on this generator back then: bv[1] absorbed eight
// accumulated rules in about a second, bv[2] took over a minute for two and
// did not return for three, and at bv[4] and above two accumulated rules
// already did not return -- even though each of those rules on its own ran
// in well under a second (see the suite above). Under the shipped defaults
// (preprocessing=false, Task 9) the bv[2]/bv[4] cases below now pass fast, and
// the load tester's own default (bv[64], fourteen rules) now passes too --
// see its own case below for why it still stays opt-in.
TEST_SUITE("bv stress check: execution") {

	TEST_CASE("8 iterations at bv[1]") {
		auto res = run_stress({ .iterations = 8, .width = 1 });
		REQUIRE( res.started );
		CHECK( res.steps_executed == 8 );
	}

	TEST_CASE("8 iterations at bv[1] with lookback") {
		auto res = run_stress({ .iterations = 8, .width = 1,
			.max_lookback = 2 });
		REQUIRE( res.started );
		CHECK( res.steps_executed == 8 );
	}

	// Used to be the widest accumulation that still returned, and the only
	// case here that was expensive: about 85s in Release and around five
	// minutes in Debug under the old preprocessing=true default, which on its
	// own made the whole ctest run several times longer. Passes fast under
	// the shipped defaults (preprocessing=false, Task 9). Unskipped 2026-08-15.
	TEST_CASE("2 iterations at bv[2]") {
		auto res = run_stress({ .iterations = 2, .width = 2 });
		REQUIRE( res.started );
		CHECK( res.steps_executed == 2 );
	}

	// Two accumulated rules at bv[4]; did not return under the old
	// preprocessing=true default. Passes fast under the shipped defaults
	// (preprocessing=false, Task 9). Unskipped 2026-08-15.
	TEST_CASE("2 iterations at bv[4]") {
		auto res = run_stress({ .iterations = 2, .width = 4 });
		REQUIRE( res.started );
		CHECK( res.steps_executed == 2 );
	}

	// The load tester's own default: fourteen rules, all seven templates, at
	// the bv[64] it ships with. First-ever pass under the shipped defaults
	// (preprocessing=false, Task 9): ~99.1s in Release -- no longer "never
	// returns", but still over the 60s default-suite bar, so it stays
	// opt-in rather than joining the suite outright. The cvc5 option-set
	// selection (backends/cvc5/cvc5_options.h, 2026-08-16) brought
	// it to ~74s and halved its peak RSS, still over the bar. Run it directly with
	// `-tc="14 iterations*" -ns`: doctest's `-tc` splits its argument on
	// comma into separate patterns, and this case's name itself contains
	// one, so passing the full name verbatim (`-tc="14 iterations at
	// bv[64], all templates"`) silently matches nothing -- we hit this.
	TEST_CASE("14 iterations at bv[64], all templates" * doctest::skip()) {
		auto res = run_stress({ .iterations = 14,
			.width = default_bv_width,
			.templates = { 1, 2, 3, 4, 5, 6, 7 } });
		REQUIRE( res.started );
		CHECK( res.steps_executed == 14 );
	}

	// The historical logged rules (private/bv_load_test.log, translated to
	// current syntax -- see private/andrei-bv-stress.md A1), replayed at
	// bv[16]. This is the workload the blasting/solver placement experiment
	// exists to move: N=1 takes about a second, and N=2 has historically
	// never returned.
	//
	// Env-gated because the rule file is not in the repository:
	// TAU_STRESS_OLDRULES is the path to it (one rule per line, blank lines
	// ignored) and TAU_STRESS_OLDRULES_N how many of them to accumulate
	// (default 2). Unset, the case is skipped -- the decorator is evaluated
	// during static initialisation, before main() runs, which is early
	// enough for the environment to have been read.
	TEST_CASE("historical logged rules at bv[16]"
		* doctest::skip(std::getenv("TAU_STRESS_OLDRULES") == nullptr))
	{
		const char* path = std::getenv("TAU_STRESS_OLDRULES");
		REQUIRE( path != nullptr );
		std::ifstream in(path);
		REQUIRE( in.good() );
		strings rules;
		std::string line;
		while (std::getline(in, line))
			if (!line.empty()) rules.push_back(line);
		REQUIRE( !rules.empty() );
		const char* n_env = std::getenv("TAU_STRESS_OLDRULES_N");
		size_t n = n_env ? static_cast<size_t>(std::atoi(n_env)) : 2;
		REQUIRE( n > 0 );
		auto res = run_stress({ .iterations = n, .width = 16,
			.rules = rules });
		REQUIRE( res.started );
		CHECK( res.steps_executed == n );
	}
}
