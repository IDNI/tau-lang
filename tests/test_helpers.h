// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifdef DEBUG
// including instead of #include "tau.h" to avoid errors pointing to the generated tau.h
#	include "boolean_algebras/nso_ba.h"
#	include "boolean_algebras/sbf_ba.h"
#else
#	include "tau.h"
#endif // DEBUG

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "testing"

// common helpers to be included in test_*_helpers.h after defining bas_pack macro

namespace idni::tau_lang {
	
using node_t = tau_lang::node<bas_pack>;
using tau = tree<node_t>;
using tt = tau::traverser;
using bac = ba_constants<node_t>;

tau::get_options parse_library() {
	static tau::get_options opts{ .parse = { .start = tau::library },
		.infer_ba_types = false };
	return opts;
}

tau::get_options parse_bf() {
	static tau::get_options opts{ .parse = { .start = tau::bf } };
	return opts;
}

tau::get_options parse_wff() {
	static tau::get_options opts{ .parse = { .start = tau::bf } };
	return opts;
}

tau::get_options parse_rec_relations() {
	static tau::get_options opts{ .parse = { .start = tau::rec_relations } };
	return opts;
}

std::optional<rr<node_t>> get_bf_nso_rr(const char* rec, const char* sample) {
	rewriter::rules rrs = get_rec_relations<node_t>(
			tau::get(rec, parse_rec_relations()));
	tref main_fm = tau::get(sample, parse_bf());
	if (!main_fm) return {};
	return get_nso_rr<node_t>(rrs, main_fm);
}

std::optional<rr<node_t>> get_nso_rr(const char* sample,
	bool wo_inference = false)
{
	LOG_TRACE << "get_nso_rr: " << sample;
	tref spec = tau::get(sample);
	assert(spec != nullptr);
	return get_nso_rr<node_t>(spec, wo_inference);
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

	LOG_DEBUG << "(T) nso_rr: " << to_str<node_t>(nso_rr.value());

	tref result = normalizer<node_t>(nso_rr.value());
	if (!result) return expect_fail;

	LOG_DEBUG << "(T) Normalized result: " << LOG_FM(result);

	return tau::get(result).child_is(nt) != expect_fail;
}

bool normalize_and_expect_fail(const char* sample, typename node_t::type nt) {
	return normalize_and_check(sample, nt, true);
}

bool normalize_and_check(const char* sample, const std::string& expected) {
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return false;

	tref result = normalizer<node_t>(nso_rr.value());
	if (!result) return false;

        return tau::get(result).to_str() == expected;
}

} // namespace idni::tau_lang
