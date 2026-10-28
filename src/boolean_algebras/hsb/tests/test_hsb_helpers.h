// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__HSB__TESTS__TEST_HSB_HELPERS_H__
#define __IDNI__TAU__HSB__TESTS__TEST_HSB_HELPERS_H__

// Builders of exact hsb values from the doubles the hsb suites write.
#include <charconv>
#include <string_view>
#include <vector>

#include "boolean_algebras/hsb/hsb.h"

// The value of a test double as its literal reads, so 0.3 is 3/10: the
// shortest fixed-point text that reads back as d, parsed exactly.
inline idni::tau_lang::hsb_rational exact(double d) {
	char buf[512];
	auto res = std::to_chars(buf, buf + sizeof buf, d, std::chars_format::fixed);
	return *idni::tau_lang::hsb_rational::parse(std::string_view(buf, res.ptr));
}

inline std::vector<idni::tau_lang::hsb_rational> exact(const std::vector<double>& v) {
	std::vector<idni::tau_lang::hsb_rational> r;
	for (double d : v) r.push_back(exact(d));
	return r;
}

// The halfspace H_{w,b}: <w,x> + b {<=,<} 0.
inline idni::tau_lang::hsb make_hs(std::vector<double> w, double b) {
	idni::tau_lang::hsb_halfspace h;
	h.w = exact(w);
	h.b = exact(b);
	return idni::tau_lang::hsb::from_halfspace(h);
}

#endif // __IDNI__TAU__HSB__TESTS__TEST_HSB_HELPERS_H__
