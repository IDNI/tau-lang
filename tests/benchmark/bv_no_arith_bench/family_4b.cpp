// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Family 4b, hand-written templates: three small programs, each with a T and
// an F variant, run as `copies` independent copies side by side, each copy
// with its own streams. Each line is one `always` part, from its own lookback;
// a <= b is the Boolean-algebra order, written a & b' = 0.
//   access       inputs req, revoke; outputs allowed, grant; the policy
//                P = {1}' leaves out element 0 at every width (P = 0 at n = 1):
//                  allowed[t] = allowed[t-1] & revoke[t]' & P
//                  grant[t] = req[t] & allowed[t]
//                  F only: req[t] <= grant[t]
//   accumulator  inputs add, reset; output acc:
//                  reset[t] != 0 ? acc[t] = add[t] : acc[t] = acc[t-1] | add[t]
//                  F only: acc[t-1] <= acc[t]
//   queue        input arrive; outputs pending, served; lookback d:
//                  pending[t] = (pending[t-1] | arrive[t]) & served[t]'
//                  served[t] <= pending[t-1] | arrive[t]
//                  atmost1(served[t])
//                  arrive[t-d] <= served[t-d] | ... | served[t]
//                T iff n <= d + 1, so the width decides the variant.
// The copies share no stream and the prefix restricted to one copy is that
// copy's own prefix, so a strategy for one copy carries over: the system plays
// one in every copy, the environment defeats a single copy. k copies have the
// verdict of one. A template's floor is the least horizon at which every part
// has an instance and its F argument holds; a given horizon below is skipped.

#include "families.h"
#include "program.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace bv_bench {

namespace {

enum class tmpl { access, accumulator, queue };
const char* const template_names[] = { "access", "accumulator", "queue" };

// T: allowed[0] is free and every later output is a function of values bound
// before it. F: from step 1 on req <= grant <= allowed <= P, and the
// environment requests P' != 0 at step 1. At T = 0 the system wins with
// allowed[0] = 1, so the floor is 1.
std::string access(unsigned n, unsigned copies, unsigned T, bool f) {
	program p(n);
	for (unsigned c = 1; c <= copies; ++c) {
		const std::string id = std::to_string(c);
		const stream_id req = p.input("req" + id);
		const stream_id revoke = p.input("revoke" + id);
		const stream_id allowed = p.output("allowed" + id);
		const stream_id grant = p.output("grant" + id);
		p.always(1, [=](instance_writer& w) {
			return "(" + w.at(allowed, 0) + " = (" + w.at(allowed, 1) + " & "
				+ w.at(revoke, 0) + "' & " + w.constant(1) + "'))";
		});
		p.always(0, [=](instance_writer& w) {
			return "(" + w.at(grant, 0) + " = (" + w.at(req, 0) + " & "
				+ w.at(allowed, 0) + "))";
		});
		if (f) p.always(0, [=](instance_writer& w) {
			return included(w.at(req, 0), w.at(grant, 0), w); });
	}
	return p.formula(T);
}

// T: acc[0] is free and acc[t] is a function of values bound before it. F:
// the environment adds 1 at step 1, so acc[1] = 1, then resets with add = 0
// at step 2, so acc[2] = 0. At T = 1 the system wins with acc[0] = 0, so the
// floor is 2.
std::string accumulator(unsigned n, unsigned copies, unsigned T, bool f) {
	program p(n);
	for (unsigned c = 1; c <= copies; ++c) {
		const std::string id = std::to_string(c);
		const stream_id add = p.input("add" + id);
		const stream_id reset = p.input("reset" + id);
		const stream_id acc = p.output("acc" + id);
		p.always(1, [=](instance_writer& w) {
			const std::string now = w.at(acc, 0);
			const std::string in = w.at(add, 0);
			return "((" + w.at(reset, 0) + " != " + w.constant(0) + ") ? ("
				+ now + " = " + in + ") : (" + now + " = (" + w.at(acc, 1)
				+ " | " + in + ")))";
		});
		if (f) p.always(1, [=](instance_writer& w) {
			return included(w.at(acc, 1), w.at(acc, 0), w); });
	}
	return p.formula(T);
}

// T (n <= d + 1): at step t serve element t mod n if it is pending or
// arriving. An element stays in pending[t-1] | arrive[t] from its arrival
// until served (the free pending[0] takes arrive[0] & served[0]'), and any
// d + 1 >= n steps in a row give it a turn. F (n > d + 1): the environment
// sends 1 at step 0; the window at t = d asks for its n elements from the
// steps 0..d, which serve at most d + 1. The floor is max(1, d).
std::string queue(unsigned n, unsigned d, unsigned copies, unsigned T) {
	program p(n);
	for (unsigned c = 1; c <= copies; ++c) {
		const std::string id = std::to_string(c);
		const stream_id arrive = p.input("arrive" + id);
		const stream_id pending = p.output("pending" + id);
		const stream_id served = p.output("served" + id);
		p.always(1, [=](instance_writer& w) {
			return "(" + w.at(pending, 0) + " = ((" + w.at(pending, 1)
				+ " | " + w.at(arrive, 0) + ") & " + w.at(served, 0)
				+ "'))";
		});
		p.always(1, [=](instance_writer& w) {
			return included(w.at(served, 0), w.at(pending, 1) + " | "
				+ w.at(arrive, 0), w);
		});
		p.always(0, [=](instance_writer& w) {
			return at_most_one(w, w.at(served, 0)); });
		p.always(d, [=](instance_writer& w) {
			return included(w.at(arrive, d), window(w, served, d), w); });
	}
	return p.formula(T);
}

} // namespace

