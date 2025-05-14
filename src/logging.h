// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

// Logging streams:
//  LOG_ERROR   << "msg";         // "(Error) msg"
//  LOG_WARNING << "msg";         // "(Warning) msg"
//  LOG_INFO    << "msg";         // "msg"
//  LOG_DEBUG   << "msg";         // "(debug) [channel] msg"
//  LOG_TRACE   << "msg";         // "(trace) [channel] msg"

// Logging macros:
//  LOG_DEBUG_I("msg");           // "(debug) [channel] (I) -- msg";
//  LOG_DEBUG_T("msg");           // "(debug) [channel] (T) -- msg";
//  LOG_DEBUG_I_F("msg", fm);     // "(debug) [channel] (I) msg " << TAU_TO_STR(fm);
//  LOG_DEBUG_I_F_DUMP("msg", fm) // "(debug) [channel] (I) msg " << TAU_DUMP_TO_STR(fm);
//  LOG_DEBUG_F(fm);              // "(debug) [channel] (F) " << TAU_TO_STR(fm);
//  LOG_DEBUG_F_DUMP(fm);         // "(debug) [channel] (F) " << TAU_DUMP_TO_STR(fm);
//  LOG_DEBUG_RR(nso_rr);         // "(debug) [channel] (RR) -- " << to_str<node>(fm);

//  LOG_TRACE_I("msg");           // "(trace) [channel] (I) -- msg";
//  LOG_TRACE_T("msg");           // "(trace) [channel] (T) -- msg";
//  LOG_TRACE_I_F("msg", fm);     // "(trace) [channel] (I) msg " << TAU_TO_STR(fm);
//  LOG_TRACE_I_F_DUMP("msg", fm) // "(trace) [channel] (I) msg " << TAU_DUMP_TO_STR(fm);
//  LOG_TRACE_F(fm);              // "(trace) [channel] (F) " << TAU_TO_STR(fm);
//  LOG_TRACE_F_DUMP(fm);         // "(trace) [channel] (F) " << TAU_DUMP_TO_STR(fm);
//  LOG_TRACE_RR(nso_rr);         // "(trace) [channel] (RR) -- " << to_str<node>(fm);

// Logging current file and line:
//   LOG_TRACE << LOG_LINE_PATH << "msg"; // (trace) [channel] /path/to/file.h:123 msg
//   LOG_TRACE << LOG_LINE << "msg";      // (trace) [channel] file.h:123 msg

// TODO (LOW) maybe allow to set a list of enabled channels (addition to --severity trace or debug)
// TODO (LOW) multiple channels for various warnings and allow user to filter them

#ifndef __IDNI__TAU__LOGGING_H__
#define __IDNI__TAU__LOGGING_H__

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/expressions.hpp>

