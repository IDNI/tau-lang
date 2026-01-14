// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <random>

#include "test_init.h"
#include "test_tau_helpers.h"
#ifdef DEBUG // in release it is included with tau.h
#	include "interpreter.h"
#endif

std::string random_file(const std::string& extension = ".out", const std::string prefix = "/tmp/") {
	// define the characters to use in the random string
	const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	// length of the random string
	const size_t length = 10;
	// random number generator
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
	std::ostringstream oss;
	// add default location
	oss << prefix;
	// generate random string
	for (size_t i = 0; i < length; ++i) oss << charset[dist(generator)];
	// append the file extension and return the file name
	oss << extension;
	return oss.str();
}

tref create_spec(const char* spec) {
	return get_nso_rr<node_t>(tau::get(spec)).value().main->get();
}

tref create_spec(io_context<node_t>& ctx, const char* spec) {
	return get_nso_rr<node_t>(ctx, tau::get(spec)).value().main->get();
}

std::optional<assignment<node_t>> run_test(tref spec, io_context<node_t>& ctx,
	const size_t& times)
{
	using node = node_t;
#ifdef DEBUG
	std::cout << "run_test/------------------------------------------------------\n";
	std::cout << "run_test/sample: " << LOG_FM_DUMP(spec) << "\n";
#endif // DEBUG

	auto intprtr = interpreter<node>::make_interpreter(spec, ctx);
	if (intprtr) {
		// we read the inputs only once (they are always empty in this test suite)

		for (size_t i = 0; i < times; ++i) {
// 			// we execute the i-th step

			auto [out, _] = intprtr.value().step();

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
				std::cout << TAU_TO_STR(var) << " <- " << TAU_TO_STR(value) << " ... ";
				if (tref io_var = tau::get(value).find_top(is<node_t, tau::io_var>); io_var) {
					std::cout << "run_test/output[" << i << "]: unexpected io_var " << TAU_TO_STR(io_var) << "\n";
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

