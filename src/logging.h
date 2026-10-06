// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file logging.h
 * @brief Boost.Log-based logging infrastructure and helper macros for Tau.
 *
 * Defines `LOG_ERROR`, `LOG_WARNING`, `LOG_INFO`, `LOG_DEBUG`, `LOG_TRACE`
 * stream macros, color-escape helpers (`LOG_BRIGHT`, `LOG_FM`, etc.),
 * the `logging` singleton that configures the Boost.Log core, and
 * `LOG_ENABLED_CHANNELS` for compile-time channel filtering.
 */

// Logging streams:
//  LOG_ERROR   << "msg";         // "(Error) msg"   on std::cerr
//  LOG_WARNING << "msg";         // "(Warning) msg" on std::cerr
//  LOG_INFO    << "msg";         // "msg"           on std::cout
//  LOG_DEBUG   << "msg";         // "(Debug) [channel] msg" on std::cout
//  LOG_TRACE   << "msg";         // "(Trace) [channel] msg" on std::cout

// Color printers for various data types
//  LOG_BRIGHT(m)     // TAU_LOG_BRIGHT_COLOR << m << TC.CLEAR()
//  LOG_NT(nt)        // TAU_LOG_NT_COLOR << node::name(nt)             <<TC.CLEAR()
//  LOG_RULE(r)       // TAU_LOG_RULE_COLOR << to_str<node>(r)          <<TC.CLEAR()
//  LOG_BA(c)         // TAU_LOG_BA_COLOR << c                          <<TC.CLEAR()
//  LOG_BA_TYPE(tid)  // TAU_LOG_BA_COLOR << get_ba_type_name<node>(tid)<<TC.CLEAR()
//  LOG_FM(fm)        // TAU_LOG_FM_COLOR << TAU_TO_STR(fm)             <<TC.CLEAR()
//  LOG_FM_DUMP(fm)   // TAU_LOG_FM_COLOR << TAU_DUMP_TO_STR(fm)        <<TC.CLEAR()
//  LOG_RR(nso_rr)    // TAU_LOG_FM_COLOR << to_str<node>(nso_rr)       <<TC.CLEAR()

// Logging current file and line:
//   LOG_TRACE << LOG_LINE_PATH << "msg"; // (Trace) [channel] /path/to/file.h:123 msg
//   LOG_TRACE << LOG_LINE << "msg";      // (Trace) [channel] file.h:123 msg

// TODO (LOW) maybe allow to set a list of enabled channels (addition to --severity trace or debug)
// TODO (LOW) multiple channels for various warnings and allow user to filter them

#ifndef __IDNI__TAU__LOGGING_H__
#define __IDNI__TAU__LOGGING_H__

#include <unordered_map>

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

// Uncomment or use -D to enable LOG_DEBUG and LOG_TRACE messages of named
// channels in RELEASE. This enables channel filtering by a following
// LOG_ENABLED_CHANNELS list for non-DEBUG builds. This list is also used in
// DEBUG builds by default. Otherwise the list holds no channel, so only the
// "global" channel's debug and trace messages pass.
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
/// @brief Channels whose debug and trace messages pass the filter.
static constexpr const char* LOG_ENABLED_CHANNELS[] = {


	/* main components*/
	// "execution",
	//"interpreter",
	// "hooks",
	// "normal_forms",
	// "assign_and_reduce",
	// "anti_prenex",
	//"normalizer",
	//"nso_rr",
	// "repl_evaluator",
	// "satisfiability",
	//"solver",
	// "splitter",
	// "tau_tree",
	// "tau_bdd",
	// "builders",
	// "extractors",
	// "from_parser",
	// "node",
	// "printers",
	// "queries",
	// "traverser",
	// "api",
	// "io_context",
	// "tau_spec",
	// "rr",

	/* related top types and type inference*/
	// "ba_types",
	// "ba_constants",
	// "resolver",
	// "inference",

	/* related to heuristics*/
	// "bv_ba_simplification",
	// "ex_subs_based_elimination",
	// "bv_predicate_blasting",
	// "syntactic_path_simplification",
	// "simplify_using_equality",

	/* testing specific */
	"testing",
};
#else // #if defined(DEBUG) || defined(TAU_LOG_CHANNELS) else
/// @brief No channel enabled: only "global" debug and trace messages pass.
static constexpr const char* LOG_ENABLED_CHANNELS[] = { "" };
#endif // #if defined(DEBUG) || defined(TAU_LOG_CHANNELS)
// -----------------------------------------------------------------------------
// Logging channels