namespace idni::tau_lang {

// uncomment to compile in logging for hooks
#define HOOK_LOGGING_ENABLED 1
// uncomment to compile in logging for pretty printer
// #define PRETTY_PRINTER_LOGGING_ENABLED 1

// uncomment to print file paths with a line number in the log
// #define TAU_LOG_LINE_PATHS
// uncomment to print file names with a line number in the log
// #define TAU_LOG_LINES

// list of enabled channels for TRACE and DEBUG messages
// INFO, WARNING and ERROR messages are not filtered by channel
// comment or uncomment as desired
static constexpr const char* LOG_ENABLED_CHANNELS [] = {
	"nso_ba",
	"sbf_ba",
	"tau_ba",
	"ba_types",
	"ba_types_inference",
	"ba_constants",
	"ba_constants_binder",
	"execution",
	"hooks",
	"interpreter",
	"normal_forms",
	"assign_and_reduce",
	"reduce_deprecated",
	"to_snf",
	"normalizer",
	"nso_rr",
	"ref_types_inference",
	"repl_evaluator",
	"satisfiability",
	"solver",
	"splitter",
	"tau_tree",
	"builders",
	"extractors",
	"from_parser",
	"node",
	"printers",
	"queries",
	"traverser",
	"types",
};

// -----------------------------------------------------------------------------
// Logging channels

// default channel name
#define LOG_CHANNEL_NAME "global"

// to define a channel name, redefine the LOG_CHANNEL_NAME macro.
// All the following LOG_TRACE and LOG_DEBUG messages will go to this channel.

// Usual place of a channel redefinition is at a .tmpl.h file right after all
// includes.
// It can be also around some part of a code.
// If other channel code follows, it has to be redefined to the proper channel.

// Channel name redefinition:
//   #undef  LOG_CHANNEL_NAME
//   #define LOG_CHANNEL_NAME "a_channel_name"

// -----------------------------------------------------------------------------

// Configure the value of a printed file (w or w/o paths) and line number printing in trace and debug messages
#ifdef TAU_LOG_LINE_PATHS
#	define LOG_LINE_VALUE         << LOG_LINE_PATH << "\t"
#else
#	ifdef TAU_LOG_LINES
#		define LOG_LINE_VALUE << LOG_LINE << "\t"
#	else
#		define LOG_LINE_VALUE // empty
#	endif
#endif

// Logging stream for error messages. Prepends message with "(Error) "
#define LOG_ERROR         BOOST_LOG_TRIVIAL(error)

// Logging stream for warning messages. Prepends message with "(Warning) "
#define LOG_WARNING       BOOST_LOG_TRIVIAL(warning)

// Logging stream for info messages. Doesn't prepend anything
#define LOG_INFO          BOOST_LOG_TRIVIAL(info)

// Logging stream for debug messages. Prepends message with "(debug) [channel] "
// locally defined LOG_CHANNEL_NAME has to be contained in the list of enabled channels
#define LOG_DEBUG         BOOST_LOG_STREAM_SEV( \
				logging::get_channel_logger(LOG_CHANNEL_NAME), \
				boost::log::trivial::debug) LOG_LINE_VALUE
// Logging stream for trace messages. Prepends message with "(trace) [channel] "
// locally defined LOG_CHANNEL_NAME has to be contained in the list of enabled channels
#define LOG_TRACE         BOOST_LOG_STREAM_SEV( \
				logging::get_channel_logger(LOG_CHANNEL_NAME), \
				boost::log::trivial::trace) LOG_LINE_VALUE

// Logging helper macros

// LOG_TRACE << LOG_LINE_PATH << "message";
#define LOG_LINE_PATH            __FILE__      << ":" << __LINE__ << " "
// LOG_TRACE << LOG_LINE_PATH << "message";
#define LOG_LINE                 __FILE_NAME__ << ":" << __LINE__ << " "

// info: (I) -- message
#define LOG_TRACE_I(m)           LOG_TRACE << "(I) -- " << m
#define LOG_DEBUG_I(m)           LOG_DEBUG << "(I) -- " << m

// info: (I) -- message with a formula
#define LOG_TRACE_I_F(m, f)      LOG_TRACE << "(I) -- " <<m<< " "<<TAU_TO_STR(f)
#define LOG_DEBUG_I_F(m, f)      LOG_DEBUG << "(I) -- " <<m<< " "<<TAU_TO_STR(f)

// info: (I) -- message with a formula dump
#define LOG_TRACE_I_F_DUMP(m, f) LOG_TRACE << "(I) -- " <<m<< " "<<TAU_TO_STR(f)
#define LOG_DEBUG_I_F_DUMP(m, f) LOG_DEBUG << "(I) -- " <<m<< " "<<TAU_TO_STR(f)

// type inference: (T) -- message
#define LOG_TRACE_T(m)           LOG_TRACE << "(T) -- " << m
#define LOG_DEBUG_T(m)           LOG_DEBUG << "(T) -- " << m

// formula: (F) formula
#define LOG_TRACE_F(f)           LOG_TRACE << "(F) " << TAU_TO_STR(f)
#define LOG_DEBUG_F(f)           LOG_DEBUG << "(F) " << TAU_TO_STR(f)

// formula: (F) formula dump
#define LOG_TRACE_F_DUMP(f)      LOG_TRACE << "(F) " << TAU_DUMP_TO_STR(f)
#define LOG_DEBUG_F_DUMP(f)      LOG_DEBUG << "(F) " << TAU_DUMP_TO_STR(f)

