// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

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
		static bool initialize_console_log = true;
		if (initialize_console_log) initialize_console_log = false,
			add_console_log(std::cout, keywords::format =
				expressions::stream << expressions::smessage);
	}
};
inline static logging_initializer initialize_logging;

#endif //__INIT_LOG_H__