std::vector<instance> family_4b(const grid& g) {
	const std::vector<unsigned> defaults{ 1, 2, 4, 8, 16, 32, 64 };
	const auto templates = g.values("template", { 0, 1, 2 }, { 0, 1, 2 });
	const auto variants = g.values("variant", { 0, 1 }, { 0, 1 });
	const auto copies = g.values("copies", { 1, 2, 4 }, { 1, 2 });
	for (unsigned ti : templates) if (ti > 2) throw std::invalid_argument(
		"family 4b's template is 0 (access), 1 (accumulator) or 2 (queue)");
	for (unsigned v : variants) if (v > 1) throw std::invalid_argument(
		"family 4b's variant is 0 (T) or 1 (F)");
	if (std::ranges::count(copies, 0u))
		throw std::invalid_argument("family 4b needs copies >= 1");

	std::vector<instance> out;
	size_t skipped = 0;
	for (unsigned ti : templates) {
		const tmpl which = static_cast<tmpl>(ti);
		const bool is_queue = which == tmpl::queue;
		for (unsigned d : is_queue ? g.values("d", { 0, 1, 2, 3 }, { 1 })
			: std::vector<unsigned>{ 0 })
		{
			const unsigned floor = which == tmpl::access ? 1
				: which == tmpl::accumulator ? 2 : std::max(1u, d);
			const auto widths = is_queue
				? g.values("width", around(defaults, d + 1),
					around({ 64 }, d + 1))
				: g.values("width", defaults, { 1, 2, 64 });
			for (unsigned T : g.values("steps", { floor, floor + 4 },
				{ floor }))
			{
				if (T < floor) { ++skipped; continue; }
				for (unsigned k : copies)
				for (unsigned v : variants)
				for (unsigned n : widths) {
					const char expected = is_queue
						? (n <= d + 1 ? 'T' : 'F') : "TF"[v];
					if (expected != "TF"[v]) continue;
					std::string params = "copies=" + std::to_string(k);
					if (is_queue) params = "d=" + std::to_string(d) + ";"
						+ params;
					out.push_back({ std::string(template_names[ti]) + "/"
						+ expected, params, n, T, expected,
						which == tmpl::access ? access(n, k, T, v == 1)
						: which == tmpl::accumulator
							? accumulator(n, k, T, v == 1)
							: queue(n, d, k, T) });
				}
			}
		}
	}
	if (skipped) std::cerr << "# family 4b: skipped " << skipped
		<< " (template, T) points with T below the template's floor\n";
	return out;
}

} // namespace bv_bench
