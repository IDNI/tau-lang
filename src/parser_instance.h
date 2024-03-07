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
#ifndef __PARSER_INSTANCE_H__
#define __PARSER_INSTANCE_H__
#include <iostream>
namespace idni {

template <typename parser_t>
parser_t& parser_instance() {
	static parser_t instance;
	return instance;
}

#ifdef DEBUG
template <typename parser_t>
void check_parser_result(const std::string& source,
	const typename parser_t::forest_type* f, int start = -1)
{
	auto& p = parser_instance<parser_t>();
	if (!f || !p.found(start))
		BOOST_LOG_TRIVIAL(error) << "# source: `" << source << "`\n"
	 		<< p.get_error().to_str() << "\n";
	else if (f->is_ambiguous()) {
		BOOST_LOG_TRIVIAL(error) << "# source: `" << source << "`\n"
			<< "# n trees: " << f->count_trees() << "\n"
			<< "# ambiguous nodes:\n";
		for (auto& n : f->ambiguous_nodes()) {
			BOOST_LOG_TRIVIAL(error) << "\t `" << n.first.first << "` ["
				<< n.first.second[0] << "," << n.first.second[1]
				<< "]\n";
			size_t d = 0;
			for (auto ns : n.second) {
				BOOST_LOG_TRIVIAL(error) << "\t\t " << d++ << "\t";
				for (auto nt : ns) BOOST_LOG_TRIVIAL(error) << " `" << nt.first
					<< "`[" << nt.second[0] << ","
					<< nt.second[1] << "] ";
				BOOST_LOG_TRIVIAL(error) << "\n";
			}
		}
	}
}
#endif // DEBUG

} // namespace idni
#endif // __PARSER_INSTANCE_H__