// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unrolling a program into its closed formula; see program.h.

#include "program.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace bv_bench {

namespace {

// The parser's multi-letter variable names: no `T` or `F` up front, since
// those are the formula constants.
void check_name(const std::string& name) {
	const bool ok = !name.empty()
		&& std::isalpha(static_cast<unsigned char>(name[0]))
		&& name[0] != 'T' && name[0] != 'F'
		&& std::all_of(name.begin() + 1, name.end(), [](unsigned char c) {
			return std::isalnum(c) != 0; });
	if (!ok) throw std::invalid_argument(
		"not a stream or binder name: '" + name + "'");
}

} // namespace

instance_writer::instance_writer(const program& p, unsigned t,
	std::vector<std::vector<char>>& used,
	std::unordered_map<std::string, unsigned>& binders)
	: p(p), t(t), used(used), binders(binders) {}

std::string instance_writer::at(stream_id s, unsigned back) {
	const program::stream& st = p.streams.at(s);
	if (st.r == program::role::parameter) {
		if (back != 0) throw std::logic_error("parameter " + st.name
			+ " read with a step offset");
		used[s][0] = 1;
		return st.name + p.type;
	}
	if (back > t) throw std::logic_error(st.name + "[t-"
		+ std::to_string(back) + "] read at step "
		+ std::to_string(t) + ", before step 0");
	const unsigned u = t - back;
	used[s][u] = 1;
	deepest = std::max(deepest, back);
	return st.name + "_" + std::to_string(u) + p.type;
}

std::string instance_writer::fresh(const std::string& base) {
	check_name(base);
	for (const program::stream& st : p.streams)
		if (st.name == base) throw std::invalid_argument(
			"binder base '" + base + "' is a stream name");
	// A repeat in the same step gets a second `_`, which no first use has.
	std::string name = base + "_" + std::to_string(t);
	if (const unsigned n = ++binders[name]; n > 1)
		name += "_" + std::to_string(n);
	return name;
}

std::string instance_writer::typed(const std::string& binder) const {
	return binder + p.type;
}

std::string instance_writer::constant(unsigned long long value) const {
	return "{ " + std::to_string(value) + " }" + p.type;
}

std::string at_most_one(instance_writer& w, const std::string& h) {
	const std::string binder = w.fresh("x");
	const std::string x = w.typed(binder);
	const std::string zero = w.constant(0);
	return "(all " + binder + " ((" + x + " & " + h + "' = " + zero
		+ ") -> ((" + x + " = " + zero + ") || (" + x + " = " + h + "))))";
}

program::program(unsigned width) : type(":bv[" + std::to_string(width) + "]") {
	if (width == 0) throw std::invalid_argument("bv width must be at least 1");
}

stream_id program::add(const std::string& name, role r) {
	check_name(name);
	for (const stream& st : streams)
		if (st.name == name) throw std::invalid_argument(
			"stream '" + name + "' declared twice");
	streams.push_back({ name, r });
	return streams.size() - 1;
}

stream_id program::input(const std::string& name) {
	return add(name, role::input);
}

stream_id program::output(const std::string& name) {
	return add(name, role::output);
}

stream_id program::parameter(const std::string& name) {
	return add(name, role::parameter);
}

void program::always(unsigned lookback, rule_body body) {
	parts.push_back({ lookback, std::move(body), std::nullopt });
}

void program::at_step(unsigned step, rule_body body) {
	parts.push_back({ 0, std::move(body), step });
}

std::string program::formula(unsigned T) const {
	for (const part& r : parts)
		if (r.step && *r.step > T) throw std::invalid_argument(
			"a constraint at step " + std::to_string(*r.step)
			+ " lies beyond step " + std::to_string(T));
	std::vector<std::vector<char>> used(streams.size(),
		std::vector<char>(T + 1, 0));
	std::unordered_map<std::string, unsigned> binders;
	std::string matrix;
	for (unsigned t = 0; t <= T; ++t)
		for (const part& r : parts) {
			if (r.step ? *r.step != t : r.lookback > t) continue;
			instance_writer w(*this, t, used, binders);
			const std::string instance = r.body(w);
			if (!r.step && w.deepest != r.lookback) throw std::logic_error(
				"a part declared with lookback "
				+ std::to_string(r.lookback) + " reads back to t-"
				+ std::to_string(w.deepest));
			if (!matrix.empty()) matrix += " && ";
			matrix += '(';
			matrix += instance;
			matrix += ')';
		}
	if (matrix.empty()) throw std::invalid_argument(
		"no part has an instance up to step " + std::to_string(T));

	std::string prefix;
	auto block = [&](const char* quantifier, role r, unsigned u) {
		bool first = true;
		for (stream_id s = 0; s < streams.size(); ++s) {
			if (streams[s].r != r || !used[s][u]) continue;
			prefix += first ? quantifier : ",";
			prefix += ' ';
			prefix += streams[s].name;
			if (r != role::parameter) {
				prefix += '_';
				prefix += std::to_string(u);
			}
			first = false;
		}
		if (!first) prefix += ' ';
	};
	block("all", role::parameter, 0);
	for (unsigned u = 0; u <= T; ++u) {
		block("all", role::input, u);
		block("ex", role::output, u);
	}
	return prefix + "(" + matrix + ").";
}

} // namespace bv_bench
