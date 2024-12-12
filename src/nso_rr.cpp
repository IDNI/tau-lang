// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt
#include "nso_rr.h"

namespace idni::tau_lang {

using namespace idni::rewriter;

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
	using parse_symbol = tau_parser::node_type;
	return make_node_from_string<
		tau_parser,
		drop_location_t<parse_symbol, tau_source_sym>,
		tau_source_sym>(drop_location<parse_symbol, tau_source_sym>,
			source, options);
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

} // idni::tau_lang namespace

// << for tau_source_sym
std::ostream& operator<<(std::ostream& stream, const idni::tau_lang::tau_source_sym& rs) {
	if (rs.nt()) stream << rs.t();
	return stream;
}

// outputs a sp_tau_source_node to a stream, using the stringify transformer
// and assumes that the constants also override operator<<.
//
// IDEA maybe it should be move to out.h
std::ostream& operator<<(std::ostream& stream, const idni::tau_lang::sp_tau_source_node& n){
	return stream << idni::tau_lang::make_string(idni::tau_lang::tau_source_terminal_extractor, n);
}

// << tau_source_node (make it shared to make use of the previous operator)
std::ostream& operator<<(std::ostream& stream, const idni::tau_lang::tau_source_node& n){
	return stream << std::make_shared<idni::tau_lang::tau_source_node>(n);
}
