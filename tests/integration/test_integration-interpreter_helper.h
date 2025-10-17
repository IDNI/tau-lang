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

struct output_console {
	output_console() = default;
	output_console(const tref type)
		: _type(get_ba_type_id<node_t>(type)) {}

	bool write(const assignment<node_t>& outputs) const {
		// for each stream in out.streams, write the value from the solution
		for (const auto& [var, value]: outputs)
			std::cout << tau::get(var) << " := " << tau::get(value) << "\n";
		return true; // success (always)
	}

	size_t type_of(tref) const { return _type; }

	void add_output(tref v, size_t, size_t) {
		if (!streams.contains(v)) streams.emplace(v, v);
	}

	void rebuild (const auto&) {};

	assignment<node_t> streams;
	size_t _type = get_ba_type_id<node_t>(sbf_type<node_t>());
};

struct input_vector {

	input_vector() = default;
	input_vector(std::vector<assignment<node_t>>& inputs) : inputs(
		std::move(inputs)) {}
	input_vector(const tref type)
		: _type(get_ba_type_id<node_t>(type)) {}
	input_vector(std::vector<assignment<node_t>>& inputs,
		const tref type) : inputs(std::move(inputs)),
			_type(get_ba_type_id<node_t>(type)) {}

	std::optional<assignment<node_t>> get() const {
		if (inputs.empty()) return { assignment<node_t>{} };
		if (current == inputs.size()) return {};
		return inputs[current];
	}

	std::pair<std::optional<assignment<node_t>>, bool> read(
		const auto&, const size_t)
	{
		if (inputs.empty()) return { assignment<node_t>{}, false };
		if (current >= inputs.size()) return { assignment<node_t>{}, false };
		return { inputs[current++], false };
	}

	size_t type_of(tref) const { return _type; }

	void add_input(tref v, size_t, size_t) {
		if (!streams.contains(v)) streams.emplace(v, v);
	}

	void rebuild (const auto&) {};

	std::vector<assignment<node_t>> inputs;
	assignment<node_t> streams;
	size_t current = 0;
	size_t _type = get_ba_type_id<node_t>(sbf_type<node_t>());
};

void build_input(const std::string& name,
	const std::vector<std::string>& values, const tref type,
	auto& assgn)
{
	size_t t = 0;
	for (const auto& val : values) {
		size_t ba_type = get_ba_type_id<node_t>(type);
		auto in_var = build_in_var_at_n<node_t>(name, t, ba_type);
		auto v = ba_constants<node_t>::get(val, type);
		auto v_const = tau::build_bf_ba_constant(v.value().first, ba_type);

		if (assgn.size() <= t) {
			subtree_map<node_t, tref> a;
			a.emplace(in_var, v_const);
			assgn.emplace_back(std::move(a));
		} else assgn[t].emplace(in_var, v_const);
		++t;
	}
}

void build_output(const std::string& name, const std::vector<std::string>& values,
		const tref type, auto& assgn) {
	size_t t = 0;
	for (const auto& val : values) {
		size_t ba_type = get_ba_type_id<node_t>(type);
		auto out_var = build_out_var_at_n<node_t>(name, t, ba_type);
		if (val.empty()) {
			assgn.emplace(out_var, nullptr);
		} else {
			auto v = ba_constants<node_t>::get(val, type);
			auto v_const = tau::build_bf_ba_constant(v.value().first,
				ba_type);
			assgn.emplace(out_var, v_const);
		}
		++t;
	}
}

inline bool matches_output(const auto& assm, const auto& memory) {
	using node = node_t;
	for (const auto& [var, val] : assm) {
		if (val == nullptr) continue;
		if (auto it = memory.find(var); it != memory.end()) {
			if (!are_bf_equal<node_t>(it->second, val)) {
				TAU_LOG_ERROR << TAU_LOG_FM(it->second)
						<< " != " << TAU_LOG_FM(val);
#ifdef DEBUG
				TAU_LOG_INFO << "first:\n"
						<< TAU_LOG_FM_DUMP(it->second);
				TAU_LOG_INFO << "second:\n" << TAU_LOG_FM_DUMP(val);
#endif // DEBUG
				return false;
			}
		} else {
			TAU_LOG_ERROR << TAU_TO_STR(var) << " not found";
			return false;
		}
	}
	return true;
}

tref create_spec(const char* spec) {
	return get_nso_rr<node_t>(tau::get(spec)).value().main->get();
}

std::optional<assignment<node_t>> run_test(const char* sample,
	input_vector& inputs, output_console& outputs, const size_t& times)
{
	tref spec = create_spec(sample);

#ifdef DEBUG
	std::cout << "run_test/------------------------------------------------------\n";
	std::cout << "run_test/sample: " << sample << "\n";
#endif // DEBUG

	spec_context<node_t> ctx;
	auto intprtr = interpreter<node_t, input_vector, output_console>
				::make_interpreter(spec, inputs, outputs, ctx);
	if (intprtr) {
		// we read the inputs only once (they are always empty in this test suite)

		for (size_t i = 0; i < times; ++i) {
			// we execute the i-th step
			auto in = inputs.get();

#ifdef DEBUG
			using node = node_t;
			std::cout << "run_test/input[" << i << "]: ";
			if (in.has_value()) {
				for (const auto& [var, value] : in.value())
					std::cout << TAU_TO_STR(var) << " <- " << TAU_TO_STR(value) << " ... ";
				std::cout << "\n";
			} else std::cout << "{}\n"; // no input
#endif // DEBUG

			auto [out, _ ] = intprtr.value().step ();

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
		const size_t& times, const tref type = sbf_type<node_t>())
{
	input_vector inputs(type);
	output_console outputs(type);
	return run_test(sample, inputs, outputs, times);
}

std::optional<assignment<node_t>> run_test(const char* sample,
	input_vector& inputs, const size_t& times)
{
	output_console outputs;
	return run_test(sample, inputs, outputs, times);
}

std::optional<assignment<node_t>> run_test(const char* sample,
	output_console& outputs, const size_t& times)
{
	input_vector inputs;
	return run_test(sample, inputs, outputs, times);
}

TEST_SUITE("configuration") {

	TEST_CASE("bdd initialization") {
		bdd_init<Bool>();
	}
}

input_vector build_i1_inputs(trefs values) {
	std::vector<assignment<node_t>> assignments;
	size_t t = 0;
	for (tref value : values) {
		assignment<node_t> assignment;
		assignment[build_in_var_at_n_indexed<node_t>(
			1, t, get_ba_type_id<node_t>(tau_type<node_t>()))] = value;
		assignments.push_back(assignment);
		++t;
	}
	input_vector ins(assignments);
	return ins;
}

