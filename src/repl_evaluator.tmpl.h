// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "repl_evaluator.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "repl_evaluator"

namespace idni::tau_lang {

namespace repl_detail {

/**
 * @brief Is @p src a truncated literal of @p type_tree, per its owning BA?
 *
 * nullopt when no BA owns the type or none offers the capability, leaving the
 * caller to fall back on tau's own parser. A definite `false` means the owner
 * looked and found the literal complete, which is not the same answer.
 */
template <typename node_t>
std::optional<bool> try_literal_incomplete(tref type_tree,
	const std::string& src)
{
	if (!type_tree) return {};
	std::optional<bool> out;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		using pack = typename node_t::bas_tuple;
		([&] {
			using BA = std::tuple_element_t<Is, pack>;
			if constexpr (ba_has_descriptor_v<node_t, BA>
				&& requires(const std::string& s) {
					{ ba_descriptor<BA, node_t>::literal_incomplete(s) }
						-> std::convertible_to<bool>; })
				if (!out && ba_descriptor<BA, node_t>::owns_type(type_tree))
					out = ba_descriptor<BA, node_t>::literal_incomplete(src);
		}(), ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename node_t::bas_tuple>>{});
	return out;
}

} // namespace repl_detail

#define TC_STATUS        TC.BG_LIGHT_CYAN()
#define TC_STATUS_OUTPUT TC(term::color::GREEN, term::color::BG_LIGHT_CYAN, \
							term::color::BRIGHT)
#define TC_ERROR         TC(term::color::RED,   term::color::BRIGHT)
#define TC_PROMPT        TC(term::color::WHITE, term::color::BRIGHT)
#define TC_OUTPUT        TC.GREEN() // TODO: change to term::color::GREEN()

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::invalid_argument() const {
	TAU_LOG_ERROR << "Invalid argument\n";
	return nullptr;
}

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<size_t> repl_evaluator<BAs...>::get_history_index(
	const tt& n, const size_t size, bool silent) const
{
	if (size == 0) {
		if (!silent) std::cout << "history is empty\n";
		return {};
	}
	auto mem_type = n | tt::only_child | tt::nt;
	auto is_relative = (mem_type == tau::rel_history);
	auto mem_id = n | mem_type | tau::history_id;
	size_t idx = 0;
	if (mem_id) idx = mem_id | tt::num;
	// TAU_LOG_TRACE << "get_history_index idx: " << idx
	// 	<< "       relative? " << is_relative << "    "
	// 	<< TAU_LOG_FM(n.value());
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
	// TAU_LOG_TRACE << "get_history_index result: "
	// 	<< (is_relative ? size - idx - 1 : idx - 1);
	return { is_relative ? size - idx - 1 : idx - 1 };
}

