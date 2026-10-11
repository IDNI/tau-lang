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

tref create_spec(const char* spec) {
	return get_nso_rr<node_t>(tau::get(spec).value_or(nullptr)).value().main->get();
}

tref create_spec(io_context<node_t>& ctx, const char* spec) {
	return get_nso_rr<node_t>(ctx, tau::get(spec).value_or(nullptr)).value().main->get();
}

std::optional<assignment<node_t>> run_test(tref spec, io_context<node_t>& ctx,
	const size_t& times)
{
	using node = node_t;
#ifdef DEBUG
	std::cout << "run_test/------------------------------------------------------\n";
	std::cout << "run_test/sample: " << tree<node_t>::get(spec).dump_to_str() << "\n";
#endif // DEBUG

	auto intprtr = interpreter<node>::make_interpreter(spec, ctx);
	if (intprtr.has_value()) {
		// we read the inputs only once (they are always empty in this test suite)

		for (size_t i = 0; i < times; ++i) {
// 			// we execute the i-th step

			auto step_res = intprtr.value().step();
			if (!step_res.has_value()) break;
			auto [out, _] = step_res.value();

			// The output can be empty if all variables have been assigned in previous steps
			if (!out.has_value()) {
				intprtr.value().memory.clear();
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
					intprtr.value().memory.clear();
					break;
				}
			}
			std::cout << "\n";
#endif // DEBUG
		}

		return intprtr.value().memory;
	}
	return {};
}

std::optional<assignment<node_t>> run_test(const char* sample,
	const size_t& times)
{
	io_context<node_t> ctx;
	tref spec = create_spec(ctx, sample);
	return run_test(spec, ctx, times);
}

std::optional<assignment<node_t>> run_test(const char* sample,
	io_context<node_t>& ctx, const size_t& times)
{
	tref spec = create_spec(ctx, sample);
	return run_test(spec, ctx, times);
}

