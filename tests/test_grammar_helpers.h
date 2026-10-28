// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TESTS__TEST_GRAMMAR_HELPERS_H__
#define __IDNI__TAU__TESTS__TEST_GRAMMAR_HELPERS_H__

// Helpers of the grammar suites of the algebras: P is the parser a BA
// grammar generates (`<id>_parser`, with its instance()).
#include <string>

// Whether the whole of @p s parses with the grammar of @p P.
template <typename P>
bool grammar_parses(const std::string& s) {
	return P::instance().parse(s.c_str(), s.size()).found;
}

// The constants every interval-like grammar spells: top, bot and bottom.
TEST_CASE_TEMPLATE_DEFINE("parse the 'top', 'bot' and 'bottom' constants", P,
	grammar_constants)
{
	CHECK(grammar_parses<P>("top"));
	CHECK(grammar_parses<P>("bot"));
	CHECK(grammar_parses<P>("bottom"));
}

#endif // __IDNI__TAU__TESTS__TEST_GRAMMAR_HELPERS_H__
