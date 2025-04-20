// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "repl_evaluator.h"

namespace idni::tau_lang {

#define TC_STATUS        TC.BG_LIGHT_CYAN()
#define TC_STATUS_OUTPUT TC(color::GREEN, color::BG_LIGHT_CYAN, color::BRIGHT)
#define TC_ERROR         TC(color::RED,   color::BRIGHT)
#define TC_PROMPT        TC(color::WHITE, color::BRIGHT)
#define TC_OUTPUT        TC.GREEN()

template <BAsPack... BAs>
void repl_evaluator<BAs...>::not_implemented_yet() {
	std::cout << "Not implemented yet.\n";
}

template <BAsPack... BAs>
std::optional<size_t> repl_evaluator<BAs...>::get_memory_index(const tt& n,
	const size_t size, bool silent) const
{
	if (size == 0) {
		if (!silent) std::cout << "history is empty\n";
		return {};
	}
	auto mem_type = n | tt::only_child | tt::nt;
	auto is_relative = (mem_type == tau_parser::rel_memory);
	auto mem_id = n | mem_type | tau_parser::memory_id;
	size_t idx = 0;
	if (mem_id) idx = mem_id | tt::num;
	if ((is_relative && idx >= size)
		|| (!is_relative && (idx == 0 || idx > size)))
	{
		if (!silent) {
			std::cout << "history location " << TC_OUTPUT
				<< (is_relative ? "%-" : "%");
			if (!is_relative || idx) std::cout << idx;
			std::cout << TC.CLEAR() << " does not exist\n";
		}
		return {};
	}
	return { is_relative ? size - idx - 1 : idx - 1 };
}

template <BAsPack... BAs>
repl_evaluator<BAs...>::memory_ref repl_evaluator<BAs...>::memory_retrieve(
	const tt& n, bool silent)
{
	if (auto pos = get_memory_index(n, m.size(), silent); pos.has_value())
		return { { m[pos.value()], pos.value() } };
	BOOST_LOG_TRIVIAL(error) << "(Error) history location does not exist\n";
	return {};
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::print_memory(const htree::sp& mem, const size_t id,
	const size_t size, bool print_relative_index) const
{
	std::cout << TC_OUTPUT << "%" << id + 1 << TC.CLEAR();
	if (print_relative_index) {
		std::cout << "/" << TC_OUTPUT;
		if (size - id == 1) std::cout << "%";
		else std::cout << "%-" << size - id - 1;
		std::cout << TC.CLEAR();
	}
	std::cout << ": ";
	tau::get(mem).print(std::cout) << "\n";
}

template <BAsPack... BAs>
tref repl_evaluator<BAs...>::apply_rr_to_rr_tau_nso(
	const size_t nt, const tt& program)
{
	// TODO
	not_implemented_yet();
	return 0;
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::history_print_cmd(const tt& command) {
	auto n = command | tau_parser::memory;
	if (!n) return;
	auto idx = get_memory_index(n.value(), m.size());
	if (idx) print_memory(m[idx.value()], idx.value(), m.size());
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::history_list_cmd() {
	if (m.size() == 0) std::cout << "history is empty\n";
	else for (size_t i = 0; i < m.size(); i++)
		print_memory(m[i], i, m.size());
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::memory_store(tref o) {
	auto v = tau::geth(o);
	// do not add into memory if the last memory value is the same
	if (!(m.size() && m.back() == v))
		m.push_back(v);
	if (opt.print_memory_store)
		print_memory(m.back(), m.size() - 1, m.size(), false);
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::history_store_cmd(const tt& command) {
	memory_store(command | tt::first | tt::ref);
}

template <BAsPack... BAs>
repl_evaluator<BAs...>::tt repl_evaluator<BAs...>::get_(tau::node::type nt,
	const tt& n, bool suppress_error)
{
	if (n.value_tree().is(nt)) return n;
	else if (n.value_tree().is(tau::memory)) {
		if (auto check = memory_retrieve(n); check) {
			tt value(check.value().first);
			if (value.value_tree().is(nt)) return value;
			else if (!suppress_error) BOOST_LOG_TRIVIAL(error)
				<< "(Error) argument has wrong type";
			return {};
		}
	}
	if (!suppress_error) BOOST_LOG_TRIVIAL(error)
		<< "(Error) argument has wrong type";
	return {};
}

template <BAsPack... BAs>
repl_evaluator<BAs...>::tt repl_evaluator<BAs...>::get_bf(const tt& n,
	bool suppress_error)
{
	return get_(tau::bf, n, suppress_error);
}
template <BAsPack... BAs>
repl_evaluator<BAs...>::tt repl_evaluator<BAs...>::get_wff(const tt& n) {
	return get_(tau::wff, n, false);
}

template <BAsPack... BAs>
bool repl_evaluator<BAs...>::contains(const tt& n, node::type nt)
{
	bool found = false;
	idni::pre_order(n).search([&](const auto& n) {
		if (tau::get(n).get_type() == nt) return found = true;
		return false;
	});
	return found;
}

template <BAsPack... BAs>
std::optional<std::pair<size_t, tref>>
	repl_evaluator<BAs...>::get_type_and_arg(const tt& n)
{
	auto nt = n | tt::nt;
	switch (nt) {
	case tau_parser::memory:
		if (auto check = memory_retrieve(n); check) {
			auto [value, _] = check.value();
			auto mem_type = tt(value) | tt::nt;
			return { { mem_type, value->get() } };
		} else return {};
	default: return { std::pair<size_t, tref>{ nt, n | tt::ref } };
	}
}

template <BAsPack... BAs>
tref repl_evaluator<BAs...>::onf_cmd(const tt& /*n*/) {
	return 0;
}


template <BAsPack... BAs>
tref repl_evaluator<BAs...>::dnf_cmd(const tt& /*n*/) {
	// TODO
	not_implemented_yet();
	return 0;
}


template <BAsPack... BAs>
tref repl_evaluator<BAs...>::cnf_cmd(const tt& /*n*/) {
	// TODO
	not_implemented_yet();
	return 0;
}


template <BAsPack... BAs>
tref repl_evaluator<BAs...>::nnf_cmd(const tt& /*n*/) {
	// TODO
	not_implemented_yet();
	return 0;
}


template <BAsPack... BAs>
tref repl_evaluator<BAs...>::mnf_cmd(const tt& /*n*/) {
	// TODO
	not_implemented_yet();
	return 0;
}


template <BAsPack... BAs>
tref repl_evaluator<BAs...>::snf_cmd(const tt& /*n*/) {
	// TODO
	not_implemented_yet();
	return 0;
}


template <BAsPack... BAs>
tref repl_evaluator<BAs...>::bf_substitute_cmd(const tt& /*n*/) {
	// TODO
	not_implemented_yet();
	return 0;
}

template <BAsPack... BAs>
tref repl_evaluator<BAs...>::substitute_cmd(const tt& /*n*/) {
	// TODO
	not_implemented_yet();
	return 0;
}

template <BAsPack... BAs>
tref repl_evaluator<BAs...>::instantiate_cmd(const tt& n) {
	auto var_type = n[2].is(tau::variable) ? tau::bf : tau::wff;
	auto nn = tau::get(n.value_tree().value, { n[0].get(), n[1].get(),
			tau::get(var_type, n[2].get()), n[3].get() });
	return substitute_cmd(nn);
}

template <BAsPack... BAs>
tref repl_evaluator<BAs...>::normalize_cmd(const tt& /*n*/) {
	// TODO
	not_implemented_yet();
	return 0;
}

template <BAsPack... BAs>
tref repl_evaluator<BAs...>::qelim_cmd(const tt& /*n*/) {
	// TODO
	not_implemented_yet();
	return 0;
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::run_cmd(const tt& /*n*/) {
	// TODO
	not_implemented_yet();
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::solve_cmd(const tt& /*n*/) {
	// TODO
	not_implemented_yet();
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::lgrs_cmd(const tt& /*n*/) {
	// TODO
	not_implemented_yet();
}

template <BAsPack... BAs>
tref repl_evaluator<BAs...>::is_valid_cmd(const tt& /*n*/)
{
	// TODO
	not_implemented_yet();
	return 0;
}

template <BAsPack... BAs>
tref repl_evaluator<BAs...>::sat_cmd(const tt& /*n*/) {
	// TODO
	not_implemented_yet();
	return 0;
}

template <BAsPack... BAs>
tref repl_evaluator<BAs...>::is_unsatisfiable_cmd(const tt& /*n*/) {
	// TODO
	not_implemented_yet();
	return 0;
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::def_rr_cmd(const tt& /*n*/) {
	// TODO
	not_implemented_yet();
}

template <NodeType node>
std::ostream& print_rule(std::ostream& os, const rewriter::rule& r) {
	tree<node>::get(r.first).print(os) << " := ";
	tree<node>::get(r.second).print(os) << ".";
	return os;
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::def_list_cmd() {
	if (definitions.size() == 0) std::cout << "definitions: empty\n";
	else std::cout << "definitions:\n";
	for (size_t i = 0; i < definitions.size(); i++)
		print_rule<node>(std::cout << "    [" << i + 1 << "] ",
						definitions[i]) << "\n";
	if (inputs.size() == 0 && outputs.size() == 0)
		std::cout << "io variables: empty\n";
	else std::cout << "io variables:\n";
	for (auto& [var, desc]: inputs) {
		auto file = desc.second.empty() ? "console"
					: "ifile(\"" + desc.second + "\")";
		std::cout << "\t" << desc.first << " "
						<< var << " = " << file << "\n";
	}
	for (auto& [var, desc]: outputs) {
		auto file = desc.second.empty() ? "console"
					: "ofile(\"" + desc.second + "\")";
		std::cout << "\t" << desc.first << " "
						<< var << " = " << file << "\n";
	}
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::def_print_cmd(const tt& command) {
	if (definitions.size() == 0) std::cout << "rec. relations: empty\n";
	auto num = command | tau_parser::number;
	if (!num) return;
	auto i = num | tt::num;
	if (i && i <= definitions.size()) {
		print_rule<node>(std::cout, definitions[i-1]) << "\n";
		return;
	}
	BOOST_LOG_TRIVIAL(error)
		<< "(Error) definition [" << i << "] does not exist\n";
	return;
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::def_input_cmd(const tt& command) {
	std::string fn;
	auto type_name = command | tau::type | tt::string;
	if (auto file_name = command
			| tau::input_stream
			| tau::q_file_name
			| tt::string; !file_name.empty()) fn = file_name;
	else fn = ""; // default input (std::cin)

	// TODO
	not_implemented_yet();
	// for (auto& t : nso_factory<tau_ba_t, BAs...>::instance().types()) {
	// 	if (type == t) {
	// 		auto var_name = command
	// 			| tau_parser::in_var_name
	// 			| optional_value_extractor<tau_nso_t>;
	// 		inputs[var_name] = {type, fn};
	// 		return;
	// 	}
	// }
	BOOST_LOG_TRIVIAL(error) << "(Error) invalid type " << type_name << "\n";
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::def_output_cmd(const tt& command) {
	std::string fn;
	std::string type_name = command | tau::type | tt::string;
	if (auto file_name = command
			| tau::output_stream
			| tau::q_file_name
			| tt::string; !file_name.empty()) fn = file_name;
	else fn = ""; // default output (std::cout)

	// TODO
	not_implemented_yet();
	// for (auto& t: nso_factory<tau_ba_t, BAs...>::instance().types()) {
	// 	if (type == t) {
	// 		auto var_name = command
	// 			| tau_parser::out_var_name
	// 			| optional_value_extractor<tau_nso_t>;
	// 		outputs[var_name] = {type, fn};
	// 		return;
	// 	}
	// }
	BOOST_LOG_TRIVIAL(error) << "(Error) invalid type " << type_name << "\n";
}

// make a nso_rr from the given tau source and binder.
template <BAsPack... BAs>
tref repl_evaluator<BAs...>::make_cli(const std::string& src) {
	// remove ascii char 22 if exists in the input
	std::string filt = src;
	filt.erase(remove_if(filt.begin(), filt.end(), [](unsigned char c) {
		return c == 22;
	}), filt.end());
	tau_parser::result result = tau_parser::instance()
		.parse(filt.c_str(), filt.size(), {
						.start = tau_parser::cli });
	auto fail = [this]() { return error = true, nullptr; };
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(tau_parser::error::info_lvl::INFO_BASIC);
		if (opt.error_quits
			|| msg.find("Syntax Error: Unexpected end")!=0)
		{
			BOOST_LOG_TRIVIAL(error) << "(Error) " << msg << "\n";
			return fail();
		}
		return nullptr; // Unexpected eof, continue with reading input
	}
	auto t = result.get_shaped_tree2();
	auto bound = tau::get(tau_parser::tree::get(t));
	if (!bound) return fail();
	return bound;
}

inline repl_option get_opt(const std::string& x) {
	if (x.empty())                       return none_opt;
	if (x == "S" || x == "severity"
		|| x == "sev")               return severity_opt;
	if (x == "s" || x == "status")       return status_opt;
	if (x == "c" || x == "colors"
		|| x == "color")             return colors_opt;
	if (x == "V" || x == "charvar")      return charvar_opt;
	if (x == "H" || x == "highlighting"
		|| x == "highlight")         return highlighting_opt;
	if (x == "I" || x == "indenting"
		|| x == "indent")            return indenting_opt;
	if (x == "d" || x == "debug"
		|| x == "dbg")               return debug_opt;
	BOOST_LOG_TRIVIAL(error) << "(Error) invalid option: " << x << "\n";
	return invalid_opt;
}

template <NodeType node>
repl_option get_opt(const typename tree<node>::traverser& n) {
	auto o = n | tau_parser::option_name;
	if (!o) return none_opt;
	return get_opt(o | tree<node>::traverser::string);
}

template <BAsPack... BAs>
std::optional<boost::log::trivial::severity_level>
	str2severity(const std::string& v)
{
	if (v == "e" || v == "error") return { boost::log::trivial::error };
	if (v == "d" || v == "debug") return { boost::log::trivial::debug };
	if (v == "t" || v == "trace") return { boost::log::trivial::trace };
	if (v == "i" || v == "info")  return { boost::log::trivial::info };
	BOOST_LOG_TRIVIAL(error) << "(Error) invalid severity value: " << v
		<< " (only error, info, debug or trace are allowed)\n";
	return {};
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::get_cmd(const tt& n) {
	return get_cmd(get_opt<node>(n));
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::get_cmd(repl_option o) {
	static std::string pbool[] = { "off", "on" };
	static std::map<repl_option, std::function<void()>> printers = {
#ifdef DEBUG
	{ debug_opt, [this]() {
		std::cout << "debug-repl:          " << pbool[opt.debug_repl] << "\n"; } },
#endif
	{ status_opt,       [this]() {
		std::cout << "status:              " << pbool[opt.status] << "\n"; } },
	{ colors_opt,       [this]() {
		std::cout << "colors:              " << pbool[opt.colors] << "\n"; } },
	{ charvar_opt,      [this]() {
		std::cout << "charvar:             " << pbool[opt.charvar] << "\n"; } },
	{ highlighting_opt, [this]() {
		std::cout << "syntax highlighting: " << pbool[pretty_printer_highlighting] << "\n"; } },
	{ indenting_opt,    [this]() {
		std::cout << "indenting:           " << pbool[pretty_printer_indenting] << "\n"; } },
	{ severity_opt,     [this]() {
		std::cout << "severity:            " << opt.severity << "\n"; } }};
	if (o == invalid_opt) return;
#ifndef DEBUG
	if (o == debug_opt) {
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) debug option not available in release build\n";
		return;
	}
#endif
	if (o == none_opt) { for (auto& [_, v] : printers) v(); return; }
	printers[o]();
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::set_cmd(const tt& n) {
	repl_option o = get_opt<node>(n);
	auto ov = n | tau::option_value;
	if (!ov) {BOOST_LOG_TRIVIAL(error) << "(Error) invalid value\n";return;}
	set_cmd(o, ov | tt::string);
	get_cmd(n);
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::set_cmd(repl_option o, const std::string& v) {
	using namespace boost::log;
	if (o == invalid_opt || o == none_opt) return;
#ifndef DEBUG
	if (o == debug_opt) {
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) debug option not available\n";
		return;
	}
#endif
	auto update_bool_value = [&v](bool& opt) {
		if (v == "t" || v == "true" || v == "on" || v == "1"
			|| v == "y" || v == "yes") opt = true;
		else if (v == "f" || v == "false" || v == "off" || v == "0"
			|| v == "n" || v == "no") opt = false;
		else BOOST_LOG_TRIVIAL(error) << "(Error) invalid bool value\n";
		return opt;
	};
	static std::map<repl_option, std::function<void()>> setters = {
#ifdef DEBUG
	{ debug_opt, [&]() {
		update_bool_value(opt.debug_repl); } },
#endif
	{ status_opt,   [&]() {
		update_bool_value(opt.status); } },
	{ colors_opt,   [&]() {
		TC.set(update_bool_value(opt.colors)); } },
	{ charvar_opt,   [&]() {
		update_charvar(update_bool_value(opt.charvar)); } },
	{ highlighting_opt,   [&]() {
		update_bool_value(pretty_printer_highlighting); } },
	{ indenting_opt,   [&]() {
		update_bool_value(pretty_printer_indenting); } },
	{ severity_opt, [&]() {
		auto sev = str2severity(v);
		if (!sev.has_value()) return;
		opt.severity = sev.value();
		boost::log::core::get()->set_filter(
			boost::log::trivial::severity >= opt.severity);
	} } };
	setters[o]();
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::update_bool_opt_cmd(const tt& n,
	const std::function<bool(bool&)>& update_fn)
{
	auto o = get_opt<node>(n);
	update_bool_opt_cmd(o, update_fn);
	get_cmd(n);
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::update_bool_opt_cmd(repl_option o,
	const std::function<bool(bool&)>& update_fn)
{
	if (o == invalid_opt || o == none_opt) return;
#ifndef DEBUG
	if (o == debug_opt) {
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) debug option not available\n";
		return;
	}
#endif
	switch (o) {
#ifdef DEBUG
	case debug_opt: update_fn(opt.debug_repl); break;
#endif
	case colors_opt:       TC.set(update_fn(opt.colors)); break;
	case charvar_opt:      update_charvar(update_fn(opt.charvar)); break;
	case highlighting_opt: update_fn(pretty_printer_highlighting); break;
	case indenting_opt:    update_fn(pretty_printer_indenting); break;
	case status_opt:       update_fn(opt.status); break;
	default: BOOST_LOG_TRIVIAL(error) << "(Error) unknown bool option\n";
		error = true;
		return;
	}
}

template <BAsPack... BAs>
bool repl_evaluator<BAs...>::update_charvar(bool value) {
	std::set<std::string> guards{
		(opt.charvar = value) ? "charvar" : "var" };
	tau_parser::instance().get_grammar().set_enabled_productions(guards);
	sbf_parser::instance().get_grammar().set_enabled_productions(guards);
	return value;
}

template <BAsPack... BAs>
int repl_evaluator<BAs...>::eval_cmd(const tt& n) {
	auto command = n | tt::only_child;
	auto command_type = command | tt::nt;
#ifdef DEBUG
	if (opt.debug_repl) {
		// std::cout << "command: " << command << "\n";
		command.value_tree().print_tree(std::cout << "tree: ") << "\n";
	}
#endif
	tref result = 0;
	switch (command_type) {
	case tau::quit_cmd:           return std::cout << "Quit.\n", 1;
	case tau::clear_cmd:          if (r) r->clear(); break;
	case tau::help_cmd:           help_cmd(command); break;
	case tau::version_cmd:        version_cmd(); break;
	case tau::get_cmd:            get_cmd(command); break;
	case tau::set_cmd:            set_cmd(command); break;
	case tau::enable_cmd:         update_bool_opt_cmd(command,
					[](bool& b){ return b = true; }); break;
	case tau::disable_cmd:        update_bool_opt_cmd(command,
					[](bool& b){ return b = false; });break;
	case tau::toggle_cmd:         update_bool_opt_cmd(command,
					[](bool& b){ return b = !b; }); break;
	case tau::history_list_cmd:   history_list_cmd(); break;
	case tau::history_print_cmd:  history_print_cmd(command); break;
	case tau::history_store_cmd:  history_store_cmd(command); break;
	// normalization
	case tau::normalize_cmd:      result = normalize_cmd(command); break;
	// execution
	case tau::run_cmd:            run_cmd(command); break;
	case tau::solve_cmd:          solve_cmd(command); break;
	case tau::lgrs_cmd:           lgrs_cmd(command); break;
	// substitution and instantiation
	case tau::subst_cmd:          result = substitute_cmd(command); break;
	case tau::inst_cmd:           result = instantiate_cmd(command); break;
	// formula checks
	case tau::sat_cmd:            result = sat_cmd(command); break;
	case tau::valid_cmd:          result = is_valid_cmd(command); break;
	case tau::unsat_cmd:          result = is_unsatisfiable_cmd(command); break;
	// normal forms
	case tau::onf_cmd:            result = onf_cmd(command); break;
	case tau::dnf_cmd:            result = dnf_cmd(command); break;
	case tau::cnf_cmd:            result = cnf_cmd(command); break;
	case tau::anf_cmd:            not_implemented_yet(); break;
	case tau::nnf_cmd:            result = nnf_cmd(command); break;
	case tau::pnf_cmd:            not_implemented_yet(); break;
	case tau::mnf_cmd:            result = mnf_cmd(command); break;
	case tau::snf_cmd:            result = snf_cmd(command); break;
	// definition of rec relations to be included during normalization
	case tau::def_rr_cmd:         def_rr_cmd(command); break;
	case tau::def_list_cmd:       def_list_cmd(); break;
	case tau::def_print_cmd:      def_print_cmd(command); break;
	// definitions of i/o streams
	case tau::def_input_cmd:      def_input_cmd(command); break;
	case tau::def_output_cmd:     def_output_cmd(command); break;
	// qelim
	case tau::qelim_cmd:          result = qelim_cmd(command); break;
	case tau::comment:            break;
	// error handling
	default: error = true;
		BOOST_LOG_TRIVIAL(error) << "\n (Error) Unknown command";
	}
#ifdef DEBUG
	if (opt.debug_repl && result) tau::get(result).print(
		std::cout << "result tree: ") << "\n";
#endif
	if (result) memory_store(result);
	return 0;
}

template <BAsPack... BAs>
repl_evaluator<BAs...>::repl_evaluator(options opt): opt(opt)
{
	TC.set(opt.colors);
	boost::log::core::get()->set_filter(
		boost::log::trivial::severity >= opt.severity);
	// Controls how fixpoint information in satisfiability.h should be printed
	if (!opt.repl_running) use_debug_output_in_sat = true;
	if (opt.experimental) std::cout << "\n!!! Experimental features "
		"enabled: new tree API (almost nothing works yet) !!!\n\n";
}

template <BAsPack... BAs>
std::string repl_evaluator<BAs...>::prompt() {
	using namespace boost::log;
	using namespace idni::term;
	std::stringstream ss;
	if (opt.status) {
		std::stringstream status;
		if (m.size()) status << " " << TC_STATUS_OUTPUT << "%"
			<< m.size() << TC.CLEAR() << TC_STATUS;
		if (opt.severity != trivial::info)
			status << " " << to_string(opt.severity);
		if (status.tellp()) ss << TC_STATUS << "["
			<< status.str() << " ]" << TC.CLEAR() << " ";
	}
	ss << (error ? TC_ERROR : TC_PROMPT) << "tau>" << TC.CLEAR() << " ";
	if (r) r->set_prompt(ss.str());
	return ss.str();
}

template <BAsPack... BAs>
int repl_evaluator<BAs...>::eval(const std::string& src) {
	error = false;
	auto tau_spec = tt(make_cli(src));
	int quit = 0;
	if (tau_spec) {
		auto commands = tau_spec || tau::cli_command;
		for (const auto& cmd : commands())
			if (quit = eval_cmd(cmd); quit == 1) break;
	} else if (!error) return 2;
	std::cout << "\n", std::cout.flush();
	if (error && opt.error_quits) return quit = 1;
	if (quit == 0) prompt();
	return quit;
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::version_cmd() {
	std::cout << full_version << "\n";
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::help_cmd(const tt& n) const {
	auto arg = n | tau::help_arg | tt::only_child;
	size_t nt = arg ? (arg | tt::nt) : tau::help_sym;
	help(nt);
}

template <BAsPack... BAs>
void repl_evaluator<BAs...>::help(size_t nt) const {
	static const std::string bool_options =
		"  <option>               <description>                        <value>\n"
#ifdef DEBUG
		"  debug-repl             show REPL commands             on/off\n"
#endif
		"  status                 show status                          on/off\n"
		"  colors                 use term colors                      on/off\n"
		"  highlighting           syntax highlighting of Tau formulas  on/off\n"
		"  indenting              indenting of Tau formulas            on/off\n";
	static const std::string all_available_options = std::string{} +
		"Available options and values:\n" + bool_options +
		"  severity               severity                             error/info/debug/trace\n";
	static const std::string bool_available_options = std::string{} +
		"Available options and values:\n" + bool_options;
	switch (nt) {
	case tau_parser::help_sym: std::cout
		<< "General commands:\n"
		<< "  help or h               print overview of available commands in Tau repl\n"
		<< "  quit or q               exit the Tau repl\n"
		<< "  version or v            print current version\n"
		<< "  clear                   clear the screen\n"
		<< "\n"

		<< "Run command:\n"
		<< "  run                     execute a Tau formula as a program\n"
		<< "\n"

		<< "Logical procedures:\n"
		<< "  normalize or n          normalize a Tau expression\n"
		<< "  qelim                   eliminate non-temporal quantifiers in a Tau formula\n"
		<< "  sat                     check if a Tau formula is satisfiable\n"
		<< "  unsat                   check if a Tau formula is unsatisfiable\n"
		<< "  valid                   check if a Tau formula is valid\n"
		<< "  solve                   compute a satisfying assignment for the free variables in a Tau formula\n"
		<< "  lgrs                    compute a LGRS for a given equation\n"
		<< "\n"

		<< "Normal form commands:\n"
		<< "  cnf                     convert a Tau expression to conjunctive normal form\n"
		<< "  dnf                     convert a Tau expression to disjunctive normal form\n"
		<< "  mnf                     convert a Tau expression to minterm normal form\n"
		//<< "  anf                     convert to algebraic normal form\n"
		<< "  nnf                     convert a Tau expression to negation normal form\n"
		//<< "  pnf                     convert to prenex normal form\n"
		<< "  onf                     convert a Tau formula to order normal form\n"
		<< "\n"

		<< "History and definitions:\n"
		<< "  history or hist         show all Tau expressions stored in the repl memory\n"
		<< "  definitions or defs     show stored IO variables and function and predicate definitions\n"
		<< "\n"

		<< "Substitution and instantiation command:\n"
		<< "  substitute, subst or s  substitute a Tau expression in a Tau expression by another\n"
		<< "  instantiate, inst or i  instantiate a variable in a Tau expression with a Tau term\n"
		<< "\n"

		<< "Settings commands:\n"
		<< "  get                     show option values\n"
		<< "  set                     set option's value\n"
		<< "  enable                  enable option's value\n"
		<< "  disable                 disable option's value\n"
		<< "  toggle                  toggle option's value\n"
		<< "\n"

		<< "Examples:\n"
		<< "  help or h examples      show examples related to the Tau language syntax\n"
		<< "\n"

		<< "Type \'help <command>\' for more information about a specific command\n";
		break;
	case tau_parser::version_sym: std::cout
		<< "version prints the current Tau version\n";
		break;
	case tau_parser::quit_sym: std::cout
		<< "quit exits the Tau repl\n";
		break;
	case tau_parser::clear_sym: std::cout
		<< "clear clears the terminal screen\n";
		break;
	case tau_parser::get_sym: std::cout
		<< "get                 prints all options and their values\n"
		<< "get <option>        prints the value of the given option\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau_parser::set_sym: std::cout
		<< "set <option> [=] <value> sets option to value\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau_parser::enable_sym: std::cout
		<< "enable <option> enables option\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau_parser::disable_sym: std::cout
		<< "disable <option> disables option\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau_parser::toggle_sym: std::cout
		<< "toggle <option> toggles option value\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau_parser::history_sym: std::cout
		<< "the history command shows all stored Tau expressions in the repl memory\n"
		<< "\n"
		<< "  history or hist                shows all stored Tau expressions\n"
		<< "  history or hist <repl_memory>  shows the Tau expression stored at the specified repl memory position\n"
		<< "  <rr>, <tau> or <term>          stores a Tau expression in the repl memory at the latest position\n"
		<< "\n"
		<< "the repl memory can be accessed in the following ways\n\n"
		<< "  %                              to retrieve the Tau expression stored at the latest position\n"
		<< "  %-<number>                     to retrieve the Tau expression stored at the latest position minus <number>\n"
		<< "  %<number>                      to retrieve the Tau expression stored at position <number>\n\n"
		<< "stored Tau expressions can be used wherever a command expects a Tau expression\n";
		break;
	case tau_parser::normalize_sym: std::cout
		<< "the normalize command normalizes a Tau expression, prints the result and\n"
		<< "saves it into the repl memory\n"
		<< "\n"
		<< "usage:\n"
		<< "  normalize <rr>		    normalizes the given tau formula with additional predicate and function definitions\n"
		<< "  normalize <tau>		    normalizes the given tau formula\n"
		<< "  normalize <term>		    normalizes the given term\n"
		<< "  normalize <repl_memory>   normalizes the Tau expression stored at the specified repl memory position\n";
		break;
	case tau_parser::qelim_sym: std::cout
		<< "the qelim command eliminates all non-temporal quantifiers, prints the result and\n"
		<< "saves it into the repl memory\n"
		<< "\n"
		<< "usage:\n"
		<< "  qelim <tau>               eliminates non-temporal quantifiers in the given tau formula\n"
		<< "  qelim <repl_memory>       eliminates non-temporal quantifiers in the Tau formula stored at the specified repl memory position\n";
		break;
	case tau_parser::run_sym: std::cout
		<< "the run command executes a Tau formula as a program\n"
		<< "\n"
		<< "run automatically chooses a single program from the set of programs satisfying a given Tau formula\n"
		<< "and executes it in such a way that in each step (starting at 0) each input variable reads a value from the specified stream\n"
		<< "and then a value is written into each specified output stream such that the Tau formula being executed is satisfied\n\n"
		<< "usage:\n"
		<< "  run <tau>                 execute the given Tau formula\n"
		<< "  run <repl_memory>         execute the Tau formula stored at the specified repl memory position\n"
		<< "\n";
		break;
	case tau_parser::solve_sym: std::cout
		<< "the solve command computes a single satisfying assignment for the free variables in a Tau formula\n"
		<< "\n"
		<< "usage:\n"
		<< "  solve [options] <tau>              computes a single satisfying assignment for the free variables in the Tau formula\n"
		<< "  solve [options] <repl_memory>      computes a single satisfying assignment for the free variables in the Tau formula stored at the specified repl memory position"
		<< "\n"
		<< "options:\n"
		<< "  --min, --minimmum                  computes the minimum extreme point of the lgrs corresponding to the solution\n"
		<< "  --max, --maximum                   computes the maximum extreme point of the lgrs corresponding to the solution\n"
		<< "  --<type>                           uses the specified type for the solution (sbf or tau)\n"
		<< "\n";
		break;
	case tau_parser::lgrs_sym: std::cout
		<< "the lgrs command computes a LGRS for an equation\n"
		<< "\n"
		<< "usage:\n"
		<< "  lgrs <tau_eq>            computes a LGRS for a given equation\n"
		<< "  lgrs <repl_memory>       computes a LGRS for the equation stored at the specified repl memory position\n"
		<< "options:\n"
		<< "  --<type>                 uses the specified type for the solution\n"
		<< "\n";
		break;
	case tau_parser::sat_sym: std::cout
		<< "the sat command checks if a Tau formula is satisfiable and if so prints T and else F\n\n"
		<< "a tau formula is satisfiable if there exists a variable assignment to non-temporal variables\n"
		<< "such that for all possible inputs there exist time compatible outputs at each point in time\n"
		<< "\n"
		<< "usage:\n"
		<< "  sat <rr>		           checks the given tau formula with additional predicate and function definitions for satisfiability\n"
		<< "  sat <tau>		           checks the given tau formula for satisfiability\n"
		<< "  sat <repl_memory>	       checks the Tau formula stored at the specified repl memory position for satisfiability\n";
		break;
	case tau_parser::valid_sym: std::cout
		<< "the valid command checks if a Tau formula is logically equivalent to T and if so prints T and else F\n"
		<< "\n"
		<< "usage:\n"
		<< "  valid <rr>		       checks the given tau formula with additional predicate and function definitions for validity\n"
		<< "  valid <tau>		       checks the given tau formula for validity\n"
		<< "  valid <repl_memory>	   checks the Tau formula stored at the specified repl memory position for validity\n";
		break;
		break;
	case tau_parser::unsat_sym: std::cout
		<< "the unsat command checks if a Tau formula is unsatisfiable and if so prints T and else F\n\n"
		<< "a tau formula is unsatisfiable if for every variable assignment to non-temporal variables\n"
		<< "there exist inputs such that there are no time compatible outputs at some point in time\n"
		<< "\n"
		<< "usage:\n"
		<< "  unsat <rr>		       checks the given tau formula with additional predicate and function definitions for unsatisfiability\n"
		<< "  unsat <tau>		       checks the given tau formula for unsatisfiability\n"
		<< "  unsat <repl_memory>	   checks the Tau formula stored at the specified repl memory position for unsatisfiability\n";
		break;
	case tau_parser::dnf_sym: std::cout
		<< "dnf converts a Tau expression to disjunctive normal form (DNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  dnf <term>               converts the given term to DNF\n"
		<< "  dnf <tau>                converts the given Tau formula to DNF\n"
		<< "  dnf <repl_memory>        converts the Tau expression stored at the specified repl memory position to DNF\n";
		break;
	case tau_parser::cnf_sym: std::cout
		<< "cnf converts a Tau expression to conjunctive normal form (CNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  cnf <term>              converts the given term to CNF\n"
		<< "  cnf <tau>               converts the given Tau formula to CNF\n"
		<< "  cnf <repl_memory>       converts the Tau expression stored at the specified repl memory position to CNF\n";
		break;
	//case tau_parser::anf_sym: std::cout
	//	<< "anf command converts a boolean formula or a well formed formula to algebraic normal form\n"
	//	<< "\n"
	//	<< "usage:\n"
	//	<< "  anf <term>             converts the given term to ANF\n"
	//	<< "  anf <tau>            converts the given tau to ANF\n"
	//	<< "  anf <memory>         converts the memory with the given id to ANF\n";
	//	break;
	case tau_parser::nnf_sym: std::cout
		<< "nnf converts a Tau expression to negation normal form (NNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  nnf <term>              converts the given term to NNF\n"
		<< "  nnf <tau>               converts the given tau formula to NNF\n"
		<< "  nnf <repl_memory>       converts the Tau expression stored at the specified repl memory position to NNF\n";
		break;
	//case tau_parser::pnf_sym: std::cout
	//	<< "pnf command converts a boolean formula or a well formed formula to prenex normal form\n"
	//	<< "\n"
	//	<< "usage:\n"
	//	<< "  pnf <term>             converts the given term to PNF\n"
	//	<< "  pnf <tau>            converts the given term to PNF\n"
	//	<< "  pnf <memory>           converts the memory with the given id to PNF\n";
	//	break;
	case tau_parser::mnf_sym: std::cout
		<< "mnf converts a Tau expression to minterm normal form (MNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  mnf <term>              converts the given term to MNF\n"
		<< "  mnf <tau>               converts the given tau formula to MNF\n"
		<< "  mnf <repl_memory>       converts the Tau expression stored at the specified repl memory position to MNF\n";
		break;
	case tau_parser::onf_sym: std::cout
		<< "onf converts a tau formula to order normal form (ONF) on the specified variable\n"
		<< "\n"
		<< "usage:\n"
		<< "  onf <var> <tau>           converts the given tau formula to ONF using <var>\n"
		<< "  onf <var> <repl_memory>   converts the Tau formula stored at the specified repl memory position to ONF using <var>\n";
		break;
	case tau_parser::subst_sym: std::cout
		<< "the substitute command substitutes a Tau expression in a Tau expression by another Tau expression\n"
		<< "\n"
		<< "usage:\n"
		<< "  substitute <input> [ <match> / <replace>]\n"
		<< "\n"
		<< "where:\n"
		<< "  <input> is the Tau expression in which to replace\n"
		<< "  <match> is the Tau expression to be replaced in <input>\n"
		<< "  <replace> is the Tau expression that is replaced for <match>\n"
		<< "\n"
		<< "  Note that if <input> is of type term, <match> and <replace> must be of type term\n"
		<< "  In general <match> and <replace> must be of the same type, so either both term or tau\n"
		<< "\n";
		break;
	case tau_parser::inst_sym: std::cout
		<< "the instantiate command instantiates a variable in a Tau formula with the specified term\n"
		<< "\n"
		<< "usage:\n"
		<< "  instantiate <input> [ <variable> / <value>]\n"
		<< "\n"
		<< "where:\n"
		<< "  <input> is the Tau term to instantiate in\n"
		<< "  <variable> is the variable to be instantiated\n"
		<< "  <value> is the Tau term to instantiate with\n"
		<< "\n";
		break;
	case tau_parser::def_sym: std::cout
		<< "the definitions command shows stored input/output stream variables and function and predicate definitions\n"
		<< "\n"
		<< "usage:\n"
		<< "  <tau_rec_relation>    defines a tau predicate\n"
        << "  <term_rec_relation>   defines a tau function\n"
		<< "  <def_input_cmd>       defines an input stream variable\n"
		<< "  <def_output_cmd>      defines an output stream variable\n"
		<< "  definitions           lists all definitions present in repl\n"
		<< "  definitions <number>  prints predicate or function at specified position\n"
		<< "\n"
		<< "Examples defining stream variables and functions can be found by typing \'help examples\'\n";
		break;
	case tau_parser::examples_sym: std::cout
		<< "examples\n"
		<< "\n"
		<< "  # defining an input stream variable\n"
		<< "  sbf i1 = console\n"
		<< "  tau i2 = ifile(\"inputs.in\")\n"
		<< "\n"
		<< "  # defining an output stream variable\n"
		<< "  sbf o1 = console\n"
		<< "  tau o2 = ofile(\"outputs.out\")\n"
		<< "\n"
		<< "  # defining functions\n"
		<< "  (Tau term function)    rr1(x,y,z) := (x & y) | z\n"
		<< "  (Tau formula function) rr2(x,y,z) := ( x = 0 || y = 0) && z = 0\n\n"
		<< "\n";
		break;
	}
}

// undef terminal color macros valid only for this file
#undef TC_STATUS
#undef TC_STATUS_OUTPUT
#undef TC_ERROR
#undef TC_PROMPT
#undef TC_OUTPUT

} // idni::tau_lang namespace