/// @brief Channel of the LOG_DEBUG and LOG_TRACE messages that follow
/// (default "global").
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

/// @brief File (with or without path) and line prefix of trace and debug
/// messages, per TAU_LOG_LINE_PATHS / TAU_LOG_LINES; empty by default.
#ifdef TAU_LOG_LINE_PATHS
#	define TAU_LOG_LINE_VALUE         << TAU_LOG_LINE_PATH << "\t"
#else
#	ifdef TAU_LOG_LINES
#		define TAU_LOG_LINE_VALUE << TAU_LOG_LINE << "\t"
#	else
#		define TAU_LOG_LINE_VALUE // empty
#	endif
#endif

/// @brief Logging stream for error messages, on std::cerr. Prepends
/// message with "(Error) ".
#define TAU_LOG_ERROR     BOOST_LOG_TRIVIAL(error)
/// @brief Alias of `TAU_LOG_ERROR`.
#define LOG_ERROR         TAU_LOG_ERROR
/// @brief Logging stream for warning messages, on std::cerr. Prepends
/// message with "(Warning) ".
#define TAU_LOG_WARNING   BOOST_LOG_TRIVIAL(warning)
/// @brief Alias of `TAU_LOG_WARNING`.
#define LOG_WARNING       TAU_LOG_WARNING

/// @brief Logging stream for info messages, on std::cout. Doesn't prepend
/// anything.
#define TAU_LOG_INFO      BOOST_LOG_TRIVIAL(info)
/// @brief Alias of `TAU_LOG_INFO`.
#define LOG_INFO          TAU_LOG_INFO

/// @brief Logging stream for debug messages. Prepends message with
/// "(Debug) [channel] " padded to a common width; the locally defined
/// LOG_CHANNEL_NAME has to be "global" or in LOG_ENABLED_CHANNELS.
//
// The severity pre-check makes a filtered-out site cost one branch:
// without it, Boost's open_record runs its attribute-registry lookups on
// every call before the filter rejects the record, which profiling caught
// on hot paths. The `for` guard stays a single statement (usable under an
// unbraced `if`) without tripping -Wdangling-else at call sites.
#define TAU_LOG_DEBUG     for (bool _tau_log_gate = logging::level() \
					<= boost::log::trivial::debug; \
				_tau_log_gate; _tau_log_gate = false) \
			  BOOST_LOG_STREAM_SEV( \
				logging::get_channel_logger(LOG_CHANNEL_NAME), \
				boost::log::trivial::debug) TAU_LOG_LINE_VALUE
/// @brief Alias of `TAU_LOG_DEBUG`.
#define LOG_DEBUG         TAU_LOG_DEBUG
/// @brief Logging stream for trace messages. Prepends message with
/// "(Trace) [channel] " padded to a common width; the locally defined
/// LOG_CHANNEL_NAME has to be "global" or in LOG_ENABLED_CHANNELS.
#define TAU_LOG_TRACE     for (bool _tau_log_gate = logging::level() \
					<= boost::log::trivial::trace; \
				_tau_log_gate; _tau_log_gate = false) \
			  BOOST_LOG_STREAM_SEV( \
				logging::get_channel_logger(LOG_CHANNEL_NAME), \
				boost::log::trivial::trace) TAU_LOG_LINE_VALUE
/// @brief Alias of `TAU_LOG_TRACE`.
#define LOG_TRACE         TAU_LOG_TRACE

// Logging helper macros

/// @brief Streams "<path>:<line> "; use as `LOG_TRACE << LOG_LINE_PATH << "message";`.
#define TAU_LOG_LINE_PATH        __FILE__      << ":" << __LINE__ << " "
/// @brief Alias of `TAU_LOG_LINE_PATH`.
#define LOG_LINE_PATH            TAU_LOG_LINE_PATH
/// @brief Streams "<file>:<line> "; use as `LOG_TRACE << LOG_LINE << "message";`.
// cl.exe has no __FILE_NAME__, so it prints the full path instead.
#if defined(_MSC_VER)
#define TAU_LOG_LINE             __FILE__ << ":" << __LINE__ << " "
#else
#define TAU_LOG_LINE             __FILE_NAME__ << ":" << __LINE__ << " "
#endif
/// @brief Alias of `TAU_LOG_LINE`.
#define LOG_LINE                 TAU_LOG_LINE

// -----------------------------------------------------------------------------
// Colors used in logging

/// @brief Color used for LOG_BRIGHT and pretty printed formulas, specs.
#define TAU_LOG_BRIGHT_COLOR        TC(term::color::WHITE, \
					term::color::BRIGHT)
