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
#ifndef __INIT_LOG_H__
#define __INIT_LOG_H__

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

struct logging_initializer {
	logging_initializer() {
		using namespace boost::log;
		core::get()->set_filter(trivial::severity >= trivial::info);
		add_console_log(std::cout, keywords::format =
			expressions::stream << expressions::smessage);
	}
};
inline static logging_initializer initialize_logging;

#endif //__INIT_LOG_H__