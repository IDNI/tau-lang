// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "boolean_algebras/nso_ba_depreciating.h"

namespace idni::tau_lang::depreciating {

using namespace idni::rewriter::depreciating;

bool pretty_printer_highlighting = false;
bool pretty_printer_indenting  = false;

// extracts terminal from sp_tau_source_node
std::function<std::optional<char>(const sp_tau_source_node& n)>
	tau_source_terminal_extractor = [](const sp_tau_source_node& n)
{
	if (!n->value.nt() && !(n->value).is_null())
		return std::optional<char>(n->value.t());
	return std::optional<char>();
};

// make a tau source from the given source code string.
sp_tau_source_node make_tau_source(const std::string& source,
	tau_parser::parse_options options)
{
	// IDEA add cache for the parser
	auto result = tau_parser::instance().parse(
		source.c_str(), source.size(), options);
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(tau_parser::error::info_lvl::INFO_BASIC);
		BOOST_LOG_TRIVIAL(error) << "(Error) " << msg << "\n";
		return nullptr; // Syntax error
	}
	using parse_symbol = tau_parser::node_type;
	return make_node_from_parse_result<
		tau_parser,
		drop_location_t<parse_symbol, tau_source_sym>,
		tau_source_sym>(drop_location<parse_symbol, tau_source_sym>,
			result);
}

// make a tau source from the given source code stream.
sp_tau_source_node make_tau_source(std::istream& is,
	tau_parser::parse_options options)
{
	using parse_symbol = tau_parser::node_type;
	return make_node_from_stream<
		tau_parser,
		drop_location_t<parse_symbol, tau_source_sym>,
		tau_source_sym>(drop_location<parse_symbol, tau_source_sym>,
			is, options);
}

// make a tau source from the given source code stream.
sp_tau_source_node make_tau_source_from_file(const std::string& filename,
	tau_parser::parse_options options)
{
	using parse_symbol = tau_parser::node_type;
	return make_node_from_file<
		tau_parser,
		drop_location_t<parse_symbol, tau_source_sym>,
		tau_source_sym>(drop_location<parse_symbol, tau_source_sym>,
			filename, options);
}

} // idni::tau_lang::depreciating namespace

// << for tau_source_sym
// std::ostream& operator<<(std::ostream& stream, const idni::tau_lang::tau_source_sym& rs) {
// 	if (rs.nt()) stream << rs.t();
// 	return stream;
// }

// outputs a sp_tau_source_node to a stream, using the stringify transformer
// and assumes that the constants also override operator<<.
//
// IDEA maybe it should be move to out.h
std::ostream& operator<<(std::ostream& stream, const idni::tau_lang::depreciating::sp_tau_source_node& n){
	return stream << idni::tau_lang::depreciating::make_string(idni::tau_lang::depreciating::tau_source_terminal_extractor, n);
}

// << tau_source_node (make it shared to make use of the previous operator)
std::ostream& operator<<(std::ostream& stream, const idni::tau_lang::depreciating::tau_source_node& n){
	return stream << std::make_shared<idni::tau_lang::depreciating::tau_source_node>(n);
}
