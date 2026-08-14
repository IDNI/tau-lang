// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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

namespace idni::tau_lang {

using strings = std::vector<std::string>;

using node_t = tau_lang::node<bas_pack>;
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
	rewriter::rules rrs = get_rec_relations<node_t>(
			tau::get(rec, prr));
	auto pbf = parse_bf();
	tref main_fm = tau::get(sample, pbf);
	if (!main_fm) return {};
	return rr<node_t>(rrs, tau::geth(main_fm));
}

inline std::optional<rr<node_t>> get_nso_rr(const char* sample)
{
	// DBG(TAU_LOG_TRACE << "get_nso_rr: " << sample;)
	tref spec = tau::get(sample);
	assert(spec != nullptr);
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

	tref result = normalizer<node>(nso_rr.value());
	if (!result) return expect_fail;

	DBG(TAU_LOG_DEBUG << "(T) Normalized result: " << TAU_LOG_FM(result);)

	return tau::get(result).child_is(nt) != expect_fail;
}

inline bool normalize_and_expect_fail(const char* sample, typename node_t::type nt) {
	return normalize_and_check(sample, nt, true);
}

inline bool matches_to_any_of(const std::string& fm_str, const strings& expected) {
#ifdef DEBUG // trace canonicity between versions of Tau (diagnostic only:
	      // must NOT gate the result on expected[0] alone, or this
	      // silently narrows "matches any of expected" down to "matches
	      // the first expected value" in DEBUG builds only, while
	      // RELEASE builds correctly check the whole list below)
	const bool canonical = fm_str == expected[0];
	std::stringstream ss; ss << "expression: " << fm_str;
	if (!canonical) ss << TAU_LOG_ERROR_COLOR << " is not canonical"
		<< TC.CLEAR() << ". expected: " << expected[0];
	TAU_LOG_TRACE << ss.str();
#endif // DEBUG
	for (const auto& e : expected) if (fm_str == e) {
		DBG(TAU_LOG_TRACE << "found in expected: " << fm_str;)
		return true;
	}
	DBG(TAU_LOG_TRACE << "not found in expected: " << fm_str;)
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

inline bool normalize_and_check(const char* sample, const strings& expected) {
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return false;

	tref result = normalizer<node_t>(nso_rr.value());
	if (!result) return false;

	return matches_to_str_to_any_of(result, expected);
}

inline bool normalize_and_check(const char* sample, const std::string& expected) {
	return normalize_and_check(sample, strings{ expected });
}

} // namespace idni::tau_lang

// ── per-test GC listener ──────────────────────────────────────────────────────
// Optional per-test GC listener. Keep disabled by default: several test cases
// retain trefs across doctest callbacks, so automatic GC before each TEST_CASE
// can invalidate cached trees. Heavy suites should call do_gc() explicitly at
// safe points instead.
#if !defined(IDNI_TAU_TESTS_GC_LISTENER_REGISTERED) && defined(DOCTEST_LIBRARY_INCLUDED) && defined(TAU_ENABLE_TEST_GC_LISTENER)
#define IDNI_TAU_TESTS_GC_LISTENER_REGISTERED

#include <unordered_set>

struct TauTestGCListener : doctest::IReporter {
	using tau_ = idni::tau_lang::tau;
	TauTestGCListener(const doctest::ContextOptions&) {}
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
REGISTER_LISTENER("tau_gc", 1, TauTestGCListener);

#endif // IDNI_TAU_TESTS_GC_LISTENER_REGISTERED
