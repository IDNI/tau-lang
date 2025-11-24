// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

// Logging streams:
//  LOG_ERROR   << "msg";         // "(Error) msg"
//  LOG_WARNING << "msg";         // "(Warning) msg"
//  LOG_INFO    << "msg";         // "msg"
//  LOG_DEBUG   << "msg";         // "(debug) [channel] msg"
//  LOG_TRACE   << "msg";         // "(trace) [channel] msg"

// Color printers for various data types
//  LOG_BRIGHT(m)     // TAU_LOG_BRIGHT_COLOR << m << TC.CLEAR()
//  LOG_NT(nt)        // TAU_LOG_NT_COLOR << node::name(nt)             <<TC.CLEAR()
//  LOG_RULE(r)       // TAU_LOG_RULE_COLOR << to_str<node>(r)          <<TC.CLEAR()
//  LOG_BA(c)         // TAU_LOG_BA_COLOR << c                          <<TC.CLEAR()
//  LOG_BA_TYPE(tid)  // TAU_LOG_BA_COLOR << get_ba_type_name<node>(tid)<<TC.CLEAR()
//  LOG_FM(fm)        // TAU_LOG_FM_COLOR << TAU_TO_STR(fm)             <<TC.CLEAR()
//  LOG_FM_DUMP(fm)   // TAU_LOG_FM_COLOR << TAU_DUMP_TO_STR(fm)        <<TC.CLEAR()
//  LOG_RR(nso_rr)    // TAU_LOG_FM_COLOR << to_str<node>(fm)           <<TC.CLEAR()

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

#include "utility/term_colors.h"

namespace idni::tau_lang {

// Uncomment to compile in logging for hooks.
// "hooks" must be listed in LOG_ENABLED_CHANNELS to pass the channel filter.
//
// #define HOOK_LOGGING_ENABLED

// Uncomment to compile in logging for pretty printer
// If enabled it prints information about traversing nodes.
// It prints to std::cerr to view cout and cerr in a split screen in parallel
//
// #define PRETTY_PRINTER_LOGGING_ENABLED 1

// Uncomment or use -D to print file paths with a line number in the log
//
// #define TAU_LOG_LINE_PATHS

// Uncomment or use -D to print file names with a line number in the log
//
// #define TAU_LOG_LINES

// Uncomment or use -D to enable LOG_DEBUG and LOG_TRACE messages in RELEASE.
// This enables channel filtering by a following LOG_ENABLED_CHANNELS list
// for non-DEBUG builds. This list is also used in DEBUG builds by default.
// Otherwise the list of enabled channels is empty.
//
// #define TAU_LOG_CHANNELS

// List of enabled channels for TRACE and DEBUG messages.
// They are disabled in RELEASE unless TAU_LOG_CHANNELS is defined.
//
// INFO, WARNING and ERROR messages are not filtered by channel.
//
// Comment or uncomment as desired.
//
#if defined(DEBUG) || defined(TAU_LOG_CHANNELS)
static constexpr const char* LOG_ENABLED_CHANNELS[] = {
	"nso_ba",
	"sbf_ba",
	"tau_ba",
	"bv_ba",
	"ba_types",
	"ba_constants",
	// "ba_types_inference",
	// "batinf_scoping",
	// "batinf_propagation",
	// "batinf_unscoping",
	"execution",
	"hooks",
	"interpreter",
	"normal_forms",
	"assign_and_reduce",
	"anti_prenex",
	"normalizer",
	"nso_rr",
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
	"testing",
	"resolver"
};
#else // #if defined(DEBUG) || defined(TAU_LOG_CHANNELS) else
static constexpr const char* LOG_ENABLED_CHANNELS[] = { "" };
#endif // #if defined(DEBUG) || defined(TAU_LOG_CHANNELS)
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
#	define TAU_LOG_LINE_VALUE         << TAU_LOG_LINE_PATH << "\t"
#else
#	ifdef TAU_LOG_LINES
#		define TAU_LOG_LINE_VALUE << TAU_LOG_LINE << "\t"
#	else
#		define TAU_LOG_LINE_VALUE // empty
#	endif
#endif

// Logging stream for error messages. Prepends message with "(Error) "
#define TAU_LOG_ERROR     BOOST_LOG_TRIVIAL(error)
#define LOG_ERROR         TAU_LOG_ERROR
// Logging stream for warning messages. Prepends message with "(Warning) "
#define TAU_LOG_WARNING   BOOST_LOG_TRIVIAL(warning)
#define LOG_WARNING       TAU_LOG_WARNING

// Logging stream for info messages. Doesn't prepend anything
#define TAU_LOG_INFO      BOOST_LOG_TRIVIAL(info)
#define LOG_INFO          TAU_LOG_INFO

// Logging stream for debug messages. Prepends message with "(debug) [channel] "
// locally defined LOG_CHANNEL_NAME has to be contained in the list of enabled channels
#define TAU_LOG_DEBUG     BOOST_LOG_STREAM_SEV( \
				logging::get_channel_logger(LOG_CHANNEL_NAME), \
				boost::log::trivial::debug) TAU_LOG_LINE_VALUE
