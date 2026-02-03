// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifdef DEBUG
// including instead of #include "tau.h" to avoid errors pointing to the generated tau.h
#	include "boolean_algebras/nso_ba.h"
#	include "boolean_algebras/sbf_ba.h"
#	include "boolean_algebras/bv_ba.h"
#	include "boolean_algebras/variant_ba.h"
#   include "ba_constants.h"
#   include "runtime.h"
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

tau::get_options parse_bf() {
	static tau::get_options opts{ .parse = { .start = tau::bf } };
	return opts;
}

tau::get_options parse_wff() {
	static tau::get_options opts{ .parse = { .start = tau::wff } };
	return opts;
}

tau::get_options parse_rec_relations() {
	static tau::get_options opts{ .parse = { .start = tau::definitions } };
	return opts;
}

std::optional<rr<node_t>> get_bf_nso_rr(const char* rec, const char* sample) {
	auto prr = parse_rec_relations();
	rewriter::rules rrs = get_rec_relations<node_t>(
			tau::get(rec, prr));
	auto pbf = parse_bf();
	tref main_fm = tau::get(sample, pbf);
	if (!main_fm) return {};
	return rr<node_t>(rrs, tau::geth(main_fm));
}

std::optional<rr<node_t>> get_nso_rr(const char* sample)
{
	// DBG(TAU_LOG_TRACE << "get_nso_rr: " << sample;)
	tref spec = tau::get(sample);
	assert(spec != nullptr);
	return get_nso_rr<node_t>(spec);
}

bool get_nso_rr_and_check(const char* sample, typename node_t::type nt){
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return false;
	auto x = tt(nso_rr.value().main) | nt;
	return x.has_value();
}

bool normalize_and_check(const char* sample,
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

bool normalize_and_expect_fail(const char* sample, typename node_t::type nt) {
	return normalize_and_check(sample, nt, true);
}

bool matches_to_any_of(const std::string& fm_str, const strings& expected) {
#ifdef DEBUG // check canonicity between versions of Tau
	const bool canonical = fm_str == expected[0];
	std::stringstream ss; ss << "expression: " << fm_str;
	if (!canonical) ss << TAU_LOG_ERROR_COLOR << " is not canonical"
		<< TC.CLEAR() << ". expected: " << expected[0];
	TAU_LOG_TRACE << ss.str();
	return canonical;
#endif // DEBUG
	for (const auto& e : expected) if (fm_str == e) {
		DBG(TAU_LOG_TRACE << "found in expected: " << fm_str;)
		return true;
	}
	DBG(TAU_LOG_TRACE << "not found in expected: " << fm_str;)
	return false;
}

bool matches_to_str_to_any_of(tref fm, const strings& expected) {
	return matches_to_any_of(tau::get(fm).to_str(), expected);
}

bool values_matches_any_of(const strings& values,
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

bool normalize_and_check(const char* sample, const strings& expected) {
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return false;

	tref result = normalizer<node_t>(nso_rr.value());
	if (!result) return false;

	return matches_to_str_to_any_of(result, expected);
}

bool normalize_and_check(const char* sample, const std::string& expected) {
	return normalize_and_check(sample, strings{ expected });
}

} // namespace idni::tau_lang
