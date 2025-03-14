// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <memory>

#include "../queries.h"

namespace idni::tau_lang {

using namespace z3;

using parse_forest = idni::parser<char, char>::pforest;
using parse_result = idni::parser<char, char>::result;
using z3_traverser_t  = traverser<z3_sym, bv_parser>;

template <typename...BAs>
std::optional<tau<BAs...>> z3_ba_factory<BAs...>::parse( const std::string& src) {
	/*// check source cache
	if (auto cn = cache.find(src); cn != cache.end())
		return cn->second;
	auto result = z3_parser::instance().parse(src.c_str(), src.size());
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(tau_parser::error::info_lvl::INFO_BASIC);
		BOOST_LOG_TRIVIAL(error) << "(Error) " << msg << "\n";
		return std::optional<tau<BAs...>>{}; // Syntax error
	}
	using parse_symbol = z3_parser::node_type;
	using namespace rewriter;
	auto root = make_node_from_tree<z3_parser,
		drop_location_t<parse_symbol, z3_sym>,
		z3_sym>(
			drop_location<parse_symbol, z3_sym>,
			result.get_shaped_tree());
	auto t = z3_traverser_t(root) | z3_parser::bitvector;*/
	auto b = std::make_shared<z3::expr>(ctx.bv_val(0, sizeof(int)));
	std::variant<BAs...> vp {b};
	auto n = rewriter::make_node<tau_sym<BAs...>>(vp, {});
	return cache.emplace(src, n).first->second;
}

template <typename...BAs>
tau<BAs...> z3_ba_factory<BAs...>::binding(const tau<BAs...>& sn) {
	auto source = sn
		| tau_parser::source
		| optional_value_extractor<tau<BAs...>>;
	std::string src = make_string(
		tau_node_terminal_extractor<BAs...>, source);
	if (auto parsed = parse(src); parsed.has_value())
		return parsed.value();
	return nullptr;
}

template <typename...BAs>
std::variant<BAs...> z3_ba_factory<BAs...>::splitter_one() const {
	throw std::logic_error("Unsupported");
}

template <typename...BAs>
std::string z3_ba_factory<BAs...>::one() const { return "1:bit[]"; }

template <typename...BAs>
std::string z3_ba_factory<BAs...>::zero() const { return "0:bit[]"; }

} // namespace idni::tau_lang
