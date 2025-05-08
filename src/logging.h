// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__LOGGING_H__
#define __IDNI__TAU__LOGGING_H__

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

namespace idni::tau_lang {

struct logging {
	logging() {
		using namespace boost::log;
		core::get()->set_filter(trivial::severity >= trivial::info);
		static bool initialize_console_log = true;
		if (initialize_console_log) initialize_console_log = false,
			add_console_log(std::cout, keywords::format =
				expressions::stream << expressions::smessage);
	}
	void trace() {
		using namespace boost::log;
		core::get()->set_filter(trivial::severity >= trivial::trace);
	}
	void debug() {
		using namespace boost::log;
		core::get()->set_filter(trivial::severity >= trivial::debug);
	}
	void info() {
		using namespace boost::log;
		core::get()->set_filter(trivial::severity >= trivial::info);
	}
	void error() {
		using namespace boost::log;
		core::get()->set_filter(trivial::severity >= trivial::error);
	}
};
inline static logging log;

} // namespace idni::tau_lang

#endif //__IDNI__TAU__LOGGING_H__