/// @brief Alias of `TAU_LOG_BRIGHT_COLOR`.
#define LOG_BRIGHT_COLOR            TAU_LOG_BRIGHT_COLOR

/// @brief Color for Error messages (only the word "Error" is colored).
#define TAU_LOG_ERROR_COLOR         TC(term::color::RED, \
					term::color::BRIGHT)
/// @brief Alias of `TAU_LOG_ERROR_COLOR`.
#define LOG_ERROR_COLOR             TAU_LOG_ERROR_COLOR

/// @brief Color for Warning messages (only the "Warning" is colored).
#define TAU_LOG_WARNING_COLOR       TC(term::color::YELLOW, \
					term::color::BRIGHT)
/// @brief Alias of `TAU_LOG_WARNING_COLOR`.
#define LOG_WARNING_COLOR           TAU_LOG_WARNING_COLOR
/// @brief Color for logging channel name.
#define TAU_LOG_CHANNEL_COLOR       TC(term::color::CYAN)
/// @brief Alias of `TAU_LOG_CHANNEL_COLOR`.
#define LOG_CHANNEL_COLOR           TAU_LOG_CHANNEL_COLOR

/// @brief Color for formulas and specs.
#define TAU_LOG_FM_COLOR            TAU_LOG_BRIGHT_COLOR
/// @brief Alias of `TAU_LOG_FM_COLOR`.
#define LOG_FM_COLOR                TAU_LOG_FM_COLOR

/// @brief Color for nonterminal / node types.
#define TAU_LOG_NT_COLOR            TC(term::color::GREEN)
/// @brief Alias of `TAU_LOG_NT_COLOR`.
#define LOG_NT_COLOR                TAU_LOG_NT_COLOR

/// @brief Color for constants and ba types.
#define TAU_LOG_BA_COLOR            TC(term::color::CYAN)
/// @brief Alias of `TAU_LOG_BA_COLOR`.
#define LOG_BA_COLOR                TAU_LOG_BA_COLOR

/// @brief Color for rules.
#define TAU_LOG_RULE_COLOR          TC(term::color::YELLOW)
/// @brief Alias of `TAU_LOG_RULE_COLOR`.
#define LOG_RULE_COLOR              TAU_LOG_RULE_COLOR

// -----------------------------------------------------------------------------
// Logging helper macros for various types to use appropriate colors
// They have to be used with `ostream`, ie. after `<<`

/// @brief LOG_BRIGHT_COLOR escapes in a stream for any element we want to make bright.
#define TAU_LOG_BRIGHT(m)    TAU_LOG_BRIGHT_COLOR << m              <<TC.CLEAR()
/// @brief Alias of `TAU_LOG_BRIGHT`.
#define LOG_BRIGHT(m)        TAU_LOG_BRIGHT(m)

/// @brief LOG_NT_COLOR escapes in a stream for `size_t` or `node::type`
///                                               or `tau_parser::nonterminal`.
#define TAU_LOG_NT(nt)       TAU_LOG_NT_COLOR << node::name(nt)     <<TC.CLEAR()
/// @brief Alias of `TAU_LOG_NT`.
#define LOG_NT(nt)           TAU_LOG_NT(nt)

/// @brief LOG_NT_COLOR escapes in a stream for `rr_sig` (recurrence relation signature).
#define TAU_LOG_RR_SIG(sig)  TAU_LOG_NT_COLOR << sig                <<TC.CLEAR()
/// @brief Alias of `TAU_LOG_RR_SIG`.
#define LOG_RR_SIG(sig)      TAU_LOG_RR_SIG(sig)

/// @brief LOG_RULE_COLOR escapes in a stream for rewriter or recurrence
/// relation def; requires type node alias to be defined.
#define TAU_LOG_RULE(r)      TAU_LOG_RULE_COLOR << to_str<node>(r)  <<TC.CLEAR()
/// @brief Alias of `TAU_LOG_RULE`.
#define LOG_RULE(r)          TAU_LOG_RULE(r)

/// @brief LOG_BA_COLOR escapes in a stream for `size_t` BA type id; an
/// unknown id prints "INVALID".
// TODO (HIGH) dropped error: ba_types::name's report -- an ostream `<<` chain cannot abort the line.
#define TAU_LOG_BA_TYPE(tid) TAU_LOG_BA_COLOR << [](size_t bid) { \
		auto nm = ba_types<node>::name(bid); \
		return nm.has_value() ? nm.value() : std::string("INVALID"); \
	}(tid) <<TC.CLEAR()