#define LOG_DEBUG         TAU_LOG_DEBUG
// Logging stream for trace messages. Prepends message with "(trace) [channel] "
// locally defined LOG_CHANNEL_NAME has to be contained in the list of enabled channels
#define TAU_LOG_TRACE     BOOST_LOG_STREAM_SEV( \
				logging::get_channel_logger(LOG_CHANNEL_NAME), \
				boost::log::trivial::trace) TAU_LOG_LINE_VALUE
#define LOG_TRACE         TAU_LOG_TRACE

// Logging helper macros

// LOG_TRACE << LOG_LINE_PATH << "message";
#define TAU_LOG_LINE_PATH        __FILE__      << ":" << __LINE__ << " "
#define LOG_LINE_PATH            TAU_LOG_LINE_PATH
// LOG_TRACE << LOG_LINE << "message";
#define TAU_LOG_LINE             __FILE_NAME__ << ":" << __LINE__ << " "
#define LOG_LINE                 TAU_LOG_LINE

// -----------------------------------------------------------------------------
// Colors used in logging

// used for LOG_BRIGHT and pretty printed formulas, specs
#define TAU_LOG_BRIGHT_COLOR        TC(term::color::WHITE, \
					term::color::BRIGHT)
#define LOG_BRIGHT_COLOR            TAU_LOG_BRIGHT_COLOR

// color for Error messages (only the word "Error" is colored)
#define TAU_LOG_ERROR_COLOR         TC(term::color::RED, \
					term::color::BRIGHT)
#define LOG_ERROR_COLOR             TAU_LOG_ERROR_COLOR

// color for Warning messages (only the "Warning" is colored)
#define TAU_LOG_WARNING_COLOR       TC(term::color::YELLOW, \
					term::color::BRIGHT)
#define LOG_WARNING_COLOR           TAU_LOG_WARNING_COLOR
// color for logging channel name
#define TAU_LOG_CHANNEL_COLOR       TC(term::color::CYAN)
#define LOG_CHANNEL_COLOR           TAU_LOG_CHANNEL_COLOR

// color for formulas and specs
#define TAU_LOG_FM_COLOR            TAU_LOG_BRIGHT_COLOR
#define LOG_FM_COLOR                TAU_LOG_FM_COLOR

// color for nonterminal / node types
#define TAU_LOG_NT_COLOR            TC(term::color::GREEN)
#define LOG_NT_COLOR                TAU_LOG_NT_COLOR

// color for constants and ba types
#define TAU_LOG_BA_COLOR            TC(term::color::CYAN)
#define LOG_BA_COLOR                TAU_LOG_BA_COLOR

// color for rules
#define TAU_LOG_RULE_COLOR          TC(term::color::YELLOW)
#define LOG_RULE_COLOR              TAU_LOG_RULE_COLOR

// -----------------------------------------------------------------------------
// Logging helper macros for various types to use appropriate colors
// They have to be used with `ostream`, ie. after `<<`

// LOG_BRIGHT_COLOR escapes in a stream for any element we want to make bright
#define TAU_LOG_BRIGHT(m)    TAU_LOG_BRIGHT_COLOR << m              <<TC.CLEAR()
#define LOG_BRIGHT(m)        TAU_LOG_BRIGHT(m)

// LOG_NT_COLOR escapes in a stream for `size_t` or `node::type`
//                                               or `tau_parser::nonterminal`
#define TAU_LOG_NT(nt)       TAU_LOG_NT_COLOR << node::name(nt)     <<TC.CLEAR()
#define LOG_NT(nt)           TAU_LOG_NT(nt)

// LOG_NT_COLOR escapes in a stream for `rr_sig` (recurrence relation signature)
#define TAU_LOG_RR_SIG(sig)  TAU_LOG_NT_COLOR << sig                <<TC.CLEAR()
#define LOG_RR_SIG(sig)      TAU_LOG_RR_SIG(sig)

// LOG_RULE_COLOR escapes in a stream for rewriter or recurrence relation def
// requires type node alias to be defined
#define TAU_LOG_RULE(r)      TAU_LOG_RULE_COLOR << to_str<node>(r)  <<TC.CLEAR()
#define LOG_RULE(r)          TAU_LOG_RULE(r)

// LOG_BA_COLOR escapes in a stream for `size_t` BA type id
#define TAU_LOG_BA_TYPE(tid) TAU_LOG_BA_COLOR << ba_types<node>::name(tid) \
								    <<TC.CLEAR()
#define LOG_BA_TYPE(tid)     TAU_LOG_BA_TYPE(tid)