// formula: (F) -- message with a formula
#define LOG_TRACE_F_F(m, f)      LOG_TRACE << "(F) -- "<<m<<" "<<TAU_TO_STR(f)
#define LOG_DEBUG_F_F(m, f)      LOG_DEBUG << "(F) -- "<<m<<" "<<TAU_TO_STR(f)

// formula: (F) -- messages with a formula
#define LOG_TRACE_F_F_DUMP(m, f) LOG_TRACE<<"(F) -- "<<m<<" "<<TAU_DUMP_TO_STR(f)
#define LOG_DEBUG_F_F_DUMP(m, f) LOG_DEBUG<<"(F) -- "<<m<<" "<<TAU_DUMP_TO_STR(f)

// spec/rrs: (RR) -- specs
#define LOG_TRACE_RR(r)          LOG_TRACE << "(RR) -- " << to_str<node>(r)
#define LOG_DEBUG_RR(r)          LOG_DEBUG << "(RR) -- " << to_str<node>(r)


// -----------------------------------------------------------------------------

BOOST_LOG_ATTRIBUTE_KEYWORD(channel_attr, "Channel", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity,
				"Severity", boost::log::trivial::severity_level)

struct logging {
	// set filter to trace level
	inline static void trace()  { set_filter(boost::log::trivial::trace);  }
	// set filter to debug level
	inline static void debug()  { set_filter(boost::log::trivial::debug);  }
	// set filter to print info, warning and error messages
	inline static void info()   { set_filter(boost::log::trivial::info);   }
	// set filter to print warning and error messages
	inline static void warning(){ set_filter(boost::log::trivial::warning);}
	// set filter to print error messages
	inline static void error()  { set_filter(boost::log::trivial::error);  }

	// set filter to a specific level
	inline static void set_filter(boost::log::trivial::severity_level level)
	{
		using namespace boost::log;
		core::get()->set_filter([level](const attribute_value_set& rec){
			auto sev = rec[trivial::severity];
			if (!sev || *sev < level) return false;
			auto ch = rec[channel_attr];
			if (level != trivial::trace && level != trivial::debug)
				return true;
			if (!ch || *ch == "global") return true;
			for (auto& channel : LOG_ENABLED_CHANNELS)
				if (channel == *ch) return true;
			return false;
		});
	}

	// initialize logging
	inline static bool initialized = false;
	logging() {
		using namespace boost::log;

		if (initialized) return;
		initialized = true;

		add_common_attributes();

		info(); // set filter to info level

		auto formatter = [](
			const record_view& rec, formatting_ostream& os)
		{
			std::stringstream ss;
			if (auto sev = rec[trivial::severity];sev) switch (*sev)
			{
			case trivial::fatal: break; // not used
			case trivial::error:   ss << "(Error) ";   break;
			case trivial::warning: ss << "(Warning) "; break;
			case trivial::info: break; // no prefix
			case trivial::trace: // (severity) [channel] message
			case trivial::debug:
				ss << "(" << (*sev == trivial::trace
					? "Trace" : "Debug") << ") ";
				ss << "[" << (rec[channel_attr]
					? *rec[channel_attr] : "global") <<"] ";
				while (ss.tellp() < 32) ss << " "; // padding
				break;
			}
			os << ss.str() << rec[expressions::smessage];
		};
		add_console_log(std::cout, keywords::format = formatter);
 	}

	using channel_logger_type =
		boost::log::sources::severity_channel_logger_mt<
			boost::log::trivial::severity_level, std::string>;

	// get a channel logger for a specific header
	inline static channel_logger_type& get_channel_logger(
						const std::string& channel_name)
	{
		using namespace boost::log;
		static std::map<std::string, channel_logger_type> loggers;
		if (loggers.find(channel_name) == loggers.end())
			loggers.emplace(channel_name,
				channel_logger_type(
					keywords::channel = channel_name));
		return loggers[channel_name];
	}
};

// static initialization of the logging system 
inline static logging initialize_logging;

} // namespace idni::tau_lang

#endif //__IDNI__TAU__LOGGING_H__