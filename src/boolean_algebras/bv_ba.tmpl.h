// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/bv_ba.h" // Only for IDE resolution, not really needed.
#include "../parser/bitvector_parser.generated.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
size_t get_bv_size(const tref t) {
	using tau = tree<node>;
	using tt = tau::traverser;
	if (auto subtype = tt(t) | tau::subtype | tt::ref; subtype) {
		return tau::get(subtype)[0].get_num();
	}
	return default_bv_size;
}

template<typename ... BAs> requires BAsPack<BAs...>
std::optional<bv> bv_constant_from_parse_tree(tref parse_tree, tref type_tree) {
	using tt = bitvector_parser::tree::traverser;
	using bv_parser = bitvector_parser::nonterminal;

	if (!parse_tree) return std::nullopt;
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
			return std::nullopt;
		}
	}
	DBG(assert(base > 0 );)
	size_t bv_size = get_bv_size<node<BAs...>>(type_tree);
	auto str = t | tt::terminals;
	try {
		return make_bitvector_cte(bv_size, str, base);
	} catch (const cvc5::CVC5ApiException& e) {
		LOG_ERROR << "Error creating bitvector constant from string '"
			<< str << "': " << e.what() << "\n";
		return std::nullopt;
	}
}

template<typename...BAs>
requires BAsPack<BAs...>
std::optional<typename node<BAs...>::constant_with_type> parse_bv(const std::string& src,
		tref type_tree) {

	auto result = bitvector_parser::instance().parse(src.c_str(), src.size());
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(bitvector_parser::error::info_lvl::INFO_BASIC);
		LOG_ERROR << "[bv] " << msg << "\n";
		return {}; // Syntax error
	}
	auto cte = bv_constant_from_parse_tree<BAs...>(result.get_shaped_tree2(), type_tree);
	if (!cte) {
		LOG_ERROR << "Failed to parse bitvector constant: " << src;
		return {};
	}
	return typename node<BAs...>::constant_with_type{ cte.value(), type_tree };
}

bool operator==(const Term& lhs, const bool& rhs) {
	auto bv_size = lhs.getSort().getBitVectorSize();
	return (rhs) ? normalize_bv(lhs) == make_bitvector_top_elem(bv_size)
				: normalize_bv(lhs) == make_bitvector_bottom_elem(bv_size);
}

bool operator==(const bool& lhs, const Term& rhs) { return rhs == lhs; }

bool operator!=(const Term& lhs, const bool& rhs) { return !(lhs == rhs); }

bool operator!=(const bool& lhs, const Term& rhs) { return !(rhs == lhs); }

template<NodeType node>
tref simplify_bv_term(tref term) {
	/*using node_t = node<BAs...>;
	using tau = tree<node_t>;
	using tt = tau::traverser;

	subtree_map<node_t, bv> vars, free_vars;
	auto bv_term = bv_eval_node<node_t>(tt(term), vars, free_vars);
	if (!bv_term) return term; // Unable to transform to bv
	auto simplified_bv = normalize(bv_term.value());
	return cvc5_tree_to_tau_tree<node_t>(simplified_bv);*/
	return term;
}

} // namespace idni::tau_lang