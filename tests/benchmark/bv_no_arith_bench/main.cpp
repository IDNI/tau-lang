// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// bv_no_arith_bench: closed formulas unrolled from bitvector programs without
// arithmetic, each run through anti_prenexing::anti_prenex in its own child
// and judged against the answer its generator argues for. Up to
// --check-max-width the solver is asked first; when it disagrees with that
// answer the row is GENERATOR? and the engine is not judged on it. CSV on
// stdout, one row per instance, then a summary line per family. The exit code
// is 1 when some answer is WRONG, 2 on a bad command line, 0 otherwise.

#include "engine.h"
#include "families.h"
#include "runner.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

using namespace bv_bench;

namespace {

struct knob {
	std::string name;
	std::string meaning;
};

struct family_entry {
	const char* id;
	const char* name;
	std::vector<knob> knobs;
	std::vector<instance> (*instances)(const grid&);  // null: not built yet
};

const std::vector<family_entry> families = {
	{ "7",  "prediction",
		{ { "k", "lookback" }, { "m", "streams" } }, family_7 },
	{ "8",  "round-robin arbiter",
		{ { "k", "clients" }, { "d", "lookback" } }, family_8 },
	{ "1",  "width thresholds",
		{ { "k", "W1 outputs" }, { "m", "W2 lookback" } }, family_1 },
	{ "2",  "pigeonhole", { { "k", "holes" } }, family_2 },
	{ "4a", "sim generator port",
		{ { "config", "grid row: 0-3 small; 4-6 medium; 7-9 practical" },
		  { "inputs", "input streams" }, { "outputs", "output streams" },
		  { "lookback", "largest lag drawn" },
		  { "lags", "lags drawn from 1..lookback" },
		  { "depth", "decision-tree depth" },
		  { "params", "free parameters" },
		  { "sabotage", "0 none (T); 1 input (F); 2 conflict (F)" } },
		family_4a },
	{ "4b", "templates",           {}, nullptr },
};

const char* const usage =
R"(usage: bv_no_arith_bench [options]
  --family LIST          families to run, from 7,8,1,2,4a,4b
                         (default: every implemented one)
  --widths LIST          bv widths n
  --steps LIST           horizons T; a formula covers the steps 0..T
  --knob NAME=LIST       a family knob, e.g. --knob k=1,2 (repeatable)
  --timeout SECONDS      wall-clock limit per child (default 60)
  --mem MB               address-space cap per child (default 4096)
  --check-max-width N    the solver checks the expected answer up to this
                         width (default 4)
  --check-timeout SECONDS
                         wall-clock limit for that solver check (default 5)
  --seed S, --count N    seed and programs per point, for generated families
  --print-formula        print each formula as a `#` line before its row
  --smoke                the smoke sweep; also what no option at all runs
A LIST is comma-separated unsigned integers. A list or knob not given takes
the family's full default, or its smoke default under --smoke.
)";

void print_usage(std::ostream& os) {
	os << usage;
	for (const family_entry& f : families) {
		if (!f.instances) continue;
		os << "Family " << f.id << " knobs:";
		const char* sep = " ";
		for (const knob& k : f.knobs) {
			os << sep << k.name << " (" << k.meaning << ')';
			sep = ", ";
		}
		os << '\n';
	}
}

[[noreturn]] void usage_error(const std::string& msg) {
	std::cerr << "bv_no_arith_bench: " << msg << "\n\n";
	print_usage(std::cerr);
	std::exit(2);
}

unsigned to_unsigned(const std::string& s, const std::string& flag) {
	if (s.empty() || s.find_first_not_of("0123456789") != std::string::npos
		|| s.size() > 9)
			usage_error(flag + ": not an unsigned integer: '" + s + "'");
	return static_cast<unsigned>(std::stoul(s));
}

double to_seconds(const std::string& s, const std::string& flag) {
	char* end = nullptr;
	const double v = std::strtod(s.c_str(), &end);
	if (*end || !(v > 0))
		usage_error(flag + ": not a positive number: '" + s + "'");
	return v;
}

std::vector<unsigned> to_list(const std::string& s, const std::string& flag) {
	std::vector<unsigned> out;
	std::istringstream is(s);
	for (std::string item; std::getline(is, item, ',');)
		out.push_back(to_unsigned(item, flag));
	if (out.empty()) usage_error(flag + ": empty list");
	return out;
}

struct options {
	std::vector<const family_entry*> selected;
	grid g;
	limits lim{ 60, 4096 };
	unsigned check_max_width = 4;
	double check_timeout_s = 5;
	bool print_formula = false;
};

