// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TAU_COMPILE_TMPL_H__
#define __IDNI__TAU__TAU_COMPILE_TMPL_H__

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <sstream>
#include <utility>
#include <vector>

#include "cpp_codegen.h"
#include "definitions.h"
#include "ltl_aba_limits.h"
#include "self_exe_path.h"
#include "tau_artifact_template.h"
#include "tau_pack.h"
#include "utility/escapes.h"

namespace idni::tau_lang {

namespace compile_detail {

// Marks a directory `tau gen` owns, so a later run may write into it and a
// non-empty directory from something else is refused instead of overwritten.
inline constexpr const char* artifact_marker_file = ".tau-artifact";

// RAII: clears definitions<Node>'s process-wide rule table, I/O context and
// type scope for the guard's lifetime and restores the prior state on
// destruction, so a compile starts from and leaves behind a clean type scope.
template <NodeType Node>
struct scoped_clean_definitions {
	scoped_clean_definitions()
		: defs(definitions<Node>::instance()), saved(defs.save()) {
		defs.clear();
	}
	~scoped_clean_definitions() { defs.restore(std::move(saved)); }
	scoped_clean_definitions(const scoped_clean_definitions&) = delete;
	scoped_clean_definitions& operator=(const scoped_clean_definitions&) = delete;
private:
	definitions<Node>& defs;
	typename definitions<Node>::snapshot saved;
};

// An SDK directory always holds TauConfig.cmake; probing only that file keeps
// a directory that merely exists from reading as an SDK.
inline bool has_sdk_config(const std::filesystem::path& dir) {
	std::error_code ec;
	bool found = std::filesystem::exists(dir / "TauConfig.cmake", ec);
	return found && !ec;
}

// The artifact CMakeLists text, from the template compiled into tau, with the
// executable target name substituted. One source of the text for the writer
// and a test that checks the emitted contract.
inline std::string artifact_cmake_text(const std::string& exe_name) {
	std::string text(tau_artifact_cmake_template);
	const std::string placeholder = "@TAU_ARTIFACT_EXE_NAME@";
	for (size_t pos = text.find(placeholder); pos != std::string::npos; ) {
		text.replace(pos, placeholder.size(), exe_name);
		pos = text.find(placeholder, pos + exe_name.size());
	}
	return text;
}

// Writes the artifact CMakeLists from the template compiled into tau,
// substituting the executable target name. The template is one source
// (cmake/tau-artifact-CMakeLists.txt.in), so an installed SDK and the emitted
// artifact state the same build contract.
inline result<bool> write_artifact_cmake(
	const std::string& out_dir, const std::string& exe_name,
	const std::string& verb)
{
	result<bool> r;
	namespace fs = std::filesystem;
	std::ofstream out(fs::path(out_dir) / "CMakeLists.txt");
	if (!out) return r.with_error(code::io_error,
		verb + ": cannot write CMakeLists.txt in " + out_dir);
	out << artifact_cmake_text(exe_name);
	return r.with_value(true);
}

// Writes one compiled-in artifact file. A failed write is the one failure the
// caller can act on, so both the open and the flush are checked.
inline result<bool> write_artifact_text(const std::filesystem::path& path,
	std::string_view text, const std::string& verb)
{
	result<bool> r;
	std::ofstream out(path);
	if (!out) return r.with_error(code::io_error,
		verb + ": cannot write " + path.string());
	out << text;
	if (!out) return r.with_error(code::io_error,
		verb + ": cannot write " + path.string());
	return r.with_value(true);
}

// Writes the artifact's preset files from the copies compiled into tau:
// CMakePresets.json, the platforms.json its include names, and the toolchain
// file that names. The artifact therefore carries no path of the machine that
// ran `gen`: the box is found at configure time, in CMakeLists.txt.
inline result<bool> write_artifact_presets(const std::string& out_dir,
	const std::string& verb) {
	namespace fs = std::filesystem;
	result<bool> r;
	fs::path dir(out_dir);
	std::error_code ec;
	fs::create_directories(dir / "cmake" / "toolchains", ec);
	if (ec) return r.with_error(code::io_error,
		verb + ": cannot create " + (dir / "cmake" / "toolchains").string()
		+ ": " + ec.message());
	TAU_TRY(bool presets_written, write_artifact_text(
		dir / "CMakePresets.json", tau_artifact_presets_json, verb));
	(void)presets_written;
	TAU_TRY(bool platforms_written, write_artifact_text(
		dir / "platforms.json", tau_platforms_json, verb));
	(void)platforms_written;
	// The toolchain files platforms.json names, `_toolchain-mingw` and
	// `_toolchain-arm64`.
	TAU_TRY(bool toolchain_written, write_artifact_text(
		dir / "cmake" / "toolchains" / "mingw-w64-x86_64.cmake",
		tau_mingw_toolchain, verb));
	(void)toolchain_written;
	TAU_TRY(bool arm64_written, write_artifact_text(
		dir / "cmake" / "toolchains" / "aarch64-linux-gnu.cmake",
		tau_aarch64_toolchain, verb));
	(void)arm64_written;
	return r.with_assert_check_value(true);
}

// Runs the SDK's build script through the portable process helper. The script
// writes configure.log and build.log into the artifact directory and prints
// the failing stage; its exit code is the verdict. The check that out_exe
// exists only catches a script that reported success without copying. The
// script output goes to compile.log there, and a failure reports its end.
// `native` selects the running tau's own build over a platform preset;
// `target_sdk`, when not empty, is the SDK the configure resolves from.
// The script is stopped after compile_build_timeout seconds (0 = never).
// Returns the program path: out_exe, or out_exe.js for a wasm build.
inline result<std::string> run_compile_script(const std::string& sdk_dir,
	const std::string& artifact_dir, const std::string& out_exe,
	const std::string& cxx, const std::string& preset,
	const std::vector<std::string>& extra_args, const std::string& target_sdk,
	bool native)
{
	result<std::string> r;
	namespace fs = std::filesystem;
	fs::path script = fs::path(sdk_dir) / "cmake" / "tau-compile.cmake";
	std::error_code ec;
	if (!fs::exists(script, ec) || ec)
		return r.with_error(code::not_found,
			"compile: the SDK has no cmake/tau-compile.cmake at " + sdk_dir);

	// The script reads TAU_EXTRA_ARGS as a cmake list, so a -G <gen> pair and
	// each -D item arrive as separate elements.
	std::string joined;
	for (const auto& a : extra_args) {
		if (!joined.empty()) joined.push_back(';');
		joined += a;
	}

	// Build beside the destination and move it into place only after a zero
	// exit, so a failed build never removes an output that already exists.
	// The script writes a native program to TAU_OUTPUT and a wasm one to
	// TAU_OUTPUT.js plus TAU_OUTPUT.wasm, so the temporaries add the
	// suffix after ".tau-build".
	const std::string temp_base = out_exe + ".tau-build";
	std::vector<std::string> outputs = { out_exe, out_exe + ".js",
		out_exe + ".wasm" };
	std::vector<std::string> temps = { temp_base, temp_base + ".js",
		temp_base + ".wasm" };

	std::vector<std::string> argv = { "cmake" };
	argv.push_back("-DTAU_ARTIFACT_DIR=" + artifact_dir);
	argv.push_back("-DTAU_OUTPUT=" + temp_base);
	if (native) argv.push_back("-DTAU_NATIVE=ON");
	else argv.push_back("-DTAU_PRESET=" + (preset.empty()
		? std::string("release") : preset));
	if (!joined.empty()) argv.push_back("-DTAU_EXTRA_ARGS=" + joined);
	// A platform SDK is passed only when one was found: for a tau preset name
	// that maps to another platform, the script reads it from the artifact's
	// own presets file instead.
	if (!target_sdk.empty()) argv.push_back("-DTAU_SDK_DIR=" + target_sdk);
	if (!cxx.empty()) argv.push_back("-DTAU_CXX=" + cxx);
	fs::path deps = fs::path(target_sdk.empty() ? sdk_dir : target_sdk)
		/ "tau-sdk-deps.cmake";
	if (fs::exists(deps, ec) && !ec)
		argv.push_back("-DTAU_SDK_DEPS=" + deps.string());
	argv.push_back("-P");
	argv.push_back(script.string());

	// A stale temporary from an earlier run must not read as this run's
	// success; the script copies only after a clean build.
	for (const auto& t : temps) fs::remove(t, ec);
	// cmake -P prints every message on stderr. A report keeps only the
	// start of an output, and the cause of a failure is at its end.
	const std::string script_log = (fs::path(artifact_dir) / "compile.log")
		.string();
	spawn_options opts;
	opts.stderr_path = script_log;
	const size_t timeout = std::min<size_t>(compile_build_timeout.get(),
		(size_t) std::numeric_limits<int>::max());
	auto spawned = spawn_capture(argv, (int) timeout,
		[](int c) { return c == 0; }, opts);
	if (!spawned.has_value()) {
		const bool timed_out = report_has_attr(spawned.report(),
			label::timeout);
		r.merge(std::move(spawned));
		std::ifstream log_in(script_log, std::ios::binary);
		std::ostringstream log_text;
		log_text << log_in.rdbuf();
		std::string_view tail = log_text.view();
		while (!tail.empty() && (tail.back() == '\n' || tail.back() == '\r'))
			tail.remove_suffix(1);
		// Long enough to hold the cmake error above the call stack of tau_exit.
		const size_t tail_len = 400;
		if (tail.size() > tail_len) tail.remove_prefix(tail.size() - tail_len);
		return r.with_error(code::runtime_error, timed_out
			? "compile: the cmake build was stopped by the build timeout "
				"(--compile-build-timeout / `set compilebuildtimeout` / "
				"TAU_COMPILE_BUILD_TIMEOUT)"
			: "compile: the cmake build failed",
			{{label::path, script_log},
			 {label::value, tail.empty() ? std::string("(no output)")
				: truncate_for_message(tail, tail_len)}});
	}
	auto move_into_place = [&](const std::string& from, const std::string& to) {
		if (!fs::exists(from, ec) || ec) return false;
		fs::remove(to, ec);
		fs::rename(from, to, ec);
		return !ec;
	};
	if (move_into_place(temps[0], outputs[0]))
		return r.with_value(out_exe);
	// A wasm program is its js loader, which loads the .wasm beside it.
	if (!move_into_place(temps[1], outputs[1]))
		return r.with_error(code::not_found,
			"compile: the cmake build produced no " + out_exe);
	if (!move_into_place(temps[2], outputs[2]))
		return r.with_error(code::not_found,
			"compile: the cmake build produced no " + outputs[2]);
	return r.with_value(outputs[1]);
}

// Writes to f the includes, the embedded spec of d and the option handling
// every artifact main starts with, up to tau_init.
inline void emit_main_head(const program_desc& d, std::ostream& f) {
	f <<
		"// Auto-generated driver for a tau-compiled spec.\n"
		"// tau.h first: the amalgamation orders solver/interpreter internals\n"
		"// that piecemeal includes cannot.\n"
		"#include \"tau.h\"\n"
		"#include \"tau_pack.h\"\n"
		"#include \"table_step_provider.h\"\n"
		"#include \"cli_options.h\"\n"
		"#include <chrono>\n"
		"#include <cstdio>\n"
		"#include <cstdlib>\n"
		"#include <cstring>\n"
		"#include <iostream>\n"
		"#include <map>\n"
		"#include <memory>\n"
		"#include <string>\n"
		"#include <utility>\n"
		"#include <vector>\n"
#ifdef TAU_CODEGEN_ARTIFACT_PREINST
		"#include \"artifact_pack_extern.h\"\n"
#endif
		"using namespace std;\n"
		"using namespace idni::tau_lang;\n"
		"using node_t = tau_pack::node_t;\n"
		"using tref = ::idni::tref;\n"
		"\n"
		"static const char* spec_src() {\n"
		"\treturn \"" << idni::escapes::encode(d.spec_src,
			idni::escapes::c_like) << "\";\n"
		"}\n"
		"\n"
		"int main(int argc, char** argv) {\n"
		"\tif (argc > 1 && strcmp(argv[1], \"--print-spec\") == 0) {\n"
		"\t\tputs(spec_src());\n"
		"\t\treturn 0;\n"
		"\t}\n"
		"\tvector<string> args;\n"
		"\tfor (int i = 0; i < argc; ++i) args.push_back(argv[i]);\n"
		"\tidni::cli cl(\"program\", args, idni::cli::commands{}, \"\",\n"
		"\t\ttau_cli_options(cli_option_set::artifact));\n"
		"\tcl.set_help_header(\"Usage: program [<options>]\");\n"
		"\tif (cl.process_args() != 0) return cl.status();\n"
		"\tauto opts = cl.get_processed_options();\n"
		"\tif (opts[\"help\"].get<bool>()) return cl.help(), 0;\n"
		"\tif (opts[\"version\"].get<bool>())\n"
		"\t\treturn cout << full_version << \"\\n\", 0;\n"
		"\tif (opts[\"license\"].get<bool>()) return cout << license, 0;\n"
		"\tlogging::set_filter(\n"
		"\t\ttau_cli_parse_severity(opts[\"severity\"].get<string>()));\n"
		"\tprint_json = opts[\"json\"].get<bool>();\n"
		"\tbool quit_on_idle = opts[\"quit\"].get<bool>();\n"
		"\tbool print_benchmarks = opts[\"benchmarks\"].get<bool>();\n"
		"\tauto init = tau_init<node_t>();\n"
		"\tinit.print_pending();\n"
		"\tif (!init.has_value()) return 1;\n"
		;
}

// Writes to f the end of every artifact main: prints the run's report and
// leaves.
inline void emit_main_tail(std::ostream& f) {
	f <<
		"\trun_r.report().print(cerr);\n"
		"\tbool run_ok = run_r.has_value() && run_r.value();\n"
		"\tif (print_benchmarks)\n"
		"\t\tcerr << \"run: \" << std::chrono::duration<double, std::milli>(\n"
		"\t\t\tstd::chrono::steady_clock::now() - run_start).count()\n"
		"\t\t\t<< \" ms\\n\";\n"
		"\t// Flush and leave without running static destructors: the pack's\n"
		"\t// static state (caches, pools, the leaked cvc5 term manager) has no\n"
		"\t// safe cross-TU destruction order, and a buffered-stdout artifact\n"
		"\t// must not lose its written outputs to a teardown crash.\n"
		"\tcout.flush();\n"
		"\tfflush(nullptr);\n"
		"\t_Exit(run_ok ? 0 : 1);\n"
		"}\n";
}

// The artifact's table main: replays the baked ba-type table, rebuilds
// atoms/strategy/templates/witnesses from the desc, and drives the same
// run_loop() the tau binary's own `run` uses. Whether a witness is a baked
// constant (edge_witnesses) or solved per step (edge_witness_templates) is a
// table_step_provider construction detail, not a different main.
inline void emit_main(const program_desc& d, std::ostream& f) {
	emit_main_head(d, f);
	f <<
		"\t// Replay the emitting process's ba-type registry so every baked\n"
		"\t// numeric type id resolves to the same type here; entries the\n"
		"\t// artifact's own static init already registered assert by identity.\n";
	for (auto& e : d.ba_type_table) {
		f << "\t{\n\t\ttref tt = ";
		switch (e.kind) {
		case ba_type_entry::recipe::reserved:
			f << e.name << "_type<node_t>()";
			break;
		case ba_type_entry::recipe::family:
			f << "pack_type_tree<node_t>(\"" << e.name << "\"";
			if (e.param) f << ", (unsigned short)" << *e.param;
			f << ")";
			break;
		case ba_type_entry::recipe::syntactic:
			f << "ba_types_detail::make_syntactic_type_tree"
			     "<node_t>(\"" << e.name << "\")";
			break;
		}
		f << ";\n"
		  << "\t\tif (!tt || ba_types<node_t>::id(tt) != " << e.id
		  << ") {\n"
		  << "\t\t\tfprintf(stderr, \"ba-type replay mismatch for "
		  << e.name;
		if (e.param) f << "[" << (int)*e.param << "]";
		f << " (expected id " << e.id << ")\\n\");\n"
		     "\t\t\treturn 2;\n\t\t}\n\t}\n";
	}
	f << "\n\tmap<string, tref> atoms;\n";
	for (auto& a : d.atoms)
		f << "\tatoms[\"" << a.prop << "\"] = " << a.ground_expr << ";\n";
	f << "\n\tcodegen::strategy strat;\n"
	  << "\tstrat.num_states = " << d.num_states << ";\n"
	  << "\tstrat.initial_state = " << d.initial_state << ";\n"
	  << "\tstrat.num_inputs = " << d.inputs.size() + d.step_guard_ks.size() << ";\n"
	  << "\tstrat.edges.resize(" << d.num_states << ");\n"
	  << "\tvector<vector<vector<tref>>> templates("
	  << d.num_states << ");\n"
	  << "\tvector<vector<vector<bool>>> template_is_counter("
	  << d.num_states << ");\n"
	  << "\tvector<vector<vector<pair<string, tref>>>> "
	     "edge_witnesses(" << d.num_states << ");\n"
	  << "\tvector<int_t> step_guard_ks{";
	for (size_t k = 0; k < d.step_guard_ks.size(); ++k)
		f << (k ? ", " : "") << d.step_guard_ks[k];
	f << "};\n";
	// witness_ctors keys by the field's sanitized cpp_name; edge_witnesses
	// keys by the real output variable name (table_step_provider's own doc
	// comment, matching flag_outputs and the output streams) -- map one to
	// the other.
	std::map<std::string, std::string> witness_var_of_cpp_name;
	for (auto& fld : d.outputs)
		if (fld.kind == field_kind::witness)
			witness_var_of_cpp_name[fld.cpp_name] = fld.prop;
	for (size_t s = 0; s < d.num_states; ++s) {
		if (s >= d.edges.size()) break;
		for (auto& e : d.edges[s]) {
			f << "\tstrat.edges[" << s << "].push_back({{";
			for (size_t k = 0; k < e.guard.size(); ++k)
				f << (k ? ", " : "") << (int)e.guard[k];
			f << "}, " << e.dst << "});\n";
			f << "\ttemplates[" << s << "].push_back({";
			for (size_t k = 0; k < e.witness_template_props.size(); ++k) {
				const bool neg = k < e.witness_template_negated.size()
					&& e.witness_template_negated[k];
				f << (k ? ", " : "")
				  << (neg ? "tree<node_t>::build_wff_neg(" : "")
				  << "atoms.at(\"" << e.witness_template_props[k] << "\")"
				  << (neg ? ")" : "");
			}
			f << "});\n";
			f << "\ttemplate_is_counter[" << s << "].push_back({";
			for (size_t k = 0; k < e.witness_template_is_counter.size(); ++k)
				f << (k ? ", " : "")
				  << (e.witness_template_is_counter[k] ? "true" : "false");
			f << "});\n";
			f << "\tedge_witnesses[" << s << "].push_back({";
			for (size_t k = 0; k < e.witness_ctors.size(); ++k) {
				const auto& [cpp_name, expr] = e.witness_ctors[k];
				f << (k ? ", " : "") << "{\""
				  << witness_var_of_cpp_name.at(cpp_name) << "\", " << expr
				  << "}";
			}
			f << "});\n";
		}
	}
	f << "\n\tvector<pair<string, tref>> input_atoms;\n";
	for (auto& fld : d.inputs)
		f << "\tinput_atoms.emplace_back(\"" << fld.prop
		  << "\", atoms.at(\"" << fld.prop << "\"));\n";
	f << "\tvector<string> flag_outputs;\n";
	for (auto& v : d.flag_output_vars)
		f << "\tflag_outputs.push_back(\"" << v << "\");\n";
	f << "\n\tio_context<node_t> ctx;\n";
	for (auto& s : d.input_streams) {
		if (s.bind == stream_desc::binding::file)
			f << "\tctx.add_input_file(\"" << s.name << "\", "
			  << s.ba_type << ", \""
			  << idni::escapes::encode(s.filename, idni::escapes::c_like)
			  << "\");\n";
		else
			f << "\tctx.add_input_console(\"" << s.name << "\", "
			  << s.ba_type << ");\n";
	}
	for (auto& s : d.output_streams) {
		// The warm-up of a clause (pin_written_warm_ups) is solved like
		// any output but never printed, so it gets no console label.
		if (s.name == "o__warmup")
			f << "\tctx.add_output(\"" << s.name << "\", "
			  << s.ba_type << ", make_shared<vector_output_stream>());\n";
		else if (s.bind == stream_desc::binding::file)
			f << "\tctx.add_output_file(\"" << s.name << "\", "
			  << s.ba_type << ", \""
			  << idni::escapes::encode(s.filename, idni::escapes::c_like)
			  << "\");\n";
		else
			f << "\tctx.add_output_console(\"" << s.name << "\", "
			  << s.ba_type << ");\n";
	}
	f <<
		"\n\tauto provider = make_shared<table_step_provider<node_t>>(\n"
		"\t\tstd::move(strat), std::move(input_atoms), std::move(flag_outputs),\n"
		"\t\tstd::move(edge_witnesses), std::move(templates), "
		"std::move(template_is_counter),\n"
		"\t\tstd::move(step_guard_ks)";
	if (d.data_game) {
		f << ",\n\t\ttable_step_provider<node_t>::from_start{ {";
		for (size_t k = 0; k < d.history.size(); ++k)
			f << (k ? ",\n\t\t\t" : "\n\t\t\t") << d.history[k];
		f << " }, " << d.lookback << " }";
	}
	f << ");\n"
		// Captured before the move below: step()'s input filter needs this
		// to tell which declared inputs a given step actually consults,
		// the same way the general solve path uses ubt_ctn.
		"\tvector<tref> live_probe_atoms = provider->live_probe_atoms();\n"
		"\tauto interp = interpreter<node_t>::make_table_interpreter(\n"
		"\t\tctx, std::move(provider), " << d.lookback << ", "
		<< d.highest_initial_pos << ", live_probe_atoms);\n"
		"\tif (!interp.has_value()) {\n"
		"\t\tinterp.report().print(cerr);\n"
		"\t\tfprintf(stderr, \"interpreter initialization failed\\n\");\n"
		"\t\treturn 2;\n"
		"\t}\n"
		"\tauto run_start = std::chrono::steady_clock::now();\n"
		"\tauto run_r = interp->run_loop(0, quit_on_idle);\n"
		;
	emit_main_tail(f);
}

// Writes to f the main of a spec `run` executes by solving as it goes: the
// artifact executes the embedded spec with the same interpreter.
inline void emit_solving_main(const program_desc& d, std::ostream& f) {
	emit_main_head(d, f);
	f <<
		"\tauto gi = api<node_t>::get_interpreter(string(spec_src()));\n"
		"\tif (!gi.has_value()) {\n"
		"\t\tgi.print(cerr);\n"
		"\t\treturn 2;\n"
		"\t}\n"
		"\tauto run_start = std::chrono::steady_clock::now();\n"
		"\tauto run_r = api<node_t>::run(gi.value(), quit_on_idle);\n";
	emit_main_tail(f);
}

// A copy of ctx whose streams are all console streams: an interpreter
// built from it opens (and so truncates) no file.
template <NodeType Node>
io_context<Node> without_files(const io_context<Node>& ctx) {
	io_context<Node> c = ctx;
	for (auto& [var, sid] : c.inputs) sid = 0;
	for (auto& [var, sid] : c.outputs) sid = 0;
	for (auto& [root, layout] : c.adt_streams) layout.stream_id = 0;
	c.input_remaps.clear();
	c.output_remaps.clear();
	return c;
}

// The platform a box serves, read from its layout: <base>/<platform>/sdk in a
// source tree, <prefix>/.../tau/sdk/<platform>/lib/cmake/Tau installed. A box
// whose path names no platform (the flat Windows zip) falls back to the
// marker beside its cmake/.
inline std::string box_platform(const std::filesystem::path& sdk_dir) {
	namespace fs = std::filesystem;
	if (sdk_dir.filename() == "sdk")
		return sdk_dir.parent_path().filename().string();
	if (sdk_dir.filename() == "Tau"
		&& sdk_dir.parent_path().filename() == "cmake"
		&& sdk_dir.parent_path().parent_path().filename() == "lib")
		return sdk_dir.parent_path().parent_path().parent_path()
			.filename().string();
	std::ifstream f(sdk_dir / "cmake" / "tau-this-platform.txt");
	std::string line;
	if (!f || !std::getline(f, line)) return {};
	while (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
		line.pop_back();
	return line;
}

// The package that carries a platform's box: the native box is `tau-sdk`, a
// cross one `tau-sdk-windows-x86_64-mingw`, `tau-sdk-wasm32-emscripten` or
// `tau-sdk-linux-arm64`, named by the store target.
inline std::string sdk_package_suffix(const std::string& platform) {
	if (platform.size() >= 4
		&& platform.compare(platform.size() - 4, 4, "-w64") == 0)
		return "-windows-x86_64-mingw";
	if (platform.size() >= 6
		&& platform.compare(platform.size() - 6, 6, "-arm64") == 0)
		return "-linux-arm64";
	if (platform.find("wasm") != std::string::npos)
		return "-wasm32-emscripten";
	return {};
}

// Maps a `--preset` name to the platform it builds in, from the table compiled
// into tau (cmake/tau_artifact_template.h.in). A platform name is its own
// platform, so the bare `--preset release-w64` the docs allow needs no map
// entry. An unknown name fails here, before any SDK is touched, and names the
// platforms.
inline result<std::string> preset_platform(const std::string& preset) {
	result<std::string> r;
	for (const auto& e : tau_preset_platform_map)
		if (e.preset == preset) return r.with_value(std::string(e.platform));
	for (std::string_view p : tau_platform_names)
		if (p == preset) return r.with_value(std::string(p));
	std::string known;
	for (std::string_view p : tau_platform_names) {
		if (!known.empty()) known += ", ";
		known += p;
	}
	return r.with_error(code::invalid_argument,
		"unknown --preset '" + preset + "'; known platforms: " + known);
}

// The build-tree SDK of one platform: <exe_dir>/../<platform>/sdk. Empty when
// the executable runs outside a source tree or that platform was not built.
inline std::string platform_sdk_dir(const std::string& platform) {
#if defined(__EMSCRIPTEN__)
	(void)platform;
	return {};
#else
	if (platform.empty()) return {};
	std::string self = self_exe_path();
	if (self.empty()) return {};
	namespace fs = std::filesystem;
	fs::path dir = fs::path(self).parent_path() / ".." / platform / "sdk";
	if (!has_sdk_config(dir)) return {};
	return dir.lexically_normal().string();
#endif
}

} // namespace compile_detail

/** @internal @copydoc resolve_sdk_dir @endinternal */
inline result<std::string> resolve_sdk_dir(const std::string& platform) {
	result<std::string> r;
#if defined(__EMSCRIPTEN__)
	(void)platform;
	return r.with_error(code::unsupported_operation,
		"gen/compile need a process model, unavailable in this build");
#else
	namespace fs = std::filesystem;
	if (const char* env = std::getenv("TAU_SDK_DIR"); env && *env) {
		fs::path dir(env);
		if (compile_detail::has_sdk_config(dir))
			return r.with_value(dir.string());
		return r.with_error(code::not_found,
			std::string("tau SDK not found at TAU_SDK_DIR=") + env);
	}
	// A source tree keeps every platform's SDK under <exe_dir>/../<platform>.
	if (!platform.empty()) {
		std::string built = compile_detail::platform_sdk_dir(platform);
		if (!built.empty()) return r.with_value(built);
	}
	std::string self = self_exe_path();
	if (self.empty())
		return r.with_error(code::not_found,
			"tau SDK not found and the executable path is unknown; "
			"set TAU_SDK_DIR");
	fs::path exe_dir = fs::path(self).parent_path();

	// One box per platform under the lib dir of the install prefix, so a box
	// for another platform is simply not this platform's box. With no platform
	// named, every box beside this binary is a candidate and the first wins.
	std::vector<fs::path> candidates;
	// A binary found by its own path builds for this machine with its build
	// folder's SDK, before any installed box.
	if (platform.empty()) candidates.push_back(exe_dir / "sdk");
	auto add_boxes = [&candidates](const fs::path& base,
		const std::string& want)
	{
		if (!want.empty()) {
			candidates.push_back(base / want / "lib" / "cmake" / "Tau");
			return;
		}
		std::error_code ec;
		for (fs::directory_iterator it(base, ec), end; !ec && it != end;
			it.increment(ec))
		{
			std::error_code dir_ec;
			if (!it->is_directory(dir_ec) || dir_ec) continue;
			candidates.push_back(it->path() / "lib" / "cmake" / "Tau");
		}
	};
	add_boxes(exe_dir / ".." / "lib" / "tau" / "sdk", platform);
	add_boxes(exe_dir / ".." / "lib64" / "tau" / "sdk", platform);
	// The multiarch lib dir carries a name this code cannot spell, so scan
	// <exe_dir>/../lib/*/tau/sdk for the box or for the platform's box.
	{
		std::error_code ec;
		fs::path lib_dir = exe_dir / ".." / "lib";
		for (fs::directory_iterator it(lib_dir, ec), end; !ec && it != end;
			it.increment(ec))
		{
			std::error_code dir_ec;
			if (!it->is_directory(dir_ec) || dir_ec) continue;
			fs::path sdk_base = it->path() / "tau" / "sdk";
			if (!platform.empty()) {
				candidates.push_back(sdk_base / platform / "lib"
					/ "cmake" / "Tau");
				continue;
			}
			std::error_code sdk_ec;
			for (fs::directory_iterator pit(sdk_base, sdk_ec), pend;
				!sdk_ec && pit != pend; pit.increment(sdk_ec))
			{
				std::error_code box_ec;
				if (!pit->is_directory(box_ec) || box_ec) continue;
				candidates.push_back(pit->path() / "lib" / "cmake"
					/ "Tau");
			}
		}
	}
	// A build tree's own SDK, and the flat Windows zip's cmake dir.
	candidates.push_back(exe_dir / "sdk");
	candidates.push_back(exe_dir / "cmake" / "Tau");
	for (const auto& candidate : candidates) {
		if (!compile_detail::has_sdk_config(candidate)) continue;
		// A fallback box (a build tree's own SDK, a flat zip's cmake dir) is
		// this platform's box only when its own marker agrees; a box for
		// another platform must not satisfy the request.
		if (!platform.empty()
			&& compile_detail::box_platform(candidate) != platform)
			continue;
		return r.with_value(candidate.lexically_normal().string());
	}
	return r.with_error(code::not_found,
		"no SDK for " + (platform.empty() ? std::string("this platform")
			: platform) + "; install tau-sdk"
		+ compile_detail::sdk_package_suffix(platform)
		+ " or build it with ./dev preset "
		+ (platform.empty() ? std::string("release") : platform));
#endif
}

// Split from compile_spec so `tau gen` stops before the build and
// `tau compile` continues into the SDK's cmake script.
/** @internal @copydoc gen_spec @endinternal */
template <NodeType Node>
result<codegen_result> gen_spec(
	const std::string& spec_src,
	const std::string& out_dir,
	const std::string& exe_name,
	const std::string& verb)
{
	namespace stdfs = std::filesystem;
	compile_detail::scoped_clean_definitions<Node> clean_defs;
	result<codegen_result> r;

	stdfs::path bdir = out_dir.empty()
		? stdfs::current_path() / "spec.build"
		: stdfs::path(out_dir);
	// The emitted presets and the build step both read this path; an absolute
	// one keeps a relative -o from resolving differently inside the configure.
	if (bdir.is_relative()) bdir = stdfs::current_path() / bdir;
	bdir = bdir.lexically_normal();
	std::error_code ec;
	// A non-empty directory is only an artifact directory when `tau gen` wrote
	// the marker, so `-o .` cannot overwrite a tree's own files.
	if (stdfs::exists(bdir, ec) && !ec) {
		std::error_code mec, eec;
		bool has_marker = stdfs::exists(
			bdir / compile_detail::artifact_marker_file, mec);
		bool empty = stdfs::is_empty(bdir, eec);
		if (eec) return r.with_error(code::io_error,
			verb + ": cannot read " + bdir.string() + ": " + eec.message());
		if (!has_marker && !empty)
			return r.with_error(code::io_error, verb + ": " + bdir.string()
				+ " is not empty and carries no "
				+ std::string(compile_detail::artifact_marker_file));
	}
	stdfs::create_directories(bdir, ec);
	if (ec) return r.with_error(code::io_error,
		verb + ": cannot create output dir " + bdir.string()
		+ ": " + ec.message());

	// 1. Parse the spec via get_spec -- the interpreter's own grammar
	// (trailing '.', stream/rec-relation definitions) -- through the same
	// nso_rr/normalizer pipeline get_interpreter uses.
	using tau_api = api<Node>;
	TAU_TRY(tref spec_tree, tau_api::get_spec_as_written(spec_src));
	if (!spec_tree) return r.with_error(code::parse_error,
		verb + ": failed to parse spec");
	// Each clause keeps the warm-up it is written with.
	TAU_TRY(spec_tree, tau_api::pin_main(spec_tree));
	spec_tree = tree<Node>::reget(spec_tree);
	TAU_TRY(auto nso_rr, get_nso_rr<Node>(spec_tree));
	TAU_TRY(tref applied, nso_rr_apply<Node>(nso_rr));
	TAU_TRY(tref fm, normalizer<Node>(applied));
	// fm is checked before make_interpreter runs, so it is never a bare-reparsed atom.
	TAU_TRY(bool has_free, has_free_vars<Node>(fm));
	if (has_free) return r.with_error(code::invalid_argument,
		"the specification contains free variables");

	// 2. Follow what `run` executes: make_interpreter chooses it, so a
	// program and a run of the spec make the same moves. When the run
	// plays the Mealy view of the data game's strategy, a finite machine
	// of at most compile_max_table_edges edges, the program carries that
	// machine. Otherwise the run solves as it goes (each step, the game or
	// the abstraction when it starts, and a revision when the update
	// stream asks for one), and the program executes the embedded spec the
	// same way.
	std::optional<ltl_aba_solution<Node>> sol;
	bool solves_each_step = false;
	// the output streams `run` prints, with their types
	std::vector<std::pair<std::string, size_t>> run_outputs;
	auto run_r = interpreter<Node>::make_interpreter(fm,
		compile_detail::without_files(
			*definitions<Node>::instance().get_io_context()));
	if (run_r.has_value()) {
		const auto& run = run_r.value();
		for (const auto& [var, _] : run.outputs)
			if (!interpreter<Node>::is_excluded_output(var))
				run_outputs.emplace_back(get_var_name<Node>(var),
					run.ctx.type_of(var));
		// a run revises its spec with what the update stream u carries,
		// which only a solving program does too
		const bool revises = std::ranges::any_of(run_outputs,
			[](const auto& o) { return o.first == "u"
				&& o.second == get_ba_type_id<Node>(tau_type<Node>()); });
		size_t edges = 0;
		if (run.cached_solution)
			for (const auto& es : run.cached_solution->aut.edges)
				edges += es.size();
		const size_t max_edges = compile_max_table_edges.get();
		if (run.plays_data_game() && run.cached_solution && !revises
			&& max_edges > 0 && edges <= max_edges)
				sol = run.cached_solution;
		else solves_each_step = true;
	}
	r.merge(std::move(run_r));
	if (!sol && !solves_each_step) {
		// `run` executes nothing, which is not a verdict: the spec may be
		// realizable or undecided. The message follows what `realizable`
		// decides, and an undecided check's report names its budget or
		// reason.
		auto real = is_ctl_star_realizable<Node>(fm, 0, false);
		if (!real.has_value()) {
			r.merge(std::move(real));
			return r.with_error(code::solver_error,
				verb + ": the realizability of the spec is "
				"UNKNOWN, so no program was built");
		}
		if (!real.value()) return r.with_error(code::unsat,
			verb + ": spec is UNREALIZABLE");
		r.merge(std::move(real));
		return r.with_error(code::unsupported_operation,
			verb + ": the spec is realizable, but `run` executes no "
			"strategy for it, so no program was built");
	}

	// 3. Build the program_desc of the strategy via the one data-driven
	// emit path (build_program_desc picks flag-only vs witness-bearing
	// itself; this wraps that choice rather than repeating it). A
	// synthesis-time refusal (PWR x witness, or a witness owner that
	// declines) surfaces here as a compile error, not a crash.
	program_desc d;
	if (sol) {
		const std::string class_name = "tau_program";
		TAU_TRY(d, build_program_desc<Node>(*sol, class_name,
			/*revisable=*/false,
			definitions<Node>::instance().get_io_context()));
	}
	d.spec_src = spec_src;
	// A stream the strategy reads no atom of still prints, as in `run`.
	if (sol) for (const auto& [name, type] : run_outputs) {
		if (std::ranges::any_of(d.output_streams,
			[&](const auto& s) { return s.name == name; })) continue;
		stream_desc sd;
		sd.name = name;
		sd.ba_type = type;
		auto& ctx = *definitions<Node>::instance().get_io_context();
		tref var = build_canonized_io_var<Node>(name);
		if (auto it = ctx.outputs.find(var);
			it != ctx.outputs.end() && it->second != 0)
		{
			sd.bind = stream_desc::binding::file;
			sd.filename = dict(it->second);
		}
		d.output_streams.push_back(std::move(sd));
	}

	// Every flag output field must key its stream on a single variable --
	// emit_main's flag_outputs list has no other way to name its guard slot.
	for (size_t k = 0; k < d.flag_output_vars.size(); ++k)
		if (d.flag_output_vars[k].empty())
			return r.with_error(code::internal_error, verb + ": flag output '"
				+ d.outputs[k].prop + "' has no single variable "
				"to key its stream on; the artifact cannot be emitted");

	// 4. Emit the artifact's main.cpp: the table main playing the
	// strategy, or the solving main.
	{
		std::ofstream f(bdir / "main.cpp");
		if (!f) return r.with_error(code::io_error,
			verb + ": cannot write main.cpp in " + bdir.string());
		if (solves_each_step) compile_detail::emit_solving_main(d, f);
		else compile_detail::emit_main(d, f);
	}

	// 5. Emit CMakeLists.txt from the template compiled into tau.
	TAU_TRY(bool cmake_written, compile_detail::write_artifact_cmake(
		bdir.string(), exe_name, verb));
	(void)cmake_written;

	// 6. Emit the presets, the platform map and the toolchain they name, from
	// the same templates compiled into tau. No SDK is consulted: `tau gen` is
	// code generation only, and the artifact carries no path of this machine.
	TAU_TRY(bool presets_written, compile_detail::write_artifact_presets(
		bdir.string(), verb));
	(void)presets_written;

	// Last, the marker that lets a later `tau gen -o` into this directory.
	TAU_TRY(bool marker_written, compile_detail::write_artifact_text(
		bdir / compile_detail::artifact_marker_file, "tau artifact\n", verb));
	(void)marker_written;

	codegen_result res;
	res.exe_path = bdir.string();
	return r.with_assert_check_value(std::move(res));
}

/** @internal @copydoc compile_spec @endinternal */
template <NodeType Node>
result<codegen_result> compile_spec(
	const std::string& spec_src,
	const std::string& out_exe,
	const std::string& build_dir,
	const std::string& cxx,
	const std::string& preset,
	const std::vector<std::string>& extra_args)
{
	namespace stdfs = std::filesystem;
	result<codegen_result> r;

	// 1. Emit the artifact (parse, synthesize, write main.cpp and the project
	// files from the templates compiled into tau). The messages name the verb.
	TAU_TRY(auto gen, gen_spec<Node>(spec_src, build_dir, "program",
		"compile"));

	// 2. Build through the SDK's cmake script, which writes its logs into the
	// artifact dir and prints the failing stage. Without --preset the build is
	// native: the SDK of the running tau, found by the path of its binary, and
	// cmake's own compiler. With --preset the name maps to a platform whose
	// SDK box and toolchain build the artifact.
	std::string dest = out_exe.empty()
		? (stdfs::path(gen.exe_path) / "program").string()
		: out_exe;
	// The flag wins over the environment variable.
	std::string cc = cxx;
	if (cc.empty())
		if (const char* env = std::getenv("TAU_CXX"); env && *env)
			cc = env;

	if (preset.empty()) {
		TAU_TRY(std::string sdk_dir, resolve_sdk_dir());
		TAU_TRY(std::string built, compile_detail::run_compile_script(
			sdk_dir, gen.exe_path, dest, cc, "", extra_args, sdk_dir,
			true));
		codegen_result res;
		res.exe_path = std::move(built);
		return r.with_assert_check_value(std::move(res));
	}
	TAU_TRY(std::string platform, compile_detail::preset_platform(preset));
	TAU_TRY(std::string sdk_dir, resolve_sdk_dir(platform));
	// The SDK the configure resolves dependencies from is the platform's box
	// when this tree has it, else the box that was found.
	std::string target_sdk;
	if (const char* env = std::getenv("TAU_SDK_DIR"); env && *env)
		target_sdk = env;
	else {
		target_sdk = compile_detail::platform_sdk_dir(platform);
		if (target_sdk.empty()) target_sdk = sdk_dir;
	}
	TAU_TRY(std::string built, compile_detail::run_compile_script(
		sdk_dir, gen.exe_path, dest, cc, platform, extra_args, target_sdk,
		false));

	codegen_result res;
	res.exe_path = std::move(built);
	return r.with_assert_check_value(std::move(res));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TAU_COMPILE_TMPL_H__