/// @brief Alias of `TAU_LOG_BA_TYPE`.
#define LOG_BA_TYPE(tid)     TAU_LOG_BA_TYPE(tid)

/// @brief LOG_BA_COLOR escapes in a stream for `size_t` BA type id with the id.
#define TAU_LOG_BA_TYPE_DUMP(tid) TAU_LOG_BA_TYPE(tid) << "(" << tid << ")"
/// @brief Alias of `TAU_LOG_BA_TYPE_DUMP`.
#define LOG_BA_TYPE_DUMP(tid)     TAU_LOG_BA_TYPE_DUMP(tid)

/// @brief LOG_BA_COLOR escapes in a stream (constants or other elements representing ba constants or ba types).
#define TAU_LOG_BA(c)        TAU_LOG_BA_COLOR << c                  <<TC.CLEAR()
/// @brief Alias of `TAU_LOG_BA`.
#define LOG_BA(c)            TAU_LOG_BA(c)

/// @brief LOG_FM_COLOR escapes in a stream for `tref` or `htref` pretty print.
#define TAU_LOG_FM(fm)       TAU_LOG_FM_COLOR << tree<node>::get(fm).to_str() \
								    <<TC.CLEAR()
/// @brief Alias of `TAU_LOG_FM`.
#define LOG_FM(fm)           TAU_LOG_FM(fm)

/// @brief LOG_FM_COLOR escapes in a stream for `tref` or `htref` pretty print
///                      and then followed by nodes tree printed in a line.
#define TAU_LOG_FM_DUMP(fm)  TAU_LOG_FM_COLOR <<tree<node>::get(fm).to_str() \
	<< TC.CLEAR() << " \t#\t" << tree<node>::get(fm).print_in_line_to_str()
/// @brief Alias of `TAU_LOG_FM_DUMP`.
#define LOG_FM_DUMP(fm)      TAU_LOG_FM_DUMP(fm)

/// @brief LOG_FM_COLOR escapes in a stream for `tref` or `htref` tree print.
#define TAU_LOG_FM_TREE(fm)  TAU_LOG_FM_COLOR<<tree<node>::get(fm).tree_to_str() \
								    <<TC.CLEAR()
/// @brief Alias of `TAU_LOG_FM_TREE`.
#define LOG_FM_TREE(fm)      TAU_LOG_FM_TREE(fm)

/// @brief LOG_FM_COLOR escapes in a stream for `rr` (recurrence relation).
#define TAU_LOG_RR(nso_rr)   TAU_LOG_FM_COLOR <<to_str<node>(nso_rr)<<TC.CLEAR()
/// @brief Alias of `TAU_LOG_RR`.
#define LOG_RR(nso_rr)       TAU_LOG_RR(nso_rr)

/// @brief LOG_FM_COLOR escapes in a stream for `rr` (recurrence relation), dumped.
#define TAU_LOG_RR_DUMP(nso_rr)  TAU_LOG_FM_COLOR<<dump_to_str<node>(nso_rr) \
								    <<TC.CLEAR()
/// @brief Alias of `TAU_LOG_RR_DUMP`.
#define LOG_RR_DUMP(nso_rr)      TAU_LOG_RR_DUMP(nso_rr)

/// @brief LOG_SPLITTER adds a LOG_WARNING_COLOR escaped dash separator.
#define TAU_LOG_SPLITTER     TAU_LOG_WARNING_COLOR<<"-------------------------"\
	"----------------------------------------------------------"<<TC.CLEAR()
/// @brief Alias of `TAU_LOG_SPLITTER`.
#define LOG_SPLITTER         TAU_LOG_SPLITTER

/// @brief Outputs tabs to pad up to the trace/debug messages beginning for new lines in log msgs.
#define TAU_LOG_PADDING      "\t\t\t\t"
/// @brief Alias of `TAU_LOG_PADDING`.
#define LOG_PADDING          TAU_LOG_PADDING

/// @brief Outputs padding + one tab indentation for indenting of new lines in log msgs.
#define TAU_LOG_INDENT       TAU_LOG_PADDING << "\t"
/// @brief Alias of `TAU_LOG_INDENT`.
#define LOG_INDENT           TAU_LOG_INDENT
/// @brief Alias of `TAU_LOG_INDENT` (one level of indentation).
#define TAU_LOG_INDENT1      TAU_LOG_INDENT
/// @brief Alias of `TAU_LOG_INDENT`.
#define LOG_INDENT1          TAU_LOG_INDENT
/// @brief Padding + two tabs of indentation for new lines in log msgs.
#define TAU_LOG_INDENT2      TAU_LOG_INDENT << "\t"
/// @brief Alias of `TAU_LOG_INDENT2`.
#define LOG_INDENT2          TAU_LOG_INDENT2