// LOG_BA_COLOR escapes in a stream for `size_t` BA type id with the id
#define TAU_LOG_BA_TYPE_DUMP(tid) TAU_LOG_BA_TYPE(tid) << "(" << tid << ")"
#define LOG_BA_TYPE_DUMP(tid)     TAU_LOG_BA_TYPE_DUMP(tid)

// LOG_BA_COLOR escapes in a stream (constants or other elements representing ba constants or ba types)
#define TAU_LOG_BA(c)        TAU_LOG_BA_COLOR << c                  <<TC.CLEAR()
#define LOG_BA(c)            TAU_LOG_BA(c)

// LOG_FM_COLOR escapes in a stream for `tref` or `htref` pretty print
#define TAU_LOG_FM(fm)       TAU_LOG_FM_COLOR << tree<node>::get(fm).to_str() \
								    <<TC.CLEAR()
#define LOG_FM(fm)           TAU_LOG_FM(fm)

// LOG_FM_COLOR escapes in a stream for `tref` or `htref` pretty print
//                      and then followed by nodes tree printed in a line
#define TAU_LOG_FM_DUMP(fm)  TAU_LOG_FM_COLOR <<tree<node>::get(fm).to_str() \
	<< TC.CLEAR() << " \t#\t" << tree<node>::get(fm).print_in_line_to_str()
#define LOG_FM_DUMP(fm)      TAU_LOG_FM_DUMP(fm)

// LOG_FM_COLOR escapes in a stream for `tref` or `htref` tree print
#define TAU_LOG_FM_TREE(fm)  TAU_LOG_FM_COLOR<<tree<node>::get(fm).tree_to_str() \
								    <<TC.CLEAR()
#define LOG_FM_TREE(fm)      TAU_LOG_FM_TREE(fm)

// LOG_FM_COLOR escapes in a stream for `rr` (recurrence relation)
#define TAU_LOG_RR(nso_rr)   TAU_LOG_FM_COLOR <<to_str<node>(nso_rr)<<TC.CLEAR()
#define LOG_RR(nso_rr)       TAU_LOG_RR(nso_rr)

// LOG_FM_COLOR escapes in a stream for `rr` (recurrence relation)
#define TAU_LOG_RR_DUMP(nso_rr)  TAU_LOG_FM_COLOR<<dump_to_str<node>(nso_rr) \
								    <<TC.CLEAR()
#define LOG_RR_DUMP(nso_rr)      TAU_LOG_RR_DUMP(nso_rr)

// LOG_SPLITTER adds a LOG_WARNING_COLOR escaped dash separater
#define TAU_LOG_SPLITTER     TAU_LOG_WARNING_COLOR<<"-------------------------"\
	"----------------------------------------------------------"<<TC.CLEAR()
#define LOG_SPLITTER         TAU_LOG_SPLITTER

// outputs tabs to pad up to the trace/debug messages beginning for new lines in log msgs
#define TAU_LOG_PADDING      "\t\t\t\t"
#define LOG_PADDING          TAU_LOG_PADDING

// outputs padding + one tab indentation for indenting of new lines in log msgs
#define TAU_LOG_INDENT       TAU_LOG_PADDING << "\t"
#define LOG_INDENT           TAU_LOG_INDENT
#define TAU_LOG_INDENT1      TAU_LOG_INDENT
#define LOG_INDENT1          TAU_LOG_INDENT
#define TAU_LOG_INDENT2      TAU_LOG_INDENT << "\t"
#define LOG_INDENT2          TAU_LOG_INDENT2

// -----------------------------------------------------------------------------

BOOST_LOG_ATTRIBUTE_KEYWORD(channel_attr, "Channel", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity,     "Severity",
			    boost::log::trivial::severity_level)

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

	inline static boost::log::trivial::severity_level level() {
		return set_level;
	}

	// set filter to a specific level
	inline static void set_filter(boost::log::trivial::severity_level level)
	{
		using namespace boost::log;
		set_level = level;
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
			case trivial::error:
				ss << "(" << LOG_ERROR_COLOR << "Error"
					<< TC.CLEAR()<<") "; break;
			case trivial::warning:
				ss << "(" << LOG_WARNING_COLOR << "Warning"
					<< TC.CLEAR()<<") "; break;
			case trivial::info: break; // no prefix
			case trivial::trace: // (severity) [channel] message
			case trivial::debug:
			{
				ss << "(" << LOG_BRIGHT((*sev == trivial::trace
						? "Trace" : "Debug")) << ") ";
				std::string ch = rec[channel_attr]
						? *rec[channel_attr] : "global";
				ss << "[" << LOG_CHANNEL_COLOR << ch
					<< TC.CLEAR() << "] ";
				size_t padding = ch.size() < 21
							? 21 - ch.size() : 0;
				for (size_t i = 0; i < padding; ++i) ss << " ";
			} break;
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

private:
	inline static boost::log::trivial::severity_level set_level;
};

// static initialization of the logging system
inline static logging initialize_logging;

} // namespace idni::tau_lang

#endif //__IDNI__TAU__LOGGING_H__