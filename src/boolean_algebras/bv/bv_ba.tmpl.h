// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/bv/bv_ba.h" // Only for IDE resolution, not really needed.
#include "boolean_algebras/bv/parser/bitvector_parser.generated.h"
#include "tau_diagnostics.h"

#include <sstream>

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
result<size_t> get_bv_size(const tref t) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto subtype = tt(t) | tau::type | tau::subtype | tt::ref;
	if (!subtype) {
		result<size_t> r;
		return r.with_assert_check_error(code::type_error,
			"get_bv_size: bv type has no explicit bitwidth");
	}
	return result<size_t>{tau::get(subtype)[0].get_num()};
}

template<typename ... BAs> requires BAsPack<BAs...>
result<bv> bv_constant_from_parse_tree(tref parse_tree, tref type_tree) {
	using tt = bitvector_parser::tree::traverser;
	using bv_parser = bitvector_parser::nonterminal;

	result<bv> r;
	if (!parse_tree) return r;
	auto t = bitvector_parser::tree::traverser(parse_tree)
					| bitvector_parser::bitvector;

	auto type = t | tt::only_child | tt::nonterminal;
	size_t base;
	switch (type) {
		case bv_parser::decimal: { base = 10; break; }
		case bv_parser::binary: { base = 2; break; }
		case bv_parser::hexadecimal: { base = 16; break; }
		default: {
			DBG(assert(false);)
			return r;
		}
	}
	DBG(assert(base > 0 );)
	TAU_TRY(size_t bv_size, get_bv_size<node<BAs...>>(type_tree));
	auto str = t | tt::terminals;
	try {
		return r.with_value(make_bitvector_cte(bv_size, str, base));
	} catch (const cvc5::CVC5ApiException& e) {
		std::ostringstream oss;
		oss << "Error creating bitvector constant from string '"
			<< str << "': " << e.what();
		return r.with_error(code::parse_error, oss.str(), {{label::width, bv_size}});
	}
}

template<typename...BAs>
requires BAsPack<BAs...>
result<typename node<BAs...>::constant_with_type> parse_bv(const std::string& src,
		tref type_tree) {
	result<typename node<BAs...>::constant_with_type> r;

	// Normalize 0x/0b prefixes to #x/#b for the bitvector parser
	std::string normalized = src;
	if (normalized.size() >= 2 && normalized[0] == '0'
			&& (normalized[1] == 'x' || normalized[1] == 'X'
			 || normalized[1] == 'b' || normalized[1] == 'B'))
		normalized[0] = '#', normalized[1] = std::tolower(normalized[1]);

	auto parsed = bitvector_parser::instance().parse(normalized.c_str(), normalized.size());
	if (!parsed.found) {
		r.error(code::parse_error, parsed.parse_error
			.to_str(bitvector_parser::error::info_lvl::INFO_BASIC));
		return r;
	}
	// TAU_TRY_OR_ATTR evaluates its attr list before the call, so it would
	// intern the source text on every successful parse. Attach it by hand,
	// only in the error branch.
	auto bv_child = bv_constant_from_parse_tree<BAs...>(
		parsed.get_shaped_tree2(), type_tree);
	const bool bv_child_well_formed = bv_child.is_well_formed();
	auto bv_cte_opt = r.merge_take(std::move(bv_child));
	if (!bv_cte_opt && !bv_child_well_formed && !r.has_error()) {
		r.error(code::parse_error, "failed to parse a bitvector constant",
			{{label::value, src}});
	}
	if (!bv_cte_opt) return r;
	bv cte = std::move(*bv_cte_opt);
	return r.with_value(typename node<BAs...>::constant_with_type{ cte, type_tree });
}

} // namespace idni::tau_lang

// Definitions of the bool comparisons declared in namespace cvc5 by bv_ba.h;
// the helpers they call stay in idni::tau_lang and so are qualified.
namespace cvc5 {

bool operator==(const Term& lhs, const bool& rhs) {
	auto bv_size = lhs.getSort().getBitVectorSize();
	return (rhs)
		? idni::tau_lang::normalize_bv(lhs)
			== idni::tau_lang::make_bitvector_top_elem(bv_size)
		: idni::tau_lang::normalize_bv(lhs)
			== idni::tau_lang::make_bitvector_bottom_elem(bv_size);
}

bool operator==(const bool& lhs, const Term& rhs) { return rhs == lhs; }

bool operator!=(const Term& lhs, const bool& rhs) { return !(lhs == rhs); }

bool operator!=(const bool& lhs, const Term& rhs) { return !(rhs == lhs); }

} // namespace cvc5

namespace idni::tau_lang {

template<NodeType node>
result<tref> simplify_bv_term(tref term) {
	result<tref> r;
	if (auto cvc5_simplified = r.merge_take(bv_ba_cvc5_simplification<node>(term)))
		return r.with_value(*cvc5_simplified);
	// A genuine cvc5-path failure (as opposed to its ordinary decline)
	// stays a failure: falling back here could silently turn a real bug
	// into custom_simplification's answer instead of reporting it.
	if (r.has_error()) return r;
	// bv_ba_custom_simplification never declines to nullptr, only fails
	// outright, carrying the round-cap report; propagate it rather than
	// falling back to term silently.
	auto custom = r.merge_take(bv_ba_custom_simplification<node>(term));
	if (!custom) return r;
	return r.with_value(*custom);
}

} // namespace idni::tau_lang