options parse_options(int argc, char** argv) {
	options opt;
	opt.g.smoke = argc == 1;
	std::string family_list;
	for (int a = 1; a < argc; ++a) {
		const std::string flag = argv[a];
		auto value = [&]() -> std::string {
			if (a + 1 >= argc) usage_error(flag + " needs a value");
			return argv[++a];
		};
		if (flag == "--family") family_list = value();
		else if (flag == "--widths") opt.g.given["width"] = to_list(value(), flag);
		else if (flag == "--steps") opt.g.given["steps"] = to_list(value(), flag);
		else if (flag == "--knob") {
			const std::string kv = value();
			const size_t eq = kv.find('=');
			if (eq == std::string::npos || eq == 0)
				usage_error("--knob wants NAME=LIST, got '" + kv + "'");
			opt.g.given[kv.substr(0, eq)] = to_list(kv.substr(eq + 1), flag);
		}
		else if (flag == "--timeout") opt.lim.timeout_s = to_seconds(value(), flag);
		else if (flag == "--mem") opt.lim.mem_mb = to_unsigned(value(), flag);
		else if (flag == "--check-max-width")
			opt.check_max_width = to_unsigned(value(), flag);
		else if (flag == "--check-timeout")
			opt.check_timeout_s = to_seconds(value(), flag);
		else if (flag == "--seed") opt.g.seed = to_unsigned(value(), flag);
		else if (flag == "--count") opt.g.count = to_unsigned(value(), flag);
		else if (flag == "--print-formula") opt.print_formula = true;
		else if (flag == "--smoke") opt.g.smoke = true;
		else if (flag == "--help" || flag == "-h") {
			print_usage(std::cout);
			std::exit(0);
		}
		else usage_error("unknown option '" + flag + "'");
	}
	if (opt.lim.mem_mb == 0) usage_error("--mem must be at least 1");

	std::string implemented;
	for (const family_entry& f : families)
		if (f.instances) implemented += (implemented.empty() ? "" : ",")
			+ std::string(f.id);
	if (family_list.empty()) {
		for (const family_entry& f : families)
			if (f.instances) opt.selected.push_back(&f);
	} else {
		std::istringstream is(family_list);
		for (std::string id; std::getline(is, id, ',');) {
			const family_entry* found = nullptr;
			for (const family_entry& f : families)
				if (id == f.id) found = &f;
			if (!found) usage_error("unknown family '" + id + "'");
			if (!found->instances) usage_error("family " + id + " ("
				+ found->name + ") is not implemented yet; implemented: "
				+ implemented);
			opt.selected.push_back(found);
		}
	}
	for (const auto& [name, values] : opt.g.given) {
		if (name == "width" || name == "steps") continue;
		bool known = false;
		for (const family_entry* f : opt.selected)
			for (const knob& k : f->knobs) known |= k.name == name;
		if (!known) usage_error("no selected family has the knob '"
			+ name + "'");
	}
	return opt;
}

std::string csv_safe(std::string s) {
	for (char& c : s) if (c == ',' || c == '\n') c = ';';
	return s;
}

std::string word(const child_run& r) {
	switch (r.how) {
	case child_run::end::answered:      return r.answer.value;
	case child_run::end::timeout:       return "timeout";
	case child_run::end::out_of_memory: return "out-of-memory";
	case child_run::end::error:         break;
	}
	return "error(" + csv_safe(r.reason) + ")";
}

struct row {
	std::string oracle = "-";
	std::string result;
	std::string status;
	double ms;
	std::string binders;
};

row judge(const instance& in, const options& opt) {
	row r;
	if (in.width <= opt.check_max_width)
		r.oracle = word(run_in_child({ opt.check_timeout_s, opt.lim.mem_mb },
			[&] { return oracle_verdict(in.formula); }));
	const child_run engine = run_in_child(opt.lim,
		[&] { return engine_verdict(in.formula); });
	const bool answered = engine.how == child_run::end::answered;
	r.result = word(engine);
	r.ms = answered ? engine.answer.ms : engine.ms;
	if (answered) r.binders = std::to_string(engine.answer.binders);
	const std::string expected(1, in.expected);
	if ((r.oracle == "T" || r.oracle == "F") && r.oracle != expected)
		r.status = "GENERATOR?";
	else if (r.result == "T" || r.result == "F")
		r.status = r.result == expected ? "correct" : "WRONG";
	else switch (engine.how) {
	case child_run::end::answered:      r.status = "undecided"; break;
	case child_run::end::timeout:       r.status = "timeout"; break;
	case child_run::end::out_of_memory: r.status = "out-of-memory"; break;
	case child_run::end::error:         r.status = "error"; break;
	}
	return r;
}

const char* const statuses[] = { "correct", "WRONG", "undecided", "timeout",
	"out-of-memory", "GENERATOR?", "error" };

} // namespace

int main(int argc, char** argv) {
	const options opt = parse_options(argc, argv);
	std::cout << std::fixed << std::setprecision(1)
		<< "family,variant,params,width,steps,expected,oracle,result,"
		   "status,ms,binders_left\n";
	bool wrong = false;
	for (const family_entry* f : opt.selected) {
		std::vector<instance> instances;
		try { instances = f->instances(opt.g); }
		catch (const std::exception& e) { usage_error(e.what()); }
		std::map<std::string, size_t> tally;
		double total_ms = 0;
		for (const instance& in : instances) {
			if (opt.print_formula) std::cout << "# formula " << f->id
				<< ',' << in.variant << ',' << in.params << ','
				<< in.width << ',' << in.steps << ": " << in.formula
				<< '\n';
			const row r = judge(in, opt);
			std::cout << f->id << ',' << in.variant << ',' << in.params
				<< ',' << in.width << ',' << in.steps << ','
				<< in.expected << ',' << r.oracle << ',' << r.result
				<< ',' << r.status << ',' << r.ms << ',' << r.binders
				<< std::endl;
			++tally[r.status];
			total_ms += r.ms;
			wrong |= r.status == "WRONG";
		}
		std::cout << "# summary family " << f->id << " (" << f->name
			<< "): instances=" << instances.size();
		for (const char* s : statuses) std::cout << ' ' << s << '='
			<< tally[s];
		std::cout << " ms=" << total_ms << std::endl;
	}
	return wrong ? 1 : 0;
}