template <typename... BAs>
requires BAsPack<BAs...>
repl_evaluator<BAs...>::history_ref repl_evaluator<BAs...>::history_retrieve(
	const tt& n, bool silent) const
{
	if (auto pos = get_history_index(n, H.size(), silent); pos.has_value())
		return { { H[pos.value()], pos.value() } };
	TAU_LOG_ERROR << "History location does not exist\n";
	return {};
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::print_history(const htref& mem, const size_t id,
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
	tau_api::print(std::cout, mem) << "\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::history_print_cmd(const tt& command) {
	auto n = command | tau::history;
	if (!n) return;
	auto idx = get_history_index(n.value(), H.size());
	if (idx) print_history(H[idx.value()], idx.value(), H.size());
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::history_list_cmd() {
	if (H.size() == 0) std::cout << "history is empty\n";
	else for (size_t i = 0; i < H.size(); i++)
		print_history(H[i], i, H.size());
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::history_store(tref o) {
	auto v = tau::geth(o);
	// do not add into history if the last history value is the same
	if (!(H.size() && H.back() == v))
		H.push_back(v);
	if (opt.print_history_store)
		print_history(H.back(), H.size() - 1, H.size(), false);
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::history_store_cmd(const tt& command) {
	history_store(command | tt::first | tt::ref);
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::get_(typename node::type nt, tref n,
	bool suppress_error) const
{
	// TAU_LOG_TRACE << "get_/n: " << node::name(nt) << "        " << TAU_DUMP_TO_STR(n);
	if (tau::get(n).is(nt)) return n;
	else if (tau::get(n).is(tau::history)) {
		if (auto check = history_retrieve(n); check) {
			const auto& h = check.value().first;
			if (tau::get(h).is(nt)) return h->get();
			else if (!suppress_error)
				TAU_LOG_ERROR << "Argument has a wrong type";
			return nullptr;
		}
	}
	if (!suppress_error) TAU_LOG_ERROR << "Argument has a wrong type";
	return nullptr;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::get_bf(tref n, bool suppress_error) const {
	return get_(tau::bf, n, suppress_error);
}
template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::get_wff(tref n) const {
	return get_(tau::wff, n, false);
}

// Puts an expression into the type-annotated form that substitution matching
// compares against.
//
// Matching is sensitive to the resolved BA type id on each node. A history
// entry produced by dnf/cnf/normalize has already been through inference and
// carries those ids, whereas an expression parsed straight off the command line
// carries none, so the two never match even when they print identically. Both
// sides therefore have to be inferred before they are compared.
//
// Inference is idempotent on an already inferred tree, so this is safe to apply
// to every argument. An expression inference rejects is returned unchanged
// rather than turned into an error, which keeps this from failing substitutions
// that used to work.
template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::infer_for_match(tref n) const {
	if (!n) return n;
	tref inferred = tau_api::infer(n);
	return inferred ? inferred : n;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::get_any(tref arg) const {
	if (auto check = get_type_and_arg(arg); check)
		return check.value().second;
	return nullptr;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::get_applied(tref arg) const {
	// create a spec from the arg and add io and rr defs
	tau_spec<node> spec;
	spec.add(arg);
	for (const htref& d : rr_defs) spec.add(d->get());
	for (const htref& d : io_defs) spec.add(d->get());
	auto maybe_nso_rr = spec.get_nso_rr();
	if (!maybe_nso_rr) {
		DBG(TAU_LOG_TRACE << "nso_rr has no value";)
		for (const auto& err : spec.errors()) {
			TAU_LOG_ERROR << err;
		}
		return nullptr;
	}
	tref main = maybe_nso_rr.value().main->get();
	if (!main) {
		DBG(TAU_LOG_TRACE << "main is nullptr";)
		return nullptr;
	}
	// add defs to global definitions:
	auto& defs = definitions<node>::instance();
	for (rewriter::rule& r : maybe_nso_rr.value().rec_relations) {
		defs.add(r.first, r.second);
		DBG(TAU_LOG_TRACE << "added def to globals: " << TAU_LOG_RULE(r);)
	}
	tref applied = nso_rr_apply(maybe_nso_rr.value());
	// tref applied = tau_api::apply_defs(main);
	DBG(TAU_LOG_TRACE << "applied: " << TAU_LOG_FM_DUMP(applied);)
	return applied;
}

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<std::pair<size_t, tref>>
	repl_evaluator<BAs...>::get_type_and_arg(const tt& n) const
{
	auto nt = n | tt::nt;
	tref r = nullptr;
	DBG(TAU_LOG_TRACE << "get_type_and_arg: " << TAU_LOG_NT(nt);)
	DBG(TAU_LOG_TRACE << "arg: " << TAU_LOG_FM_DUMP(n | tt::ref);)
	switch (nt) {
		case tau::history:
			if (auto check = history_retrieve(n); check) {
				auto [value, _] = check.value();
				nt = tt(value) | tt::nt;
				r = value->get();
				break;
			} else return {};
		default: r = n | tt::ref;
	}
	r = get_applied(r);
	if (!r) return {};
	return { { tau::get(r).get_type(), r } };
}

template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& repl_evaluator<BAs...>::benchmarks(measuring& m) const {
	if (opt.print_benchmarks) return m(std::cerr);
	return std::cerr;
}

template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& repl_evaluator<BAs...>::benchmarks(measuring& m,
	idni::measures::timer& t) const
{
	m.ms = t.stop();
	if (opt.print_benchmarks) return m(std::cerr);
	return std::cerr;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::onf_cmd(const tt& n) {
	// grammar: "onf" __ variable __ onf_cmd_arg -- n[1] is the variable
	// itself (must not go through get_any/apply_all_defs, which expect a
	// formula/history argument), n[2] is the formula.
	tref var = n[1].get();
	tref arg = n[2].get();
	measuring m("onf");
	idni::measures::timer t;
	t.start();
	tref r = nullptr;
	if (auto value = get_any(arg); value)
		if (tref applied = tau_api::apply_all_defs(m.part(), value); applied)
			r = onf<node>(applied, var);
	return benchmarks(m, t), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::dnf_cmd(const tt& n) {
	measuring m;
	tref r = nullptr;
	if (auto value = get_any(n[1].get()); value)
		r = tau_api::dnf(m, value);
	return benchmarks(m), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::cnf_cmd(const tt& n) {
	measuring m;
	tref r = nullptr;
	if (auto value = get_any(n[1].get()); value)
		r = tau_api::cnf(m, value);
	return benchmarks(m), r;
}


template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::nnf_cmd(const tt& n) {
	measuring m;
	tref r = nullptr;
	if (auto value = get_any(n[1].get()); value)
		r = tau_api::nnf(m, value);
	return benchmarks(m), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::anf_cmd(const tt& n) {
	measuring m;
	tref r = nullptr;
	if (auto value = get_any(n[1].get()); value) {
		constexpr size_t bf_type = 0;
		r = anf<node, bf_type>(value);
	}
	return benchmarks(m), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::pnf_cmd(const tt& n) {
	measuring m;
	tref r = nullptr;
	if (auto value = get_any(n[1].get()); value)
		r = pnf<node>(value);
	return benchmarks(m), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::mnf_cmd(const tt& n) {
	measuring m("mnf");
	idni::measures::timer t;
	t.start();
 	tref r = nullptr;
	tref arg = n[1].get();
	auto wff_mnf = [](tref applied) {
		return unequal_to_not_equal<node>(
			reduce<node>(to_dnf<node>(
				bf_reduce_canonical<node>()(applied))));
	};
	if (auto value = get_any(arg); value)
		if (tref applied = tau_api::apply_all_defs(m.part(), value); applied)
			switch (tau::get(applied).get_type()) {
			case tau::wff: r = wff_mnf(applied); break;
			case tau::bf:  r = bf_reduced_dnf<node>(applied); break;
			default: return invalid_argument();
			}
	return benchmarks(m, t), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::subst_cmd(const tt& n) {
	// DBG(TAU_LOG_TRACE << "subst_cmd" << LOG_FM_DUMP(n.value());)
	tref arg1 = n | tt::second | tt::ref;
	tref arg2 = n | tt::third  | tt::ref;
	tref arg3 = n | tt::fourth | tt::ref;
	// TAU_LOG_TRACE << "subst_cmd arg1: " << TAU_DUMP_TO_STR(arg1);
	// TAU_LOG_TRACE << "subst_cmd arg2: " << TAU_DUMP_TO_STR(arg2);
	// TAU_LOG_TRACE << "subst_cmd arg3: " << TAU_DUMP_TO_STR(arg3);

	measuring m;
	// Since the history command cannot be type-checked we do it here
	// First try to get bf
	tref in = get_bf(arg1, true);
	if (in) { // BF substitution
		tref thiz = get_bf(arg2), with = get_bf(arg3);
		if (!in || !thiz || !with) return invalid_argument();
		in = infer_for_match(in), thiz = infer_for_match(thiz),
			with = infer_for_match(with);
		// strip bf of variables so we match also quantifiers
		if (is<node, tau::bf>(thiz) && is_child<node, tau::variable>(thiz))
			thiz = tau::trim(thiz),	with = tau::trim(with);
		// DBG(TAU_LOG_TRACE << "bf in:   " << TAU_LOG_FM_DUMP(in);)
		// DBG(TAU_LOG_TRACE << "thiz:    " << TAU_LOG_FM_DUMP(thiz);)
		// DBG(TAU_LOG_TRACE << "with:    " << TAU_LOG_FM_DUMP(with);)
		tref r = tau_api::substitute(m, in, thiz, with);
		return benchmarks(m), r;
	}
	// First argument was not a bf so it must be a wff
	in = get_wff(arg1);
	// Now sort out the remaining argument types
	tref with, thiz = get_bf(arg2, true);
	if (thiz) with = get_bf(arg3);
	else thiz = get_wff(arg2), with = get_wff(arg3);
	// Check for correct argument types
	if (!thiz || !in || !with) {
		TAU_LOG_ERROR << "Invalid argument\n";
		return nullptr;
	}
	in = infer_for_match(in), thiz = infer_for_match(thiz),
		with = infer_for_match(with);
	// strip bf of variables so we match also quantifiers
	if (is<node, tau::bf>(thiz) && is_child<node, tau::variable>(thiz))
		thiz = tau::trim(thiz),	with = tau::trim(with);
	// DBG(TAU_LOG_TRACE << "wff in: " << TAU_LOG_FM_DUMP(in);)
	// DBG(TAU_LOG_TRACE << "thiz:   " << TAU_LOG_FM_DUMP(thiz);)
	// DBG(TAU_LOG_TRACE << "with:   " << TAU_LOG_FM_DUMP(with);)
	tref r = tau_api::substitute(m, in, thiz, with);
	return benchmarks(m), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::inst_cmd(const tt& n) {
	// DBG(TAU_LOG_TRACE << "inst_cmd" << LOG_FM_DUMP(n.value());)
	const auto& t = n.value_tree();
	if (!t[2][0].is(tau::variable)) {
		TAU_LOG_ERROR << "Invalid argument\n";
		return nullptr;
	}
	tref nn = tau::get(t.value, { t.first(), t.second(),
			t.third(), t.child(3) });
	return subst_cmd(nn);
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::normalize_cmd(const tt& n) {
	tref arg = n[1].get();
	auto check = get_type_and_arg(arg);
	if (!check) return nullptr;
	auto [type, value] = check.value();
	if (reject_ctl_star_if_disabled(value)) return nullptr;
	measuring m;
	tref r;
	switch (type) {
		case tau::wff: r = tau_api::normalize_formula(m, value); break;
		case tau::bf:  r = tau_api::normalize_term(m, value); break;
		default: return nullptr;
	}
	return benchmarks(m), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::qelim_cmd(const tt& n) {
	measuring m;
	tref r = nullptr;
	if (auto value = get_any(n[1].get());
		value && !reject_ctl_star_if_disabled(value))
		r = tau_api::eliminate_quantifiers(m, value);
	return benchmarks(m), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::reset_cmd() {
	H.clear();
	rr_defs.clear();
	io_defs.clear();
	definitions<node>::instance().clear();
	std::cout << "Session reset: history, definitions, and IO streams cleared.\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::whatis_cmd(const tt& n) {
	tref arg = n[1].get();
	auto check = get_type_and_arg(arg);
	if (!check) return nullptr;
	auto [type, value] = check.value();
	const std::string& node_type = tau::get(value).get_type_name();
	std::cout << "node type: " << node_type;
	if (type == tau::bf || type == tau::wff) {
		size_t ba_type = tau::get(value).get_ba_type();
		if (ba_type) std::cout << "  BA type: " << ba_types<node>::name(ba_type);
	}
	std::cout << "\n";
	return value;
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::run_cmd(const tt& n) {

	DBG(TAU_LOG_TRACE << "run_cmd: " << TAU_LOG_FM(n.value());)
	measuring m("run");

	// run [N steps] [<fm>]: new or continued session, bounded or natural.
	auto num_t = n | tau::num;
	const bool bounded = (bool) num_t;
	const size_t steps = bounded ? (size_t)(num_t | tt::num) : 0;

	// Formula is the 3rd child when a count was given, else the 2nd.
	tref value = nullptr;
	if (auto fc = bounded ? (n | tt::third) : (n | tt::second))
		value = get_any(fc | tt::ref);

	if (value) {
		if (reject_ctl_star_if_disabled(value)) return;
		DBG(TAU_LOG_TRACE << "run_cmd/value: " << TAU_LOG_FM(value);)
		// Build a tau_spec from the formula and REPL-defined io/rr defs,
		// mirroring get_applied(). This uses the tau_spec path which correctly
		// handles io_var resolution for LTL and safety formulas.
		tau_spec<node> spec;
		spec.add(value);
		// IN-N5: definition bodies are conjoined into the spec; gate them
		// like the main formula, or A/E smuggled through a def bypass the
		// fragment check.
		for (const htref& d : rr_defs) {
			if (reject_ctl_star_if_disabled(d->get())) return;
			spec.add(d->get());
		}
		for (const htref& d : io_defs) {
			if (reject_ctl_star_if_disabled(d->get())) return;
			spec.add(d->get());
		}

		auto maybe_i = tau_api::get_interpreter(m.part(), spec);
		if (!maybe_i) return;

		// A new formula replaces any stored session.
		running = std::make_unique<run_session>(std::move(maybe_i.value()));
		running->steps_done   = 0;
		running->steps_to_run = steps; // 0 = natural
		running->t.start();
		continue_running();
		return;
	}

	// No formula: continue the stored session (error if none).
	if (!running) {
		TAU_LOG_ERROR << "no run to continue; start one with "
			"`run <formula>` (optionally `run N steps <formula>`)";
		return;
	}
		// `run N steps` runs N more steps; bare `run` continues naturally.
	running->steps_to_run = bounded ? running->steps_done + steps : 0;
	running->t.start();
	continue_running();
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::stop_cmd() {
	if (!running) { std::cout << "no run in progress\n"; return; }
	finish_running();
	std::cout << "run stopped\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::ltl_cmd(const tt& n) {
	DBG(TAU_LOG_TRACE << "ltl_cmd: " << TAU_LOG_FM(n.value());)
	measuring m("ltl");
	idni::measures::timer t;

	tref value = get_any(n[1].get());
	if (!value) return;
	// IN-N5: `ltl` was the one formula command with no fragment gate.
	if (reject_ctl_star_if_disabled(value)) return;

	t.start();
	DBG(TAU_LOG_TRACE << "ltl_cmd/value: " << TAU_LOG_FM(value);)

	// IN-R4: the synthesis backend reports "no verdict" by throwing;
	// nothing above this frame catches it, so a slow or missing ltlsynt
	// (or a refused CTL* placement) used to terminate the REPL.
	try {
		ltl_explain<node>(value, std::cout);
	} catch (const ltl_synthesis_error& e) {
		TAU_LOG_ERROR << "UNKNOWN: the synthesis backend failed, timed "
			"out or refused the formula (" << e.what()
			<< "); realizability could not be decided";
		error = true;
	}
	benchmarks(m, t);
}

// Drives a `run` session's step loop, suspending via `pending` (instead of
// blocking) when it needs input; the next eval() call resumes it.
template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::continue_running(
	std::optional<pending_request> retry)
{
	bool first = true;
	while (running) {
		// At budget: stop cleanly but KEEP the session for a later `run`.
		if (running->steps_to_run != 0
			&& running->steps_done >= running->steps_to_run) {
			running->t.pause();
			return;
		}
		auto& step_m = running->m.part();
		// time_point advances iff a step produced output; api::step returns
		// nullopt both when it needs input and when auto_continue is false.
		const size_t tp_before = running->interp.time_point;
		// IN-R4: a mid-run pointwise-revision update can reach ltlsynt
		// (through update() -> pointwise_revision), whose failures are
		// thrown; end the session with a diagnostic instead of the
		// process.
		decltype(tau_api::step(step_m, running->interp)) maybe_outputs;
		try {
			maybe_outputs = tau_api::step(step_m, running->interp);
		} catch (const ltl_synthesis_error& e) {
			TAU_LOG_ERROR << "UNKNOWN: the synthesis backend failed, "
				"timed out or refused the formula during this step ("
				<< e.what() << "); ending the run";
			running->m.parts.pop_back();
			running.reset();
			error = true;
			return;
		}
		// copy this step's timing before dropping the node (step_m is a
		// reference into m.parts that pop_back() invalidates)
		measuring step_m_copy = step_m;
		running->m.parts.pop_back();

		if (!maybe_outputs) {
			const bool produced =
				running->interp.time_point != tp_before;
			// Input-independent step: output already written.
			// "continue?" prompt, mirroring C++ run(fm, ctx, N).
			if (produced && running->steps_to_run != 0) {
				++running->steps_done;
				if (opt.print_benchmarks) step_m_copy(std::cout, 1);
				std::cout << "\n";
				first = false;
				continue;
			}
			running->t.pause();
			// a console input stream stopped the step needing a value:
			// find it and prompt for that value (label/type are ours)
			for (auto& [var, stream] : running->interp.inputs) {
				auto rp = std::dynamic_pointer_cast<
					repl_pending_input_stream>(stream);
				if (!rp || !rp->awaiting()) continue;
				size_t tp = rp->awaiting_time_point();
				size_t tid = running->interp.ctx.type_of(var);
				std::string type_name = get_ba_type_name<node>(tid);
				if (!type_name.empty() && type_name.front() == ':')
					type_name.erase(0, 1);
				std::stringstream lbl;
				lbl << get_var_name<node>(var) << "[" << tp << "] : "
					<< type_name << " := ";
				pending = { pending_request::stream_value, lbl.str(),
					rp, tp, get_ba_type_tree<node>(tid) };
				reprompt();
				return; // suspend: wait for the answer
			}
		// Ended before budget (e.g. input EOF): stop, do not prompt.
			if (running->steps_to_run != 0) return;
			// no awaiting stream -> rejected value (re-ask) or end of run
			if (first && retry) pending = *retry;
			else pending = { pending_request::continue_or_quit,
				"continue? [Enter]/[q]: ", nullptr };
			reprompt();
			return; // suspend: wait for the answer
		}
		// this step produced output: print its timing right after (to
		// cout, so it stays in order and belongs to the step it measures,
		// before the next "Execution step"), then a blank line
		++running->steps_done;
		if (opt.print_benchmarks) step_m_copy(std::cout, 1);
		std::cout << "\n";
		first = false;
	}
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::finish_running() {
	if (running) benchmarks(running->m, running->t);
	running.reset();
	pending.reset();
}

template <typename... BAs>
requires BAsPack<BAs...>
bool repl_evaluator<BAs...>::stream_value_incomplete(
	const std::string& src, tref type_tree) const
{
	// The BA owning the type answers for its own literals.
	if (auto r = repl_detail::try_literal_incomplete<node>(type_tree, src))
		return *r;
	// Only a tau value is a spec; tau_spec::parse() returns true on
	// EOF-incomplete input and flags is_eof().
	if (type_tree && is_tau_type<node>(type_tree)) {
		tau_spec<node> s;
		s.parse(src);
		return s.is_eof();
	}
	return false;
}

#ifdef TAU_PARSER_HAS_FTXUI
template <typename... BAs>
requires BAsPack<BAs...>
repl_key_action repl_evaluator<BAs...>::on_repl_key(const std::string& key) {
	// only claim keys while a `run` session is awaiting input
	if (!pending) return {};
	if (pending->kind == pending_request::continue_or_quit) {
		// single-key gate: Enter continues, q (or Ctrl-C) quits
		if (key == "enter") return { repl_key_action::submit, "" };
		if (key == "q" || key == "ctrl-c")
			return { repl_key_action::submit, "q" };
		return { repl_key_action::consume, {} }; // ignore other keys
	}
	// stream value: type normally, but Ctrl-C aborts the whole run
	if (key == "ctrl-c") {
		run_abort_ = true;
		return { repl_key_action::submit, "" };
	}
	return {};
}
#endif

template <NodeType node>
solver_mode get_solver_cmd_mode(tref n) {
	using tau = tree<node>;
	if (tref solver_mode = tau::get(n).find_top(is<node, tau::solver_mode>);
		solver_mode)
	{
		typename node::type mode = tau::get(solver_mode)[0].get_type();
		return (mode == tau::solver_mode_minimum) ? solver_mode::minimum
							: solver_mode::maximum;
	} else return solver_mode::general;
}

template <NodeType node>
size_t get_solver_cmd_type(tref n) {
	size_t type = find_ba_type<node>(n);
	return type > 0 ? type
		: get_ba_type_id<node>(
			node::ba::default_type());
}

template <NodeType node>
void print_solver_cmd_solution(std::optional<solution<node>>& solution,
		size_t type_id)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	// bf_t/bf_f carry no BA type; serialize_constant renders them as the
	// type's one/zero (see the ba_constant case in tau_tree_printers). This
	// also covers the general ba_constant case, unlike a narrower dedicated
	// bf_t/bf_f branch; type_id is the fallback for untyped variables.
	if (!solution) { std::cout << "no solution\n"; return; }

	std::cout << "solution: {\n";
	for (auto [var, value]: solution.value()) {
		if (tt(value) | tau::ba_constant) {
			print_binding<node>(std::cout, var, value);
			continue;
		}
		size_t t = find_ba_type<node>(var);
		if (t == 0) t = type_id;
		std::stringstream ss;
		if (!serialize_constant<node>(ss, value, t))
			print_binding<node>(std::cout, var, value);
		else std::cout << "\t" << tau::get(var).to_str() << " := { "
			<< ss.str() << " }" << ba_types<node>::name(t) << "\n";
	}
	std::cout << "}\n";
}

// Prints the interpreter memory plus a binding count. Uses the type-agnostic
// form because memory entries span BA types and not all have a resolvable one.
template <NodeType node>
void print_memory(const assignment<node>& memory) {
	using tau = tree<node>;
	using tt = typename tau::traverser;
	if (memory.empty()) {
		std::cout << "memory: {}\n0 bindings\n";
		return;
	}
	// subtree_map order is pointer identity, not stable across runs.
	// Sort io_var keys by constant_io_comp (matching interpreter::write);
	// other keys by printed form -- constant_io_comp assumes io_var shape.
	trefs io_keys, other_keys;
	io_keys.reserve(memory.size());
	for (auto [var, value] : memory)
		((tt(var) | tau::variable | tau::io_var) ? io_keys : other_keys)
			.push_back(var);
	std::sort(io_keys.begin(), io_keys.end(), constant_io_comp<node>);
	std::sort(other_keys.begin(), other_keys.end(), [](tref a, tref b) {
		return tau::get(a).to_str() < tau::get(b).to_str();
	});
	std::cout << "memory: {\n";
	for (tref var : io_keys)    print_binding<node>(std::cout, var, memory.at(var));
	for (tref var : other_keys) print_binding<node>(std::cout, var, memory.at(var));
	std::cout << "}\n" << memory.size() << " binding"
		<< (memory.size() == 1 ? "" : "s") << "\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::memory_cmd() {
	if (!running) {
		std::cout << "no run in progress; memory is only tracked "
			"during an active `run` session\n";
		return;
	}
	print_memory<node>(running->interp.memory);
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::solve_cmd(const tt& n) {
	// setting solver options
	tref arg = n.value_tree().first();
	while (tau::get(arg).has_right_sibling())
		arg = tau::get(arg).right_sibling();
	tref value = get_any(arg);
	if (!value) return;
	measuring m;
	auto solution = tau_api::solve(m, value,
		get_solver_cmd_mode<node>(n.value()));
	benchmarks(m);
	if (!solution) { std::cout << "no solution\n"; return; }

	// the printer needs the BA type of the solution, not the grammar
	// nonterminal of the argument that get_type_and_arg also returns
	print_solver_cmd_solution<node>(solution,
		get_solver_cmd_type<node>(value));
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::lgrs_cmd(const tt& n) {
	tref arg = n.value_tree().first();
	while (tau::get(arg).has_right_sibling())
		arg = tau::get(arg).right_sibling();
	tref value = get_any(arg);
	if (!value) return;
	measuring m;
	auto solution = tau_api::lgrs(m, value);
	benchmarks(m);
	if (!solution) { std::cout << "no solution\n"; return; }
	// trefs vars = tau::get(equations).select_top(is_child<node, tau::variable>);
	// same as solve_cmd: the printer takes a BA type id, not the grammar
	// nonterminal that get_type_and_arg also returns
	print_solver_cmd_solution<node>(solution,
		get_solver_cmd_type<node>(value));
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::valid_cmd(const tt& n) {
	measuring m;
	tref r = nullptr;
	if (tref value = get_any(n[1].get());
		value && !reject_ctl_star_if_disabled(value))
		r = tau_api::valid(m, value) ? tau::_T() : tau::_F();
	return benchmarks(m), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::sat_cmd(const tt& n) {
	measuring m;
	tref r = nullptr;
	if (tref value = get_any(n[1].get());
		value && !reject_ctl_star_if_disabled(value))
		r = tau_api::sat(m, value) ? tau::_T() : tau::_F();
	return benchmarks(m), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::unsat_cmd(const tt& n) {
	measuring m;
	tref r = nullptr;
	if (tref value = get_any(n[1].get());
		value && !reject_ctl_star_if_disabled(value))
		r = tau_api::unsat(m, value) ? tau::_T() : tau::_F();
	return benchmarks(m), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::realizable_cmd(const tt& n) {
	measuring m;
	tref r = nullptr;
	if (tref value = get_any(n[1].get()); value)
		r = tau_api::realizable(m, value) ? tau::_T() : tau::_F();
	return benchmarks(m), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::unrealizable_cmd(const tt& n) {
	measuring m;
	tref r = nullptr;
	if (tref value = get_any(n[1].get()); value)
		r = tau_api::unrealizable(m, value) ? tau::_T() : tau::_F();
	return benchmarks(m), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::def_rr_cmd(const tt& n) {
	// grammar: rec_relation => ref ":=" (capture | ref | wff | bf)
	tref def = n | tt::first | tt::ref;
	const auto& t = tau::get(def);
	// Reject a definition that can never be used before it is stored, so
	// that using it hangs the unfolding (issue 20) and so that it does not
	// invalidate every later command by sitting in the definition list.
	if (tref ref = get_unbindable_relative_offset<node>(
		t[0].get(), t[1].get()); ref)
	{
		TAU_LOG_ERROR << "Definition " << tau::get(def).to_str()
			<< " cannot use the relative offset of "
			<< tau::get(ref).to_str() << ": its head declares no "
			"offset to bind it. Give the head an offset, as in "
			"f[n](x), or use a fixed offset";
		return;
	}
	rr_defs.push_back(tau::geth(def));
	size_t idx = rr_defs.size() - 1;
	std::cout << "[" << idx + 1 << "] " << tau::get(rr_defs[idx]->get()).to_str() << "\n";
	// Register definition head early so type inference recognizes it
	tt rrt(def);
	htref head = rrt | tt::first | tt::handle;
	htref body = rrt | tt::second | tt::handle;
	if (head) definitions<node>::instance().add(head, body);
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::def_list_cmd() {
	auto& defs = definitions<node>::instance();
	if (rr_defs.empty()) std::cout << "Definitions: empty\n";
	else std::cout << "Definitions:\n";
	for (size_t i = 0; i < rr_defs.size(); i++)
		std::cout << "    [" << i + 1 << "] "
			<< tau::get(rr_defs[i]->get()).to_str() << "\n";
	if (io_defs.empty()) std::cout << "Streams: empty\n";
	else std::cout << "Streams:\n";
	for (size_t i = 0; i < io_defs.size(); i++)
		std::cout << "    [" << i + 1 << "] "
			<< tau::get(io_defs[i]->get()).to_str() << "\n";
	std::cout << *defs.get_io_context();
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::def_print_cmd(const tt& command) {
	auto num = command | tau::num;
	if (!num) return;
	auto i = num | tt::num;
	if (i && i <= rr_defs.size()) {
		std::cout << tau::get(rr_defs[i-1]->get()).to_str() << "\n";
		return;
	}
	TAU_LOG_ERROR << "Definition [" << i << "] does not exist\n";
	return;
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::def_input_cmd(const tt& n) {
	tref def = n | tt::first | tt::ref;
	// tree<node>::geth() asserts on a null tref, and the read sites
	// below would dereference it anyway.
	if (!def) {
		TAU_LOG_ERROR << "Invalid stream definition";
		return;
	}
	// IN-R6: `w_` is the reserved CTL* witness prefix -- an executed E
	// reduction registers internal output streams named w_<n>, so a user
	// stream with that prefix would collide with them.
	if (tref name_node = tau::get(def).first(); name_node) {
		const std::string sname = tau::get(name_node).to_str();
		if (sname.rfind("w_", 0) == 0) {
			TAU_LOG_ERROR << "Stream name '" << sname
				<< "' uses the reserved witness prefix `w_`\n";
			return;
		}
	}
	io_defs.push_back(tau::geth(def));
	size_t idx = io_defs.size() - 1;
	std::cout << "[" << idx + 1 << "] " << tau::get(io_defs[idx]->get()).to_str() << "\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::def_output_cmd(const tt& n) {
	tref def = n | tt::first | tt::ref;
	// tree<node>::geth() asserts on a null tref, and the read sites
	// below would dereference it anyway.
	if (!def) {
		TAU_LOG_ERROR << "Invalid stream definition";
		return;
	}
	// IN-R6: `w_` is the reserved CTL* witness prefix -- an executed E
	// reduction registers internal output streams named w_<n>, so a user
	// stream with that prefix would collide with them.
	if (tref name_node = tau::get(def).first(); name_node) {
		const std::string sname = tau::get(name_node).to_str();
		if (sname.rfind("w_", 0) == 0) {
			TAU_LOG_ERROR << "Stream name '" << sname
				<< "' uses the reserved witness prefix `w_`\n";
			return;
		}
	}
	io_defs.push_back(tau::geth(def));
	size_t idx = io_defs.size() - 1;
	std::cout << "[" << idx + 1 << "] " << tau::get(io_defs[idx]->get()).to_str() << "\n";
}

// make a nso_rr from the given tau source and binder.
template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::make_cli(const std::string& src) {
	// Remove ascii char 22 only. '#' comments are handled by the grammar;
	// a manual strip here was brace-blind and swallowed {#b...} constants.
	std::string filt;
	filt.reserve(src.size());
	for (size_t i = 0; i < src.size(); ) {
		if (static_cast<unsigned char>(src[i]) == 22) {
			++i; // skip ascii 22
		} else {
			filt += src[i++];
		}
	}
	tau_parser::result result = tau_parser::instance()
		.parse(filt.c_str(), filt.size(), {
						.start = tau::cli });
	auto fail = [this]() { return error = true, nullptr; };
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(tau_parser::error::info_lvl::INFO_BASIC);
		if (opt.error_quits
			|| msg.find("Syntax Error: Unexpected end")!=0)
		{
			std::string hint = classify_parse_error<node>(filt);
			TAU_LOG_ERROR << "[repl] " << msg
				<< (hint.empty() ? "" : "\nhint: " + hint) << "\n";
			return fail();
		}
		return nullptr; // Unexpected eof, continue with reading input
	}
	auto t = result.get_shaped_tree2();
	auto& defs = definitions<node>::instance();
	typename tau::get_options opts = {
		.infer_ba_types = true,
		.use_default_types = false,
		.reget_with_hooks = false,
		.definition_heads = defs.get_definition_heads(),
		.global_scope = defs.get_global_scope(),
		.context = defs.get_io_context()
	};
	auto bound = tau::get(tau_parser::tree::get(t), opts);
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
	if (x == "B" || x == "blasting")     return blasting_opt;
	if (x == "H" || x == "highlighting"
		|| x == "highlight")         return highlighting_opt;
	if (x == "I" || x == "indenting"
		|| x == "indent")            return indenting_opt;
	// RE-1: this arm used to claim "B" as well, but blasting_opt above
	// already matches it, so the benchmarks short option was unreachable
	// and `get B` silently meant blasting. Benchmarks gets the still-free
	// lowercase "b" instead, which leaves blasting's "B" alone.
	if (x == "b" || x == "benchmarks"
		|| x == "benchmarking")      return print_benchmarks_opt;
	if (x == "d" || x == "debug"
		|| x == "dbg")               return debug_opt;
	// Full names only: every single letter that would fit is taken (see
	// RE-1 above). These two are numeric options, not flags.
	//
	// No underscore in the spelling: the grammar has
	// `option_name => alnum+` (parser/tau.tgf:231), so `block_max_splits`
	// does not even parse as an option name. Every existing option is a
	// single alnum word for the same reason (`charvar`, `benchmarks`).
	if (x == "maxsplits"
		|| x == "blockmaxsplits")    return block_max_splits_opt;
	if (x == "maxrounds"
		|| x == "blockmaxrounds")    return block_max_rounds_opt;
	if (x == "fixpointsteps"
		|| x == "maxfixpointsteps")  return fixpoint_steps_opt;
	if (x == "flagsteps"
		|| x == "maxflagsearchsteps") return flag_search_steps_opt;
	if (x == "blastdepth"
		|| x == "maxblastreentrydepth") return blast_depth_opt;
	if (x == "squeezecap"
		|| x == "blocksqueezecap")   return squeeze_cap_opt;
	if (x == "simplifyrounds"
		|| x == "maxsimplifyrounds") return simplify_rounds_opt;
	if (x == "defpasses"
		|| x == "maxdefpasses")      return def_passes_opt;
	if (x == "enumsteps"
		|| x == "maxenumsteps")      return enum_steps_opt;
	if (x == "rewriterounds"
		|| x == "maxrewriterounds")  return rewrite_rounds_opt;
	if (x == "gcminsize")                return gc_min_size_opt;
	if (x == "gcgrowth"
		|| x == "gcgrowthfactor")    return gc_growth_opt;
	if (x == "specsizewarn")             return spec_size_warn_opt;
	if (x == "revisionalts"
		|| x == "maxrevisionalts")   return revision_alts_opt;
	if (x == "maxsubsets"
		|| x == "maxconsistencysubsets") return consistency_subsets_opt;
	if (x == "cachebound")               return cache_bound_opt;
	if (x == "maxcoverproducts")         return cover_products_opt;
	TAU_LOG_ERROR << "Invalid option: " << x << "\n";
	return invalid_opt;
}

template <NodeType node>
repl_option get_opt(const typename tree<node>::traverser& n) {
	auto o = n | tau_parser::option_name;
	if (!o) return none_opt;
	return get_opt(o | tree<node>::traverser::string);
}

inline std::optional<boost::log::trivial::severity_level>
	str2severity(const std::string& v)
{
	// TODO (LOW) should we add also warning? and what about fatal?
	if (v == "e" || v == "error") return { boost::log::trivial::error };
	if (v == "d" || v == "debug") return { boost::log::trivial::debug };
	if (v == "t" || v == "trace") return { boost::log::trivial::trace };
	if (v == "i" || v == "info")  return { boost::log::trivial::info };
	TAU_LOG_ERROR << "Invalid severity value: " << v
		<< " (only error, info, debug or trace are allowed)\n";
	return {};
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::get_cmd(const tt& n) {
	return get_cmd(get_opt<node>(n));
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::get_cmd(repl_option o) {
	static std::string pbool[] = { "off", "on" };
	// Not static: the lambdas below capture `this` and must be rebuilt on
	// every call, otherwise a static map would keep referring to whichever
	// repl_evaluator instance happened to call get_cmd() first.
	std::map<repl_option, std::function<void()>> printers = {
#ifdef DEBUG
	{ debug_opt, [this]() {
		std::cout << "debug-repl:          " << pbool[opt.debug_repl] << "\n"; } },
#endif // DEBUG
	{ status_opt,       [this]() {
		std::cout << "status:              " << pbool[opt.status] << "\n"; } },
	{ colors_opt,       [this]() {
		std::cout << "colors:              " << pbool[opt.colors] << "\n"; } },
	{ charvar_opt,      [this]() {
		std::cout << "charvar:             " << pbool[opt.charvar] << "\n"; } },
	{ blasting_opt,      [this]() {
		std::cout << "blasting:            " << pbool[opt.blasting] << "\n"; } },
	{ highlighting_opt, []() {
		std::cout << "syntax highlighting: " << pbool[pretty_printer_highlighting] << "\n"; } },
	{ indenting_opt,    []() {
		std::cout << "indenting:           " << pbool[pretty_printer_indenting] << "\n"; } },
	{ severity_opt,     [this]() {
		std::cout << "severity:            " << opt.severity << "\n"; } },
	{ print_benchmarks_opt, [this]() {
		std::cout << "benchmarks:          " << pbool[opt.print_benchmarks] << "\n"; } }
	};
	// Read from the library globals, not from `opt`: they are what the
	// algorithm actually consults, and a caller using the api setters
	// directly would otherwise be misreported here. Both "unlimited"
	// representations print alike: 0 for the caps and SIZE_MAX for the
	// two decrementing block budgets.
	auto climit = [](size_t v) -> std::string {
		return v == 0 || v == std::numeric_limits<size_t>::max()
			? "unlimited" : std::to_string(v); };
	std::map<repl_option, std::function<void()>> limit_printers = {
	{ block_max_splits_opt, [climit]() {
		std::cout << "maxsplits:           " << climit(block_boole_max_splits) << "\n"; } },
	{ block_max_rounds_opt, [climit]() {
		std::cout << "maxrounds:           " << climit(block_max_rounds) << "\n"; } },
	{ fixpoint_steps_opt, [climit]() {
		std::cout << "fixpointsteps:       " << climit(max_fixpoint_steps) << "\n"; } },
	{ flag_search_steps_opt, [climit]() {
		std::cout << "flagsteps:           " << climit(max_flag_search_steps) << "\n"; } },
	{ blast_depth_opt, [climit]() {
		std::cout << "blastdepth:          " << climit(max_blast_reentry_depth) << "\n"; } },
	{ squeeze_cap_opt, [climit]() {
		std::cout << "squeezecap:          " << climit(block_squeeze_cap) << "\n"; } },
	{ simplify_rounds_opt, [climit]() {
		std::cout << "simplifyrounds:      " << climit(max_simplify_rounds) << "\n"; } },
	{ def_passes_opt, [climit]() {
		std::cout << "defpasses:           " << climit(max_def_passes) << "\n"; } },
	{ enum_steps_opt, [climit]() {
		std::cout << "enumsteps:           " << climit(max_enum_steps) << "\n"; } },
	{ rewrite_rounds_opt, [climit]() {
		std::cout << "rewriterounds:       " << climit(max_rewrite_rounds) << "\n"; } },
	{ gc_min_size_opt, []() {
		std::cout << "gcminsize:           " << interpreter<node>::gc_min_size << "\n"; } },
	{ gc_growth_opt, []() {
		std::cout << "gcgrowth:            " << interpreter<node>::gc_growth_factor << "\n"; } },
	{ spec_size_warn_opt, []() {
		const size_t v = interpreter<node>::spec_size_warn_threshold;
		std::cout << "specsizewarn:        "
			<< (v ? std::to_string(v) : "off") << "\n"; } },
	{ revision_alts_opt, [climit]() {
		std::cout << "revisionalts:        " << climit(interpreter<node>::max_revision_alts) << "\n"; } },
	{ consistency_subsets_opt, [climit]() {
		std::cout << "maxsubsets:          " << climit(max_consistency_subsets) << "\n"; } },
	{ cache_bound_opt, [climit]() {
		std::cout << "cachebound:          " << climit(cache_bound) << "\n"; } },
	{ cover_products_opt, [climit]() {
		std::cout << "maxcoverproducts:    " << climit(max_cover_products) << "\n"; } }
	};
	printers.insert(limit_printers.begin(), limit_printers.end());
	if (o == invalid_opt) return;
#ifndef DEBUG
	// RE-2: answering a query about an option this build does not carry is
	// not an error condition, it is the answer. Reported at info level so a
	// plain `get debug` no longer prints "(Error)" in a release build.
	if (o == debug_opt) {
		TAU_LOG_INFO << "Debug option not available in release build\n";
		return;
	}
#endif // DEBUG
	if (o == none_opt) { for (auto& [_, v] : printers) v(); return; }
	printers[o]();
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::set_cmd(const tt& n) {
	repl_option o = get_opt<node>(n);
	auto ov = n | tau::option_value;
	if (!ov) { TAU_LOG_ERROR << "Invalid value\n"; return; }
	set_cmd(o, ov | tt::string);
	get_cmd(n);
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::set_cmd(repl_option o, const std::string& v) {
	using namespace boost::log;
	if (o == invalid_opt || o == none_opt) return;
#ifndef DEBUG
	// RE-2: a warning, not an error -- the command was understood, it just
	// cannot take effect in a build without DEBUG.
	if (o == debug_opt) {
		TAU_LOG_WARNING << "Debug option not available in release build\n";
		return;
	}
#endif // DEBUG
	// A count. Zero is accepted and means "unlimited" (specsizewarn: off)
	// by the unified limit-option convention -- the api setters translate
	// it to each knob's internal representation.
	auto str2count = [&v](void) -> std::optional<size_t> {
		size_t n = 0;
		if (v.empty()) { TAU_LOG_ERROR << "Invalid value\n"; return {}; }
		for (char c : v) {
			if (c < '0' || c > '9') {
				TAU_LOG_ERROR << "Invalid value: expected a "
					"count\n";
				return {};
			}
			n = n * 10 + static_cast<size_t>(c - '0');
		}
		return n;
	};
	// A decimal number (gcgrowth); the grammar admits digits and '.'.
	auto str2double = [&v](void) -> std::optional<double> {
		try {
			size_t pos = 0;
			double d = std::stod(v, &pos);
			if (pos != v.size())
				throw std::invalid_argument(v);
			return d;
		} catch (const std::exception&) {
			TAU_LOG_ERROR << "Invalid value: expected a number\n";
			return {};
		}
	};
	auto update_bool_value = [&v](bool& opt) {
		if (v == "t" || v == "true" || v == "on" || v == "1"
			|| v == "y" || v == "yes") opt = true;
		else if (v == "f" || v == "false" || v == "off" || v == "0"
			|| v == "n" || v == "no") opt = false;
		else TAU_LOG_ERROR << "Invalid value\n";
		return opt;
	};
	// Not static: the lambdas below capture `this`, `v` and the local
	// update_bool_value lambda by reference, all of which are only valid
	// for the duration of this call. A static map would keep those
	// references pointing at the FIRST call's (long-gone) stack frame,
	// causing every later set command to read dangling references.
	std::map<repl_option, std::function<void()>> setters = {
#ifdef DEBUG
	{ debug_opt, [&]() {
		update_bool_value(opt.debug_repl); } },
#endif // DEBUG
	{ status_opt,   [&]() {
		update_bool_value(opt.status); } },
	{ colors_opt,   [&]() {
		TC.set(update_bool_value(opt.colors)); } },
	{ charvar_opt,   [&]() {
		update_charvar(update_bool_value(opt.charvar)); } },
	{ blasting_opt,   [&]() {
		update_blasting(update_bool_value(opt.blasting)); } },
	{ highlighting_opt,   [&]() {
		update_bool_value(pretty_printer_highlighting); } },
	{ indenting_opt,   [&]() {
		update_bool_value(pretty_printer_indenting); } },
	{ print_benchmarks_opt, [&]() {
		update_bool_value(opt.print_benchmarks); } },
	{ severity_opt, [&]() {
		auto sev = str2severity(v);
		if (!sev.has_value()) return;
		opt.severity = sev.value();
		logging::set_filter(opt.severity);
	} },
	// Every numeric option funnels through its api setter, the same
	// surface the CLI options use, so the two stay in lockstep.
	{ block_max_splits_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_block_max_splits(*n); } },
	{ block_max_rounds_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_block_max_rounds(*n); } },
	{ fixpoint_steps_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_fixpoint_steps(*n); } },
	{ flag_search_steps_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_flag_search_steps(*n); } },
	{ blast_depth_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_blast_reentry_depth(*n); } },
	{ squeeze_cap_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_block_squeeze_cap(*n); } },
	{ simplify_rounds_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_simplify_rounds(*n); } },
	{ def_passes_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_def_passes(*n); } },
	{ enum_steps_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_enum_steps(*n); } },
	{ rewrite_rounds_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_rewrite_rounds(*n); } },
	{ gc_min_size_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_gc_min_size(*n); } },
	{ gc_growth_opt, [&]() { if (auto d = str2double(); d)
		api<node>::set_gc_growth_factor(*d); } },
	{ spec_size_warn_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_spec_size_warn(*n); } },
	{ revision_alts_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_revision_alts(*n); } },
	{ consistency_subsets_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_consistency_subsets(*n); } },
	{ cache_bound_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_cache_bound(*n); } },
	{ cover_products_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_cover_products(*n); } } };
	setters[o]();
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::update_bool_opt_cmd(const tt& n,
	const std::function<bool(bool&)>& update_fn)
{
	auto o = get_opt<node>(n);
	update_bool_opt_cmd(o, update_fn);
	if (!error) get_cmd(n);
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::update_bool_opt_cmd(repl_option o,
	const std::function<bool(bool&)>& update_fn)
{
	if (o == invalid_opt || o == none_opt) return;
#ifndef DEBUG
	// RE-2: a warning, not an error -- the command was understood, it just
	// cannot take effect in a build without DEBUG.
	if (o == debug_opt) {
		TAU_LOG_WARNING << "Debug option not available in release build\n";
		return;
	}
#endif // DEBUG
	switch (o) {
#ifdef DEBUG
	case debug_opt: update_fn(opt.debug_repl); break;
#endif // DEBUG
	case colors_opt:           TC.set(update_fn(opt.colors)); break;
	case charvar_opt:          update_charvar(update_fn(opt.charvar));break;
	case blasting_opt:     	   update_blasting(update_fn(opt.blasting)); break;
	case highlighting_opt:     update_fn(pretty_printer_highlighting);break;
	case indenting_opt:        update_fn(pretty_printer_indenting); break;
	case status_opt:           update_fn(opt.status); break;
	case print_benchmarks_opt: update_fn(opt.print_benchmarks); break;
	case block_max_splits_opt:
	case block_max_rounds_opt:
	case fixpoint_steps_opt:
	case flag_search_steps_opt:
	case blast_depth_opt:
	case squeeze_cap_opt:
	case simplify_rounds_opt:
	case def_passes_opt:
	case enum_steps_opt:
	case rewrite_rounds_opt:
	case gc_min_size_opt:
	case gc_growth_opt:
	case spec_size_warn_opt:
	case revision_alts_opt:
	case consistency_subsets_opt:
	case cache_bound_opt:
	case cover_products_opt:
		TAU_LOG_ERROR << "This option takes a count, not a flag: use "
			"`set <option> <n>`\n", error = true;
		return;
	default: TAU_LOG_ERROR << "Invalid option\n", error = true; return;
	}
}

template <typename... BAs>
requires BAsPack<BAs...>
bool repl_evaluator<BAs...>::update_charvar(bool value) {
	api<node>::set_charvar(opt.charvar = value);
	return value;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool repl_evaluator<BAs...>::update_blasting(bool value) {
	api<node>::set_blasting(opt.blasting = value);
	return value;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool repl_evaluator<BAs...>::reject_ctl_star_if_disabled(tref fm) {
	if (opt.fragment == fragment_ctl_star || !fm) return false;
	if (has_ctl_star_operators<node>(fm)) {
		TAU_LOG_ERROR << "CTL* operators (A/E/-) require the ctl_star "
			"fragment. Switch with: fragment ctl_star\n";
		error = true;
		return true;
	}
	return false;
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::fragment_cmd(const tt& n) {
	auto fn = n | tau_parser::fragment_name;
	if (!fn) {
		TAU_LOG_ERROR << "Missing fragment name\n";
		error = true;
		return;
	}
	auto fnt = fn | tt::only_child | tt::nt;
	if (fnt == tau_parser::fragment_ltl) {
		opt.fragment = fragment_ltl;
		std::cout << "fragment: ltl\n";
	} else if (fnt == tau_parser::fragment_ctl_star) {
		opt.fragment = fragment_ctl_star;
		std::cout << "fragment: ctl_star\n";
	} else {
		TAU_LOG_ERROR << "Unknown fragment. Available: ltl, ctl_star\n";
		error = true;
	}
}

template <typename... BAs>
requires BAsPack<BAs...>
int repl_evaluator<BAs...>::eval_cmd(const tt& n) {
	auto command = n | tt::only_child;
	auto command_type = command | tt::nt;
#ifdef DEBUG
	if (opt.debug_repl) {
		// std::cout << "command: " << command << "\n";
		command.value_tree().print_tree(std::cout << "tree: ") << "\n";
	}
#endif // DEBUG
	tref result = 0;
	switch (command_type) {
	case tau::quit_cmd:           return std::cout << "Quit.\n", 1;
	case tau::clear_cmd:
		if (r) r->clear();
#ifdef TAU_PARSER_HAS_FTXUI
		else if (r_ftx) r_ftx->clear();
#endif
		break;
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
	case tau::fragment_cmd:       fragment_cmd(command); break;
	case tau::history_list_cmd:   history_list_cmd(); break;
	case tau::history_print_cmd:  history_print_cmd(command); break;
	case tau::history_store_cmd:  history_store_cmd(command); break;
	// normalization
	case tau::normalize_cmd:      result = normalize_cmd(command); break;
	// execution
	case tau::run_cmd:            run_cmd(command); break;
	case tau::stop_cmd:           stop_cmd(); break;
	case tau::memory_cmd:         memory_cmd(); break;
	case tau::ltl_cmd:            ltl_cmd(command); break;
	case tau::solve_cmd:          solve_cmd(command); break;
	case tau::lgrs_cmd:           lgrs_cmd(command); break;
	// substitution and instantiation
	case tau::subst_cmd:          result = subst_cmd(command); break;
	case tau::inst_cmd:           result = inst_cmd(command); break;
	// formula checks
	case tau::sat_cmd:            result = sat_cmd(command); break;
	case tau::valid_cmd:          result = valid_cmd(command); break;
	case tau::unsat_cmd:          result = unsat_cmd(command); break;
	case tau::realizable_cmd:     result = realizable_cmd(command); break;
	case tau::unrealizable_cmd:   result = unrealizable_cmd(command); break;
	// normal forms
	case tau::onf_cmd:            result = onf_cmd(command); break;
	case tau::dnf_cmd:            result = dnf_cmd(command); break;
	case tau::cnf_cmd:            result = cnf_cmd(command); break;
	case tau::nnf_cmd:            result = nnf_cmd(command); break;
	case tau::mnf_cmd:            result = mnf_cmd(command); break;
	// definition of rec relations to be included during normalization
	case tau::def_rr_cmd:         def_rr_cmd(command); break;
	case tau::def_list_cmd:       def_list_cmd(); break;
	case tau::def_print_cmd:      def_print_cmd(command); break;
	// definitions of i/o streams
	case tau::def_input_cmd:      def_input_cmd(command); break;
	case tau::def_output_cmd:     def_output_cmd(command); break;
	// qelim
	case tau::qelim_cmd:          result = qelim_cmd(command); break;
	// type inspection and session management
	case tau::whatis_cmd:         result = whatis_cmd(command); break;
	case tau::reset_cmd:          reset_cmd(); break;
	case tau::comment:            break;
	// error handling
	default: error = true; std::cout << std::endl;
		TAU_LOG_ERROR << "Unknown command";
	}
#ifdef DEBUG
	if (opt.debug_repl && result) tau::get(result).print_tree(
		std::cout << "result tree: ") << "\n";
#endif // DEBUG
	if (result) history_store(result);
	return 0;
}

template <typename... BAs>
requires BAsPack<BAs...>
repl_evaluator<BAs...>::repl_evaluator(options opt): opt(opt)
{
	TC.set(opt.colors);
	logging::set_filter(opt.severity);
	if (opt.experimental) std::cout << "\n!!! Experimental features "
		"enabled (expect unstable behavior) !!!\n\n";
	// Propagate the CLI-provided charvar/blasting values to the api's
	// global state; without this, --charvar/--blasting have no effect in
	// REPL mode until the user runs "set"/"toggle" (they were only ever
	// applied to the api in main.cpp's non-interactive spec-file path).
	update_charvar(opt.charvar);
	update_blasting(opt.blasting);
	// console input streams resolve through the REPL cycle, never blocking
	definitions<node>::instance().get_io_context()->console_input_factory =
		[](const std::string&) {
			return std::make_shared<repl_pending_input_stream>();
		};
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::reprompt() {
	std::stringstream ss;
	if (pending) {
		// pending->label already ends with one trailing space
		ss << (error ? TC_ERROR : TC_PROMPT) << pending->label
			<< TC.CLEAR();
	} else {
		if (opt.status) {
			std::stringstream status;
			if (H.size()) status << " " << TC_STATUS_OUTPUT << "%"
				<< H.size() << TC.CLEAR() << TC_STATUS;
			if (opt.severity != boost::log::trivial::info)
				status << " " << to_string(opt.severity);
			if (status.tellp()) ss << TC_STATUS << "["
				<< status.str() << " ]" << TC.CLEAR() << " ";
		}
		ss << (error ? TC_ERROR : TC_PROMPT) << "tau>" << TC.CLEAR()
			<< " ";
	}
	if (r) r->set_prompt(ss.str());
#ifdef TAU_PARSER_HAS_FTXUI
	if (r_ftx) r_ftx->set_prompt(ss.str());
#endif
}

template <typename... BAs>
requires BAsPack<BAs...>
int repl_evaluator<BAs...>::eval(const std::string& src) {
	// while a `run` session is pending, src is its answer, not a new command
	if (pending) {
		// incomplete value: return 2 so more lines accumulate (multiline)
		if (!run_abort_ && pending->kind == pending_request::stream_value
			&& stream_value_incomplete(src, pending->type_tree))
					return 2;
		auto req = *pending;
		pending.reset();
		if (!run_abort_ && req.kind == pending_request::stream_value)
			req.stream->set(src);
		bool stop = run_abort_
			|| (req.kind == pending_request::continue_or_quit
				&& (src == "q" || src == "quit"));
		run_abort_ = false;
		if (stop) finish_running();
		else {
			running->t.unpause();
			if (req.kind == pending_request::stream_value)
				continue_running(req);
			else continue_running();
		}
		std::cout << "\n", std::cout.flush();
		if (!pending) reprompt();
		return 0;
	}
	error = false;
	tref cli = make_cli(src);
	// Pin the parsed command line for the whole evaluation: a `run` among
	// its commands steps the interpreter, which calls maybe_gc(), and the
	// commands still queued behind it live in this very tree. A line that
	// failed to parse has no tree and needs no pin -- tree<node>::geth()
	// asserts on a null tref, unlike bintree's.
	htref cli_pin = cli ? tau::geth(cli) : htref{};
	auto tau_spec = tt(cli);
	int quit = 0;
	if (tau_spec) {
		auto commands = tau_spec || tau::cli_command;
		for (const auto& cmd : commands())
			if (quit = eval_cmd(cmd); quit == 1) break;
	} else if (!error) return 2;
	std::cout << "\n", std::cout.flush();
	if (error && opt.error_quits) return quit = 1;
	if (quit == 0) reprompt();
	return quit;
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::version_cmd() {
	std::cout << full_version << "\n"
		<< "algebras: " << node::ba::types_joined() << "\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::help_cmd(const tt& n) const {
	auto arg = n | tau::help_arg | tt::only_child;
	size_t nt = arg ? (arg | tt::nt) : tau::help_sym;
	help(nt);
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::help(size_t nt) const {
	static const std::string bool_options =
		"  <option>               <description>                        <value>\n"
#ifdef DEBUG
		"  debug-repl             show REPL commands                   on/off\n"
#endif // DEBUG
		"  status                 show status                          on/off\n"
		"  colors                 use term colors                      on/off\n"
		"  highlighting           syntax highlighting of Tau formulas  on/off\n"
		"  indenting              indenting of Tau formulas            on/off\n"
		"  charvar (V)            character-variable notation          on/off\n"
		"  blasting (B)           bitvector predicate blasting         on/off\n"
		"  benchmarks (b)         print timing benchmarks              on/off\n";
	static const std::string numeric_options =
		"and the numeric limit options, set with `set <option> <n>` "
		"(0 = unlimited;\nspecsizewarn: 0 = off; gcgrowth <= 0 disables "
		"gc; each mirrors the CLI option\nof the same meaning):\n"
		"  <option>               <description>                        <default>\n"
		"  maxsplits              anti-prenex per-block Boole splits   unlimited\n"
		"  maxrounds              anti-prenex driver rounds            unlimited\n"
		"  fixpointsteps          temporal-normalization fixpoint steps unlimited\n"
		"  flagsteps              eventual-flag search steps           unlimited\n"
		"  blastdepth             blast-block re-entry nesting         unlimited\n"
		"  squeezecap             block-squeeze operand-set size cap   unlimited\n"
		"  simplifyrounds         bitvector simplification rounds      unlimited\n"
		"  defpasses              definition-expansion passes          unlimited\n"
		"  enumsteps              recurrence enumeration steps         unlimited\n"
		"  rewriterounds          rewrite-to-fixpoint rounds           unlimited\n"
		"  gcminsize              gc trigger floor (tree nodes)        256\n"
		"  gcgrowth               gc growth-factor trigger (decimal)   1.5\n"
		"  specsizewarn           updated-spec size warning (chars)    off\n"
		"  revisionalts           revision alternatives kept per part  unlimited\n"
		"  maxsubsets             k-ary consistency subset checks      4096\n"
		"  cachebound             string-keyed synthesis cache bound   4096\n"
		"  maxcoverproducts       oracle mixed-type coverage products  256\n";
	static const std::string all_available_options = std::string{} +
		"Available options and values:\n" + bool_options +
		"  severity               severity                             error/info/debug/trace\n"
		+ numeric_options;
	static const std::string bool_available_options = std::string{} +
		"Available options and values:\n" + bool_options;
	switch (nt) {
	case tau::help_sym: std::cout
		<< "General commands:\n"
		<< "  help or h               print overview of available commands in Tau repl\n"
		<< "  quit or q               exit the Tau repl\n"
		<< "  version or v            print current version\n"
		<< "  clear                   clear the screen\n"
		<< "\n"

		<< "Run command:\n"
		<< "  run                     execute a Tau formula as a program\n"
		<< "  stop                    clear the stored run session\n"
		<< "  memory                  print the current run session's memory (variable-to-value map)\n"
		<< "  ltl                     print the full LTL(ABA) translation pipeline\n"
		<< "\n"

		<< "Logical procedures:\n"
		<< "  normalize or n          normalize a Tau expression\n"
		<< "  qelim                   eliminate non-temporal quantifiers in a Tau formula\n"
		<< "  sat                     check if a Tau formula is satisfiable\n"
		<< "  unsat                   check if a Tau formula is unsatisfiable\n"
		<< "  valid                   check if a Tau formula is valid\n"
		<< "  realizable              check if a Tau specification is realizable\n"
		<< "  unrealizable            check if a Tau specification is unrealizable\n"
		<< "  solve                   compute a satisfying assignment for the free variables in a Tau formula\n"
		<< "  lgrs                    compute a LGRS for a given equation\n"
		<< "\n"

		<< "Normal form commands:\n"
		<< "  cnf                     convert a Tau expression to conjunctive normal form\n"
		<< "  dnf                     convert a Tau expression to disjunctive normal form\n"
		<< "  mnf                     convert a Tau expression to minterm normal form\n"
		<< "  anf                     convert a Tau expression to algebraic normal form\n"
		<< "  nnf                     convert a Tau expression to negation normal form\n"
		<< "  pnf                     convert a Tau expression to prenex normal form\n"
		<< "  onf                     convert a Tau formula to order normal form\n"
		<< "\n"

		<< "History and definitions:\n"
		<< "  history or hist         show all Tau expressions stored in the repl history\n"
		<< "  definitions or defs     show stored IO variables and function and predicate definitions\n"
		<< "  reset                   clear history, definitions, and IO streams\n"
		<< "\n"

		<< "Inspection commands:\n"
		<< "  whatis                  show the inferred type of a Tau expression\n"
		<< "\n"

		<< "Substitution and instantiation command:\n"
		<< "  substitute, subst or s  substitute a Tau expression in a Tau expression by another\n"
		<< "  instantiate, inst or i  instantiate a variable in a Tau expression with a Tau term\n"
		<< "\n"

		<< "Fragment selection:\n"
		<< "  fragment ltl            use default LTL fragment (no A/E/-)\n"
		<< "  fragment ctl_star       enable CTL* fragment (adds A, E, - operators)\n"
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
	case tau::version_sym: std::cout
		<< "version prints the current Tau version\n";
		break;
	case tau::quit_sym: std::cout
		<< "quit exits the Tau repl\n";
		break;
	case tau::clear_sym: std::cout
		<< "clear clears the terminal screen\n"
		<< "\n"
		<< "usage:\n"
		<< "  clear or c              clears the terminal screen\n";
		break;
	case tau::get_sym: std::cout
		<< "get                       prints all options and their values\n"
		<< "get <option>              prints the value of the given option\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau::set_sym: std::cout
		<< "set <option> [=] <value> sets option to value\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau::enable_sym: std::cout
		<< "enable <option>           enables option\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau::disable_sym: std::cout
		<< "disable <option>          disables option\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau::toggle_sym: std::cout
		<< "toggle <option>           toggles option value\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau::history_sym: std::cout
		<< "the history command shows all stored Tau expressions in the repl history\n"
		<< "\n"
		<< "  history or hist                 shows all stored Tau expressions\n"
		<< "  history or hist <repl_history>  shows the Tau expression stored at the specified repl history position\n"
		<< "  <rr>, <tau> or <term>           stores a Tau expression in the repl history at the latest position\n"
		<< "\n"
		<< "the repl history can be accessed in the following ways\n\n"
		<< "  %                       to retrieve the Tau expression stored at the latest position\n"
		<< "  %-<number>              to retrieve the Tau expression stored at the latest position minus <number>\n"
		<< "  %<number>               to retrieve the Tau expression stored at position <number>\n\n"
		<< "stored Tau expressions can be used wherever a command expects a Tau expression\n";
		break;
	case tau::normalize_sym: std::cout
		<< "the normalize command normalizes a Tau expression, prints the result and\n"
		<< "saves it into the repl history\n"
		<< "\n"
		<< "usage:\n"
		<< "  normalize <rr>            normalizes the given tau formula with additional predicate and function definitions\n"
		<< "  normalize <tau>           normalizes the given tau formula\n"
		<< "  normalize <term>          normalizes the given term\n"
		<< "  normalize <repl_history>  normalizes the Tau expression stored at the specified repl history position\n";
		break;
	case tau::qelim_sym: std::cout
		<< "the qelim command eliminates all non-temporal quantifiers, prints the result and\n"
		<< "saves it into the repl history\n"
		<< "\n"
		<< "usage:\n"
		<< "  qelim <tau>             eliminates non-temporal quantifiers in the given tau formula\n"
		<< "  qelim <repl_history>    eliminates non-temporal quantifiers in the Tau formula stored at the specified repl history position\n";
		break;
	case tau::ltl_sym: std::cout
		<< "the ltl command prints the full LTL(ABA) translation pipeline\n"
		<< "\n"
		<< "usage:\n"
		<< "  ltl <formula>           print the translation pipeline for the given LTL(ABA) formula\n"
		<< "  ltl <repl_history>      print the pipeline for the formula at the given history position\n"
		<< "\n"
		<< "pipeline steps shown:\n"
		<< "  - data atom extraction: which ABA subformulas map to propositional variables\n"
		<< "  - input/output classification of each proposition\n"
		<< "  - ABA consistency constraints added to the LTL skeleton\n"
		<< "  - LTL skeleton sent to ltlsynt\n"
		<< "  - ltlsynt result (REALIZABLE/UNREALIZABLE)\n"
		<< "  - HOA strategy automaton (states, edges with guards)\n"
		<< "  - ABA oracle feasibility check for each strategy edge\n"
		<< "  - final synthesized safety formula\n"
		<< "\n";
		break;
	case tau::run_sym: std::cout
		<< "the run command executes a Tau formula as a program\n"
		<< "\n"
		<< "run automatically chooses a single program from the set of programs satisfying a given Tau formula\n"
		<< "and executes it in such a way that in each step (starting at 0) each input variable reads a value from the specified stream\n"
		<< "and then a value is written into each specified output stream such that the Tau formula being executed is satisfied\n\n"
		<< "usage:\n"
		<< "  run <tau>               execute the given Tau formula\n"
		<< "  run <repl_history>      execute the Tau formula stored at the specified repl history position\n"
		<< "  run N steps <tau>       execute the given formula for exactly N steps\n"
		<< "  run N steps             continue the stored run for N more steps\n"
		<< "  run                     continue the stored run (until it ends or needs input)\n"
		<< "  stop                    discard the stored run session\n"
		<< "  memory                  print the stored run session's current memory\n"
		<< "\n"
		<< "a new `run <tau>` replaces any stored session; `run` / `run N steps`\n"
		<< "with no formula continue the stored one. `N step` (singular) also works.\n"
		<< "\n";
		break;
	case tau::memory_sym: std::cout
		<< "the memory command prints the running interpreter's current\n"
		<< "memory: the variable-to-value map retained across `run` steps\n"
		<< "\n"
		<< "usage:\n"
		<< "  memory                  print the current run session's memory\n"
		<< "\n"
		<< "requires an active `run` session (start one with `run <tau>` or\n"
		<< "`run N steps <tau>`); prints a message instead of a map if none is active.\n"
		<< "also prints the number of bindings held, since the interpreter prunes\n"
		<< "memory entries no future step can read, so this count is expected to\n"
		<< "stay bounded as steps advance rather than grow without limit.\n"
		<< "\n";
		break;
	case tau::solve_sym: std::cout
		<< "the solve command computes a single satisfying assignment for the free variables in a Tau formula\n"
		<< "\n"
		<< "usage:\n"
		<< "  solve [options] <tau>           computes a single satisfying assignment for the free variables in the Tau formula\n"
		<< "  solve [options] <repl_history>  computes a single satisfying assignment for the free variables in the Tau formula stored at the specified repl history position"
		<< "\n"
		<< "options:\n"
		<< "  --min, --minimum                   computes a minimum solution of the system of equations\n"
		<< "  --max, --maximum                   computes a maximum solution of the system of equations\n"
		<< "  --<type>                           uses the specified type for the solution (sbf or tau)\n"
		<< "\n";
		break;
	case tau::lgrs_sym: std::cout
		<< "the lgrs command computes a LGRS for an equation\n"
		<< "\n"
		<< "usage:\n"
		<< "  lgrs <tau_eq>           computes a LGRS for a given equation\n"
		<< "  lgrs <repl_history>     computes a LGRS for the equation stored at the specified repl history position\n"
		<< "options:\n"
		<< "  --<type>                uses the specified type for the solution\n"
		<< "\n";
		break;
	case tau::whatis_sym: std::cout
		<< "the whatis command shows the inferred type of a Tau expression\n"
		<< "\n"
		<< "usage:\n"
		<< "  whatis <tau>            shows the type of the given Tau expression\n"
		<< "  whatis <term>           shows the type of the given term\n"
		<< "  whatis <repl_history>   shows the type of the expression at the given history position\n"
		<< "\n"
		// the BA type names are whatever the configured pack holds
		<< "type names: wff, bf, " << node::ba::types_joined() << "\n";
		break;
	case tau::reset_sym: std::cout
		<< "the reset command clears the REPL session state\n"
		<< "\n"
		<< "usage:\n"
		<< "  reset                   clears history, definitions, and IO streams\n";
		break;
	case tau::sat_sym: std::cout
		<< "the sat command checks if a Tau formula is satisfiable and if so prints T and else F\n\n"
		<< "a tau formula is satisfiable if there exists a variable assignment to non-temporal variables\n"
		<< "such that for all possible inputs there exist time compatible outputs at each point in time\n"
		<< "\n"
		<< "usage:\n"
		<< "  sat <rr>                checks the given tau formula with additional predicate and function definitions for satisfiability\n"
		<< "  sat <tau>               checks the given tau formula for satisfiability\n"
		<< "  sat <repl_history>      checks the Tau formula stored at the specified repl history position for satisfiability\n";
		break;
	case tau::valid_sym: std::cout
		<< "the valid command checks if a Tau formula is logically equivalent to T and if so prints T and else F\n"
		<< "\n"
		<< "usage:\n"
		<< "  valid <rr>              checks the given tau formula with additional predicate and function definitions for validity\n"
		<< "  valid <tau>             checks the given tau formula for validity\n"
		<< "  valid <repl_history>    checks the Tau formula stored at the specified repl history position for validity\n";
		break;
	case tau::unsat_sym: std::cout
		<< "the unsat command checks if a Tau formula is unsatisfiable and if so prints T and else F\n\n"
		<< "a tau formula is unsatisfiable if for every variable assignment to non-temporal variables\n"
		<< "there exist inputs such that there are no time compatible outputs at some point in time\n"
		<< "\n"
		<< "usage:\n"
		<< "  unsat <rr>              checks the given tau formula with additional predicate and function definitions for unsatisfiability\n"
		<< "  unsat <tau>             checks the given tau formula for unsatisfiability\n"
		<< "  unsat <repl_history>    checks the Tau formula stored at the specified repl history position for unsatisfiability\n";
		break;
	case tau::realizable_sym: std::cout
		<< "the realizable command checks if a Tau specification is realizable and if so prints T and else F\n\n"
		<< "a tau specification is realizable if there exists a winning system strategy that, for\n"
		<< "every possible sequence of inputs, produces outputs satisfying the specification at every point in time\n"
		<< "\n"
		<< "usage:\n"
		<< "  realizable <rr>              checks the given tau formula with additional predicate and function definitions for realizability\n"
		<< "  realizable <tau>             checks the given tau formula for realizability\n"
		<< "  realizable <repl_history>    checks the Tau formula stored at the specified repl history position for realizability\n";
		break;
	case tau::unrealizable_sym: std::cout
		<< "the unrealizable command checks if a Tau specification is unrealizable and if so prints T and else F\n\n"
		<< "a tau specification is unrealizable if no system strategy exists that, for every possible\n"
		<< "sequence of inputs, produces outputs satisfying the specification at every point in time\n"
		<< "\n"
		<< "usage:\n"
		<< "  unrealizable <rr>              checks the given tau formula with additional predicate and function definitions for unrealizability\n"
		<< "  unrealizable <tau>             checks the given tau formula for unrealizability\n"
		<< "  unrealizable <repl_history>    checks the Tau formula stored at the specified repl history position for unrealizability\n";
		break;
	case tau::dnf_sym: std::cout
		<< "dnf converts a Tau expression to disjunctive normal form (DNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  dnf <term>              converts the given term to DNF\n"
		<< "  dnf <tau>               converts the given Tau formula to DNF\n"
		<< "  dnf <repl_history>      converts the Tau expression stored at the specified repl history position to DNF\n";
		break;
	case tau::cnf_sym: std::cout
		<< "cnf converts a Tau expression to conjunctive normal form (CNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  cnf <term>              converts the given term to CNF\n"
		<< "  cnf <tau>               converts the given Tau formula to CNF\n"
		<< "  cnf <repl_history>      converts the Tau expression stored at the specified repl history position to CNF\n";
		break;
	case tau::nnf_sym: std::cout
		<< "nnf converts a Tau expression to negation normal form (NNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  nnf <term>              converts the given term to NNF\n"
		<< "  nnf <tau>               converts the given tau formula to NNF\n"
		<< "  nnf <repl_history>      converts the Tau expression stored at the specified repl history position to NNF\n";
		break;
	case tau::mnf_sym: std::cout
		<< "mnf converts a Tau expression to minterm normal form (MNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  mnf <term>              converts the given term to MNF\n"
		<< "  mnf <tau>               converts the given tau formula to MNF\n"
		<< "  mnf <repl_history>      converts the Tau expression stored at the specified repl history position to MNF\n";
		break;
	case tau::onf_sym: std::cout
		<< "onf converts a tau formula to order normal form (ONF) on the specified variable\n"
		<< "\n"
		<< "usage:\n"
		<< "  onf <var> <tau>           converts the given tau formula to ONF using <var>\n"
		<< "  onf <var> <repl_history>  converts the Tau formula stored at the specified repl history position to ONF using <var>\n";
		break;
	case tau::subst_sym: std::cout
		<< "the substitute command substitutes a Tau expression in a Tau expression by another Tau expression\n"
		<< "\n"
		<< "usage:\n"
		<< "  substitute <input> '[' <match> / <replace> ']'\n"
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
	case tau::inst_sym: std::cout
		<< "the instantiate command instantiates a variable in a Tau formula with the specified term\n"
		<< "\n"
		<< "usage:\n"
		<< "  instantiate <input> '[' <variable> / <value> ']'\n"
		<< "\n"
		<< "where:\n"
		<< "  <input> is the Tau term to instantiate in\n"
		<< "  <variable> is the variable to be instantiated\n"
		<< "  <value> is the Tau term to instantiate with\n"
		<< "\n";
		break;
	case tau::def_sym: std::cout
		<< "the definitions command shows stored input/output stream variables and function and predicate definitions\n"
		<< "\n"
		<< "usage:\n"
		<< "  <tau_rec_relation>      defines a tau predicate\n"
		<< "  <term_rec_relation>     defines a tau function\n"
		<< "  <def_input_cmd>         defines an input stream variable\n"
		<< "  <def_output_cmd>        defines an output stream variable\n"
		<< "  definitions             lists all definitions present in repl\n"
		<< "  definitions <number>    prints predicate or function at specified position\n"
		<< "\n"
		<< "Examples defining stream variables and functions can be found by typing \'help examples\'\n";
		break;
	case tau::examples_sym: std::cout
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

} // namespace idni::tau_lang