// -----------------------------------------------------------------------------

/// @brief Boost.Log keyword for the "Channel" attribute of a record.
BOOST_LOG_ATTRIBUTE_KEYWORD(channel_attr, "Channel", std::string)
/// @brief Boost.Log keyword for the "Severity" attribute of a record.
BOOST_LOG_ATTRIBUTE_KEYWORD(severity,     "Severity",
			    boost::log::trivial::severity_level)

/**
 * @brief Singleton that configures and owns the Boost.Log core for Tau.
 *
 * Constructed once at program start via `initialize_logging`. After
 * construction the default severity is `info`. Call `trace()` / `debug()` etc.
 * to lower the threshold dynamically.
 */
struct logging {
	/** @brief Set log filter to `trace` (most verbose). */
	inline static void trace()  { set_filter(boost::log::trivial::trace);  }
	/** @brief Set log filter to `debug`. */
	inline static void debug()  { set_filter(boost::log::trivial::debug);  }
	/** @brief Set log filter to `info` (info, warning, error). */
	inline static void info()   { set_filter(boost::log::trivial::info);   }
	/** @brief Set log filter to `warning` (warning + error only). */
	inline static void warning(){ set_filter(boost::log::trivial::warning);}
	/** @brief Set log filter to `error` (errors only). */
	inline static void error()  { set_filter(boost::log::trivial::error);  }

	/** @brief Return the current active severity threshold. */
	inline static boost::log::trivial::severity_level level() {
		return set_level;
	}

	/**
	 * @brief Apply severity @p level as the active filter for the Boost.Log core.
	 * 
	 * Sets a global severity level filter and records @p level for `level()`.
	 * Per-channel filtering applies only while the threshold is trace or
	 * debug: then a record of a named channel passes only when the channel is
	 * in the compile-time LOG_ENABLED_CHANNELS list, while the "global"
	 * channel and records with no channel always pass. Above debug,
	 * filtering is by severity only.
	 * 
	 * @param level Minimum severity level to log (trace, debug, info, warning, error, fatal)
	 */
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

	/// @brief true once the Boost.Log core has been configured.
	inline static bool initialized = false;
	/**
	 * @brief One-time Boost.Log setup (subsequent constructions no-op).
	 *
	 * Adds the common attributes, applies the default info-level filter
	 * (see set_filter) and installs two console sinks, std::cout for trace,
	 * debug and info and std::cerr for warning and above, whose formatter
	 * prefixes records by severity: colored "(Error)" / "(Warning)" tags, a
	 * "(Trace)"/"(Debug)" tag plus a padded [channel] label, and no prefix
	 * for info.
	 */
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
		// Warnings and errors are diagnostics, not program output: route
		// them to stderr so a failure is visible even when stdout is
		// captured/redirected, and so the process's observable output
		// stream stays clean.
		add_console_log(std::cout, keywords::format = formatter,
			keywords::filter = severity < trivial::warning);
		add_console_log(std::cerr, keywords::format = formatter,
			keywords::filter = severity >= trivial::warning);
 	}

	/// @brief Severity-and-channel logger, thread-safe unless Boost.Log is
	/// built without threads.
#ifdef BOOST_LOG_NO_THREADS
	using channel_logger_type =
		boost::log::sources::severity_channel_logger<
			boost::log::trivial::severity_level, std::string>;
#else // BOOST_LOG_NO_THREADS
	using channel_logger_type =
		boost::log::sources::severity_channel_logger_mt<
			boost::log::trivial::severity_level, std::string>;
#endif // BOOST_LOG_NO_THREADS

	/**
	 * @brief Return (creating if needed) the per-channel logger for @p channel_name.
	 *
	 * The loggers live in a function-local static map for the life of the
	 * process; the returned reference stays valid.
	 */
	inline static channel_logger_type& get_channel_logger(
						const std::string& channel_name)
	{
		using namespace boost::log;
		static std::unordered_map<std::string, channel_logger_type> loggers;
		// One lookup, not three (this runs on every DEBUG
		// LOG_DEBUG/LOG_TRACE statement).
		return loggers.try_emplace(channel_name,
			channel_logger_type(
				keywords::channel = channel_name))
			.first->second;
	}

private:
	inline static boost::log::trivial::severity_level set_level;
};

/// @brief Static instance whose construction configures the logging system.
inline static logging initialize_logging;

} // namespace idni::tau_lang

#endif //__IDNI__TAU__LOGGING_H__