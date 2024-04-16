// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.
#include "nso_rr.h"

namespace idni::tau {

std::function<bool(const size_t n)>& get_is_non_essential_terminal() {
	static std::function<bool(const size_t n)> fn = [](const size_t n)
	{
		return n == tau_parser::nul
			|| n == tau_parser::eof
			|| n == tau_parser::space
			|| n == tau_parser::digit
			|| n == tau_parser::xdigit
			|| n == tau_parser::alpha
			|| n == tau_parser::alnum
			|| n == tau_parser::punct
			|| n == tau_parser::printable
			|| n == tau_parser::comment
			|| n == tau_parser::__
			|| n == tau_parser::_
			|| n == tau_parser::open_bracket
			|| n == tau_parser::close_bracket
			|| n == tau_parser::open_brace
			|| n == tau_parser::close_brace;
	};
	return fn;
}

std::function<bool(const size_t n)>& is_non_essential_terminal =
	get_is_non_essential_terminal();

std::function<bool(const tau_source_sym& n)>& get_is_non_essential_sym() {
	static std::function<bool(const tau_source_sym& n)> fn =
		[](const tau_source_sym&n)
	{
		if (!n.nt()) return false;
		return get_is_non_essential_terminal()(n.n());
	};
	return fn;
}

std::function<bool(const tau_source_sym&)>& is_non_essential_sym =
	get_is_non_essential_sym();

std::function<bool(const sp_tau_source_node&)> is_non_essential_source =
	[] (const sp_tau_source_node& n)
{
	if (!n->value.nt()) return false;
	return get_is_non_essential_terminal()(n->value.n());
};

// extracts terminal from sp_tau_source_node
std::function<std::optional<char>(const sp_tau_source_node& n)>
	tau_source_terminal_extractor = [](const sp_tau_source_node& n)
{
	if (!n->value.nt() && !(n->value).is_null())
		return std::optional<char>(n->value.t());
	return std::optional<char>();
};

sp_tau_source_node clean_tau_source(const sp_tau_source_node& tau_source) {
	// FIXME (LOW) fix the trim implementation
	return trim_top<
			is_non_essential_source_t,
			tau_source_sym,
			sp_tau_source_node>(
		tau_source, is_non_essential_source);
}

// make a tau source from the given source code string.
sp_tau_source_node make_tau_source(const std::string& source,
	tau_parser::parse_options options)
{
	using parse_symbol = tau_parser::node_type;
	return make_node_from_string<
		tau_parser,
		drop_location_t<parse_symbol, tau_source_sym>,
		parse_symbol,
		tau_source_sym>(
			drop_location<parse_symbol, tau_source_sym>, source, options);
}

// make a tau source from the given source code stream.
sp_tau_source_node make_tau_source(std::istream& is) {
	using parse_symbol = tau_parser::node_type;
	return make_node_from_stream<
			tau_parser,
			drop_location_t<parse_symbol, tau_source_sym>,
			parse_symbol,
			tau_source_sym>(
		drop_location<parse_symbol, tau_source_sym>, is);
}

// make a tau source from the given source code stream.
sp_tau_source_node make_tau_source_from_file(const std::string& filename) {
	using parse_symbol = tau_parser::node_type;
	return make_node_from_file<
		tau_parser,
		drop_location_t<parse_symbol, tau_source_sym>,
		parse_symbol,
		tau_source_sym>(
			drop_location<parse_symbol, tau_source_sym>, filename);
}

} // idni::tau namespace

// << for tau_source_sym
std::ostream& operator<<(std::ostream& stream, const idni::tau::tau_source_sym& rs) {
	if (rs.nt()) stream << rs.t();
	return stream;
}

// outputs a sp_tau_source_node to a stream, using the stringify transformer
// and assumes that the constants also override operator<<.
//
// IDEA maybe it should be move to out.h
std::ostream& operator<<(std::ostream& stream, const idni::tau::sp_tau_source_node& n){
	return stream << idni::tau::make_string(idni::tau::tau_source_terminal_extractor, n);
}

// << tau_source_node (make it shared to make use of the previous operator)
std::ostream& operator<<(std::ostream& stream, const idni::tau::tau_source_node& n){
	return stream << std::make_shared<idni::tau::tau_source_node>(n);
}
