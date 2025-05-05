// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau.h"

// common helpers to be included in test_*_helpers.h after defining bas_pack macro

namespace idni::tau_lang {

using bnode = node<bas_pack>;
using tau = tree<bnode>;
using tt = tau::traverser;
using bac = ba_constants<bas_pack>;
using bacb = ba_constants_binder<bas_pack>;

tau::parse_options parse_library() {
	static tau::parse_options opts(
		tau_parser::parse_options{ .start = tau::library });
	return opts;
}

tau::parse_options parse_bf() {
	static tau::parse_options opts(
		tau_parser::parse_options{ .start = tau::bf });
	return opts;
}

tau::parse_options parse_wff() {
	static tau::parse_options opts(
		tau_parser::parse_options{ .start = tau::bf });
	return opts;
}

tau::parse_options parse_rec_relations() {
	static tau::parse_options opts(
		tau_parser::parse_options{ .start = tau::rec_relations });
	return opts;
}

std::optional<rr> get_bf_nso_rr(const char* rec, const char* sample) {
	rewriter::rules rrs = get_rec_relations<bnode>(
			tau::get(rec, parse_rec_relations()));
	tref main_fm = tau::get(sample, parse_bf());
	if (!main_fm) return {};
	return get_nso_rr<bnode>(rrs, main_fm);
}

std::optional<rr> get_nso_rr(const char* sample, bool wo_inference = false) {
	return get_nso_rr<bnode>(tau::get(sample), wo_inference);
}

bool get_nso_rr_and_check(const char* sample, tau::node::type nt){
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return false;
	auto x = tt(nso_rr.value().main) | nt;
	return x.has_value();
}

bool normalize_and_check(const char* sample,
	typename bnode::type nt, bool expect_fail = false)
{
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return expect_fail;

	tref result = normalizer<bnode>(nso_rr.value());
	if (!result) return expect_fail;
	return tau::get(result).child_is(nt) != expect_fail;
}

bool normalize_and_expect_fail(const char* sample, typename bnode::type nt) {
	return normalize_and_check(sample, nt, true);
}

bool normalize_and_check(const char* sample, const std::string& expected) {
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return false;

	tref result = normalizer<bnode>(nso_rr.value());
	if (!result) return false;

        return tau::get(result).to_str() == expected;
}

} // namespace idni::tau_lang
