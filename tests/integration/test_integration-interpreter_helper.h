// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <filesystem>
#include <random>

#include "test_init.h"
#include "test_tau_helpers.h"
#ifdef DEBUG // in release it is included with tau.h
#	include "interpreter.h"
#endif

// Empty prefix → system temp dir. Paths use `/` so Tau `file("...")`
// literals do not treat Windows backslashes as escapes.
std::string random_file(const std::string& extension = ".out",
	const std::string prefix = "")
{
	const char charset[] =
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	const size_t length = 10;
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
	std::string dir = prefix;
	if (dir.empty()) {
		dir = std::filesystem::temp_directory_path().string();
		if (!dir.empty() && dir.back() != '/' && dir.back() != '\\')
			dir.push_back('/');
		for (char& c : dir) if (c == '\\') c = '/';
	}
	std::ostringstream oss;
	oss << dir;
	for (size_t i = 0; i < length; ++i) oss << charset[dist(generator)];
	oss << extension;
	return oss.str();
}

// Windows refuses unlink while a stream still holds the file; drop the
// handle first (destroy interpreter / io_context) then call this.
inline void remove_temp(const std::string& path) {
	std::error_code ec;
	std::filesystem::remove(path, ec);
}

tref create_spec(io_context<node_t>& ctx, const char* spec) {
	return get_nso_rr<node_t>(ctx, tau::get(spec).value_or(nullptr)).value().main->get();
}

// Runs spec for at most times steps and returns the interpreter's memory.
// A run stops early when a step has no output or awaits an input the
// streams no longer hold; any other failure of make_interpreter or of a
// step is the run's error.
result<assignment<node_t>> run_test(tref spec, io_context<node_t>& ctx,
	const size_t& times)
{
	using node = node_t;
	result<assignment<node_t>> r;
#ifdef DEBUG
	std::cout << "run_test/------------------------------------------------------\n";
	std::cout << "run_test/sample: " << tree<node_t>::get(spec).dump_to_str() << "\n";
#endif // DEBUG

	TAU_TRY(auto intprtr, interpreter<node>::make_interpreter(spec, ctx));
	for (size_t i = 0; i < times; ++i) {
		auto step_res = intprtr.step();
		if (!step_res.has_value() && step_awaiting_input(step_res.report()))
			break;
		TAU_TRY(auto step, std::move(step_res));
		auto& out = step.first;

		// The output can be empty if all variables have been assigned in previous steps
		if (!out.has_value()) {
			intprtr.memory.clear();
#ifdef DEBUG
			std::cout << "run_test/output[" << i << "]: {}\n"; // no output
#endif // DEBUG
			break;
		}

#ifdef DEBUG
		std::cout << "run_test/output[" << i << "]: ";
		for (const auto& [var, value]: out.value()) {
			std::cout << tree<node_t>::get(var).to_str() << " <- " << tree<node_t>::get(value).to_str() << " ... ";
			if (tref io_var = tau::get(value).find_top(is<node_t, tau::io_var>); io_var) {
				std::cout << "run_test/output[" << i << "]: unexpected io_var " << tree<node_t>::get(io_var).to_str() << "\n";
				intprtr.memory.clear();
				break;
			}
		}
		std::cout << "\n";
#endif // DEBUG
	}
	return r.with_value(std::move(intprtr.memory));
}

result<assignment<node_t>> run_test(const char* sample,
	const size_t& times)
{
	io_context<node_t> ctx;
	tref spec = create_spec(ctx, sample);
	return run_test(spec, ctx, times);
}

result<assignment<node_t>> run_test(const char* sample,
	io_context<node_t>& ctx, const size_t& times)
{
	tref spec = create_spec(ctx, sample);
	return run_test(spec, ctx, times);
}

