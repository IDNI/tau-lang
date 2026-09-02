// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "repl_evaluator.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "repl_evaluator"

namespace idni::tau_lang {

using namespace cvc5;

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

// Structural equality ignoring types: `typed` annotation children are skipped
// and every node value is compared with its BA type id erased. Used to match
// a fully unannotated pattern against occurrences whose types were resolved
// by inference -- a pattern parsed from the command line can never carry
// those resolved ids, so it could never be strictly equal to them.
template <typename... BAs>
requires BAsPack<BAs...>
bool repl_evaluator<BAs...>::equal_modulo_types(tref a, tref b) const {
	const auto& na = tau::get(a);
	const auto& nb = tau::get(b);
	// a ba_constant's data field reads differently depending on its
	// ba_type, and constants of different types are distinct anyway, so
	// compare those verbatim (see tree::untype)
	auto key = [](const auto& n) {
		return n.is(tau::ba_constant) ? n.value
			: n.value.ba_retype(untyped_type_id<node>());
	};
	if (!(key(na) == key(nb))) return false;
	trefs ca, cb;
	for (tref c : na.get_children())
		if (!tau::get(c).is(tau::typed)) ca.push_back(c);
	for (tref c : nb.get_children())
		if (!tau::get(c).is(tau::typed)) cb.push_back(c);
	if (ca.size() != cb.size()) return false;
	for (size_t i = 0; i < ca.size(); ++i)
		if (!equal_modulo_types(ca[i], cb[i])) return false;
	return true;
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
	auto& defs = definitions<node>::instance();
	// type_defs is spliced first only for parallel structure with rr_defs/
	// io_defs below -- tau_spec::add's type_def case is a no-op (see its
	// own comment, tau_spec.tmpl.h), so this loop has no functional effect
	// on the spec assembled here today; get_applied()'s actual cross-line
	// ADT visibility comes from upstream, via make_cli()'s
	// session_type_defs (see this file's def_type_cmd/make_cli), which
	// resolves an io/rr def's own ADT annotations at ITS declaration parse,
	// before it ever reaches type_defs/get_applied.
	// (->get(): type_defs/rr_defs/io_defs store htref, not tref -- see
	// their declaration comment in repl_evaluator.h for why.)
	for (const htref& hd : type_defs) spec.add(hd->get());
	for (const htref& hd : rr_defs) spec.add(hd->get());
	for (const htref& hd : io_defs) {
		tref d = hd->get();
		// A tuple-typed (ADT) io def's per-member registration and its
		// ctx->adt_streams grouping layout were already fully built when
		// it was first declared: adt_flatten_rewrite_io_def, called from
		// adt_flatten_rewrite's def_input_cmd/def_output_cmd case (see
		// src/adt/adt_flatten.tmpl.h), at the def's own original parse.
		// io_defs itself still holds that def's ORIGINAL, un-flattened
		// tree (its `typed: <ADT name>` annotation intact) so
		// def_input_cmd()/def_output_cmd() can echo it back to the user.
		// Splicing that raw tree back in here, on every later
		// normalize/sat/solve/run, re-runs infer_ba_types/update_types on
		// it with no ADT registry left to resolve `<ADT name>` -- which
		// used to fabricate a SECOND, un-grouped "bare root" stream
		// registration in ctx alongside the correct per-member one,
		// silently duplicating it. rebuild_inputs/rebuild_outputs
		// (interpreter.tmpl.h) would then also try to read/write through
		// that stray bare-root stream, producing spurious "Failed to
		// read/write ..." errors during `run`.
		//
		// This is still safe now that session type_defs became visible
		// cross-line (this file's own type_defs is now threaded through
		// make_cli()'s get_options into adt_flatten/adt_registry::build,
		// so an ADT type declared on an earlier REPL line is no longer
		// invisible to a later line's parse -- see the REPL test file's
		// header comment). That makes THIS def's own original
		// declaration parse resolve correctly even when its ADT type
		// came from an earlier line (adt_flatten_rewrite_io_def now sees
		// it too there), but it does not change what re-splicing here
		// would do. get_applied() only ever calls tau_spec::add(tref) on
		// spec (never tau_spec::parse(string)), so spec's parts_/parsed_
		// stay empty; get_nso_rr() -> get() (tau_spec.tmpl.h) DOES still
		// call tau::get(...) once, at :54-55, but on the ptree
		// build_parse_tree() returns for that empty parts_ -- an empty
		// `spec` ptree node with no children (build_parse_tree()'s own
		// defs.empty()/!main branch) -- so that one tau::get call parses
		// NONE of the user's actual text or spliced trees; nothing is
		// there yet for adt_flatten to see either way. The real spliced
		// content -- this def's raw tree (added via add()'s
		// input_def/output_def case into defs_) and arg's formula (via
		// add()'s wff/bf case into main_) -- is merged in AFTERWARDS by
		// plain tree-node constructors (tau::get(tau::main, main_),
		// tau::get(tau::definitions, spec_defs), tau::get(tau::spec, ...),
		// tau_spec.tmpl.h:63-72), which build a node from an existing
		// tref and never re-parse or re-flatten it. So splicing this raw,
		// un-flattened tree back in would still hit "no ADT registry left
		// to resolve `<ADT name>`" regardless of session visibility --
		// bypassed by construction (no parse ever touches it), not by
		// content; unlike an ordinary (non-ADT) cross-line io def --
		// which DOES still need this splice, since infer_ba_types (which
		// DOES run here, via get()'s own direct call, tau_spec.tmpl.h:76)
		// resolves its base type -- a tuple-typed def has nothing left to
		// contribute here, so it is skipped outright rather than spliced.
		tref head = tt(d) | tt::first | tt::ref;
		size_t root_sid = head ? tau::get(head).data() : 0;
		if (root_sid && defs.get_io_context()->adt_streams.contains(root_sid))
			continue;
		spec.add(d);
	}
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
	// children: [0] the command symbol, [1] the input expression, then one
	// subst_group per bracket group (issue #99), each holding two children
	// per comma separated match/replace pair. Groups compose sequentially
	// -- each is applied to the previous group's result -- while the pairs
	// inside a group are applied simultaneously
	const auto& t = n.value_tree();
	size_t sz = t.children_size();
	DBG(assert(sz >= 3);)

	measuring m;
	// Since the history command cannot be type-checked we do it here
	// First try to get bf
	tref in = get_bf(t.second(), true);
	bool bf_in = in != nullptr;
	// First argument was not a bf so it must be a wff
	if (!bf_in) in = get_wff(t.second());
	if (!in) return invalid_argument();

	// one simultaneous substitution step: all of a group's pairs, laid out
	// flat as match/replace successors in `pairs`, applied in a single
	// pass over `in`, so no pair's replacement is ever re-matched by
	// another pair of the same group
	auto step = [&](tref in, const trefs& pairs) -> tref {
		DBG(assert(pairs.size() >= 2 && pairs.size() % 2 == 0);)
		// infer_for_match hides inference failures, but whether the
		// input actually inferred is needed below: only then can a
		// failing result inference be attributed to the substitution
		tref in_inferred = tau_api::infer(in);
		bool in_typed = in_inferred != nullptr;
		if (in_typed) in = in_inferred;
		// structurally keyed so a re-parsed duplicate pattern is caught
		subtree_map<node, tref> changes;
		for (size_t i = 0; i + 1 < pairs.size(); i += 2) {
			tref thiz, with;
			if (bf_in) {
				// a bf input takes only bf pairs
				thiz = get_bf(pairs[i]),
				with = get_bf(pairs[i + 1]);
			} else {
				// a wff input takes a bf/bf or a wff/wff
				// pair, decided per pair
				thiz = get_bf(pairs[i], true);
				if (thiz) with = get_bf(pairs[i + 1]);
				else thiz = get_wff(pairs[i]),
					with = get_wff(pairs[i + 1]);
			}
			if (!thiz || !with) return invalid_argument();
			// only the match side needs the inferred,
			// type-annotated form (matching is type-id sensitive);
			// the replacement is left as parsed so an unannotated
			// replacement can adopt the matched context's type
			// during the result inference below -- inferring it
			// here would stamp the default type on it and
			// manufacture a conflict with any non-default context.
			// An input that could not be inferred at all still
			// carries its raw parsed nodes, so the pattern has to
			// stay raw as well or the two could never be
			// structurally equal
			tref raw = thiz;
			if (in_typed) thiz = infer_for_match(thiz);
			// strip bf of variables so we match also quantifiers
			if (is<node, tau::bf>(thiz)
				&& is_child<node, tau::variable>(thiz))
				thiz = tau::trim(thiz), with = tau::trim(with),
				raw = tau::trim(raw);
			auto add_change = [&](tref key, tref val) {
				if (!changes.emplace(key, val).second) {
					TAU_LOG_ERROR << "Duplicate match"
						" pattern in substitution\n";
					return false;
				}
				return true;
			};
			if (contains<node>(in, thiz)) {
				if (!add_change(thiz, with)) return nullptr;
				continue;
			}
			// The inferred pattern has no occurrence. A pattern
			// carrying no annotation at all is underspecified
			// rather than default-typed for matching purposes:
			// fall back to matching it with types erased, so it
			// also finds occurrences whose types were resolved by
			// inference (e.g. a variable an earlier substitution
			// or bracket group introduced into a non-default-typed
			// context). Annotated patterns stay strict.
			auto is_annotated = [](tref c) {
				return tau::get(c).is(tau::typed)
					|| tau::get(c).get_ba_type()
						!= untyped_type_id<node>();
			};
			trefs occs;
			if (!tau::get(raw).find_top(is_annotated)) {
				auto q = [&](tref el) {
					return equal_modulo_types(el, raw);
				};
				occs = rewriter::select_top<node>(in, q);
			}
			// a pattern that does not occur in the input can never
			// fire; say so instead of silently returning the input
			// unchanged. A warning, not an error: substituting
			// into an expression the pattern is absent from is
			// legitimate in history-driven flows
			if (occs.empty())
				TAU_LOG_WARNING << "Substitution pattern did"
					" not match anything in the input: "
					<< tau::get(thiz).to_str() << "\n";
			else for (tref occ : occs)
				if (!add_change(occ, with)) return nullptr;
		}
		tref r = tau_api::substitute(m, in,
			std::map<tref, tref>(changes.begin(), changes.end()));
		// Reject a result that no longer type-checks (e.g. an sbf
		// subterm replaced by a bv one, or mismatched bv widths)
		// instead of storing an ill-typed expression that every later
		// inference-running command would fail on. Untyped expressions
		// carry the default type and unannotated replacements are
		// resolved by inference against their context, so only
		// genuinely conflicting annotations are rejected. An input
		// that already failed inference is left to the old behavior.
		if (r && in_typed) {
			tref inferred = tau_api::infer(r);
			if (!inferred) {
				TAU_LOG_ERROR << "Substitution rejected: "
					"the result is not well-typed\n";
				return nullptr;
			}
			// keep the fully inferred result so the next group
			// and later type-id sensitive commands (further
			// subst, n, sat, ...) see resolved types
			r = inferred;
		}
		return r;
	};

	// each bracket group (subst_group, or inst_group when inst_cmd
	// delegates here) rewrites the previous group's result
	for (size_t g = 2; in && g < sz; ++g)
		in = step(in, tau::get(t.child(g)).get_children());
	return benchmarks(m), in;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::inst_cmd(const tt& n) {
	// DBG(TAU_LOG_TRACE << "inst_cmd" << LOG_FM_DUMP(n.value());)
	// children: [0] the command symbol, [1] the input expression, then
	// one inst_group per bracket group, shaped exactly like subst's
	// groups (issue #99). The only difference to subst is that every
	// pair's match side must be a variable, checked here before
	// delegating to subst_cmd on the node as parsed
	const auto& t = n.value_tree();
	for (size_t g = 2; g < t.children_size(); ++g) {
		const trefs pairs = tau::get(t.child(g)).get_children();
		for (size_t i = 0; i + 1 < pairs.size(); i += 2)
			if (!tau::get(pairs[i])[0].is(tau::variable)) {
				TAU_LOG_ERROR << "Invalid argument\n";
				return nullptr;
			}
	}
	return subst_cmd(n);
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::normalize_cmd(const tt& n) {
	tref arg = n[1].get();
	auto check = get_type_and_arg(arg);
	if (!check) return nullptr;
	auto [type, value] = check.value();
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
	if (auto value = get_any(n[1].get()); value)
		r = tau_api::eliminate_quantifiers(m, value);
	return benchmarks(m), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::run_cmd(const tt& n) {

	DBG(TAU_LOG_TRACE << "run_cmd: " << TAU_LOG_FM(n.value());)
	measuring m("run");

	tref value = get_any(n[1].get());
	if (!value) return;

	DBG(TAU_LOG_TRACE << "run_cmd/value: " << TAU_LOG_FM(value);)
	value = tau_api::simplify(m.part(), value);
	DBG(TAU_LOG_TRACE << "run_cmd/simplified: " << TAU_LOG_FM(value);)

	auto maybe_i = tau_api::get_interpreter(m.part(), value);
	if (!maybe_i) return;

	running = std::make_unique<run_session>(std::move(maybe_i.value()));
	running->t.start();
	continue_running();
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
		auto& step_m = running->m.part();
		auto maybe_outputs = tau_api::step(step_m, running->interp);
		// copy this step's timing before dropping the node (step_m is a
		// reference into m.parts that pop_back() invalidates)
		measuring step_m_copy = step_m;
		running->m.parts.pop_back();

		if (!maybe_outputs) {
			running->t.pause();
			// a console input stream stopped the step needing a value:
			// find it and prompt for that value (label/type are ours).
			// find_repl_pending_input sees through an ADT tuple member's
			// adt_member_input_stream/adt_tuple_reader (and this library's
			// own ownership-bridging physical-stream wrapper,
			// interpreter.tmpl.h) to the actual repl_pending_input_stream,
			// so a tuple-typed console input is found here exactly like a
			// plain one -- it's the SAME shared stream regardless of which
			// member var this loop iteration looks at (one physical stream
			// per tuple root, interpreter.tmpl.h's rebuild_inputs), so only
			// the first member reached here ever finds it still awaiting:
			// read_time_point's memo (io_context.tmpl.h) makes every other
			// member's leaf() reuse the SAME successful read once one
			// member consumes the pending value, instead of re-querying
			// the physical stream and re-flagging it awaiting.
			for (auto& [var, stream] : running->interp.inputs) {
				auto rp = find_repl_pending_input<node>(stream);
				if (!rp || !rp->awaiting()) continue;
				size_t tp = rp->awaiting_time_point();
				std::stringstream lbl;
				tref type_tree = nullptr;
				if (const adt_stream_layout<node>* layout =
					find_adt_stream_for_member<node>(
						running->interp.ctx, var); layout)
				{
					// One physical stream/prompt for the WHOLE tuple
					// literal (design doc sec. 4) -- label with the
					// stream's own root name, not this member's dotted
					// name, plus a wire-shaped hint of what to type.
					// type_tree stays null: a tuple literal isn't a single
					// BA type, so stream_value_incomplete (below) skips
					// its type-specific incomplete-value checks for it.
					lbl << dict(layout->root_name_sid) << "[" << tp
						<< "] := " << adt_wire_hint<node>(*layout) << " ";
				} else {
					size_t tid = running->interp.ctx.type_of(var);
					std::string type_name = get_ba_type_name<node>(tid);
					if (!type_name.empty() && type_name.front() == ':')
						type_name.erase(0, 1);
					lbl << get_var_name<node>(var) << "[" << tp << "] : "
						<< type_name << " := ";
					type_tree = get_ba_type_tree<node>(tid);
				}
				pending = { pending_request::stream_value, lbl.str(),
					rp, tp, type_tree };
				reprompt();
				return; // suspend: wait for the answer
			}
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
	auto is_unexpected_end = [](const std::string& msg) {
		return msg.find("Unexpected end of file") != std::string::npos;
	};
	// A tuple-typed (ADT) stream's prompt has no single BA type_tree
	// (continue_running leaves it null: see its own comment) -- a wire
	// literal isn't type-checked line-by-line the way a single BA
	// constant is, so there is no type-specific "still incomplete" check
	// to run for it; treat every line as complete (no multiline
	// continuation for a tuple literal -- type the whole thing on one
	// line). Also guards against dereferencing a null type_tree below.
	if (!type_tree) return false;
	if (is_sbf_type<node>(type_tree)) {
		auto result = sbf_parser::instance().parse(src.c_str(), src.size());
		return !result.found && is_unexpected_end(result.parse_error
			.to_str(sbf_parser::error::info_lvl::INFO_BASIC));
	}
	if (is_bv_type_family<node>(type_tree)) {
		auto result = bitvector_parser::instance()
						.parse(src.c_str(), src.size());
		return !result.found && is_unexpected_end(result.parse_error
			.to_str(bitvector_parser::error::info_lvl::INFO_BASIC));
	}
	// By REPL convention a '.'-terminated line is a completed tau value.
	// member_path makes "name." a valid prefix (of "name.member"), which
	// would otherwise reclassify bad values as incomplete and leave the
	// run silently waiting for more input.
	if (auto p = src.find_last_not_of(" \t\r\n");
		p != std::string::npos && src[p] == '.') return false;
	// tau_spec::parse() returns true on EOF-incomplete input, flags is_eof()
	tau_spec<node> s;
	s.parse(src);
	return s.is_eof();
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

// Reads the solver mode requested by a solve command tree: minimum or
// maximum when a solver_mode node is present, general otherwise.
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

// BA type id a solve command runs under: the first type annotation found
// in the command tree, or the default BA type's id when it has none.
template <NodeType node>
size_t get_solver_cmd_type(tref n) {
	size_t type = find_ba_type<node>(n);
	return type > 0 ? type
		: get_ba_type_id<node>(
			node::ba::default_type());
}

// Prints a solve command's result to stdout: "no solution" for nullopt,
// otherwise one `var := value` line per assignment. bf_t/bf_f values are
// rendered as the typed one/zero constant of the variable's own annotated
// type when it has one, falling back to type_id (the command's type).
template <NodeType node>
void print_solver_cmd_solution(std::optional<solution<node>>& solution,
		size_t type_id)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	// ba_types::name() renders the `typed` node, which already prints its
	// own leading ':', and BA constants are printed as `{ c }` elsewhere
	// (see the ba_constant case in tau_tree_printers), so match that form
	// here -- this branch never ran before, and printed `{c}::sbf`.
	// A variable carries its own type when it was annotated; prefer it
	// over the command's: with a cast in the system, `d:bv[24]` may be
	// solved inside a bv[48] formula, and printing its all-ones value as
	// the `1` of bv[48] showed 2^48-1 for what is 2^24-1.
	auto type_of = [&type_id](tref var) {
		size_t t = tau::get(var).get_ba_type();
		return t ? t : type_id;
	};
	auto print_constant_case_typed = [](tref var, const std::string& c, size_t t) {
		std::cout << "\t" << tau::get(var).to_str() << " := { " << c
			<< " }" << ba_types<node>::name(t) << "\n";
	};

	auto print_zero_case = [&](tref var) {
		size_t t = type_of(var);
		print_constant_case_typed(var,
			node::ba::zero(get_ba_type_tree<node>(t)), t);
	};

	auto print_one_case = [&](tref var) {
		size_t t = type_of(var);
		print_constant_case_typed(var,
			node::ba::one(get_ba_type_tree<node>(t)), t);
	};

	auto print_general_case = [](tref var, tref value) {
		std::cout << "\t" << tau::get(var).to_str() << " := "
			<< tau::get(value).to_str() << "\n";
	};

	if (!solution) { std::cout << "no solution\n"; return; }

	std::cout << "solution: {\n";
	for (auto [var, value]: solution.value()) {
		if (auto check = tt(value) | tau::bf_t; check)
			print_one_case(var);
		else if (check = tt(value) | tau::bf_f; check)
			print_zero_case(var);
		else
			print_general_case(var, value);
	}
	std::cout << "}\n";
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
	if (tref value = get_any(n[1].get()); value)
		r = tau_api::valid(m, value) ? tau::_T() : tau::_F();
	return benchmarks(m), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::sat_cmd(const tt& n) {
	measuring m;
	tref r = nullptr;
	if (tref value = get_any(n[1].get()); value)
		r = tau_api::sat(m, value) ? tau::_T() : tau::_F();
	return benchmarks(m), r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::unsat_cmd(const tt& n) {
	measuring m;
	tref r = nullptr;
	if (tref value = get_any(n[1].get()); value)
		r = tau_api::unsat(m, value) ? tau::_T() : tau::_F();
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
	io_defs.push_back(tau::geth(n | tt::first | tt::ref));
	size_t idx = io_defs.size() - 1;
	std::cout << "[" << idx + 1 << "] " << tau::get(io_defs[idx]->get()).to_str() << "\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::def_output_cmd(const tt& n) {
	io_defs.push_back(tau::geth(n | tt::first | tt::ref));
	size_t idx = io_defs.size() - 1;
	std::cout << "[" << idx + 1 << "] " << tau::get(io_defs[idx]->get()).to_str() << "\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::def_type_cmd(const tt& n) {
	tref def = n | tt::first | tt::ref;
	size_t name_sid = tt(def) | tau::type_name | tt::data;
	// A redeclaration (same type name) replaces the earlier entry rather
	// than piling up: session_type_defs (get_options, threaded into
	// adt_registry::build via adt_flatten) always wants THIS session's
	// latest definition for a name to reach later lines, and the vector
	// otherwise grows unboundedly across a long REPL session re-declaring
	// the same type. Declaration order of the surviving entries is not
	// otherwise preserved (the redeclaration moves to the back); build()'s
	// own last-def-wins-in-vector-order semantics only matter for a session
	// list with duplicates, which this keeps from ever happening.
	std::erase_if(type_defs, [&](const htref& hd) {
		return (tt(hd->get()) | tau::type_name | tt::data) == name_sid;
	});
	type_defs.push_back(tau::geth(def));
	size_t idx = type_defs.size() - 1;
	std::cout << "[" << idx + 1 << "] "
		<< tau::get(type_defs[idx]->get()).to_str() << "\n";
}

// make a nso_rr from the given tau source and binder.
template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::make_cli(const std::string& src) {
	// remove ascii char 22 if exists in the input
	std::string filt = src;
	filt.erase(remove_if(filt.begin(), filt.end(), [](unsigned char c) {
		return c == 22;
	}), filt.end());
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
			TAU_LOG_ERROR << "[repl] " << msg << "\n";
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
		.context = defs.get_io_context(),
		.session_type_defs = &type_defs
	};
	auto bound = tau::get(tau_parser::tree::get(t), opts);
	if (!bound) return fail();
	return bound;
}

// Maps an option name (short or long alias) to its repl_option. Empty
// input yields none_opt; an unrecognized name logs an error and yields
// invalid_opt, so callers can tell "no option given" from a typo.
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
	if (x == "maxclauses"
		|| x == "cqemaxclauses")     return cqe_max_clauses_opt;
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
	TAU_LOG_ERROR << "Invalid option: " << x << "\n";
	return invalid_opt;
}

// Reads the option_name child of a get/set command tree and resolves it
// via get_opt(string); none_opt when the command names no option.
template <NodeType node>
repl_option get_opt(const typename tree<node>::traverser& n) {
	auto o = n | tau_parser::option_name;
	if (!o) return none_opt;
	return get_opt(o | tree<node>::traverser::string);
}

// Parses a severity option value ("e"/"error", "d"/"debug", "t"/"trace",
// "i"/"info") into a boost severity level; anything else logs an error
// and yields nullopt.
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
	{ cqe_max_clauses_opt, [climit]() {
		std::cout << "maxclauses:          " << climit(cqe_max_clauses) << "\n"; } },
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
		std::cout << "revisionalts:        " << climit(interpreter<node>::max_revision_alts) << "\n"; } }
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
	{ cqe_max_clauses_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_cqe_max_clauses(*n); } },
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
		api<node>::set_max_revision_alts(*n); } } };
	setters[o]();
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::update_bool_opt_cmd(const tt& n,
	const std::function<bool(bool&)>& update_fn)
{
	auto o = get_opt<node>(n);
	update_bool_opt_cmd(o, update_fn);
	get_cmd(n);
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
	case cqe_max_clauses_opt:
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
	case tau::subst_cmd:          result = subst_cmd(command); break;
	case tau::inst_cmd:           result = inst_cmd(command); break;
	// formula checks
	case tau::sat_cmd:            result = sat_cmd(command); break;
	case tau::valid_cmd:          result = valid_cmd(command); break;
	case tau::unsat_cmd:          result = unsat_cmd(command); break;
	// normal forms
	case tau::onf_cmd:            result = onf_cmd(command); break;
	case tau::dnf_cmd:            result = dnf_cmd(command); break;
	case tau::cnf_cmd:            result = cnf_cmd(command); break;
//	Commented out because they are not implemented yet
//	case tau::anf_cmd:            not_implemented_yet(); break;
//	case tau::pnf_cmd:            not_implemented_yet(); break;
	case tau::nnf_cmd:            result = nnf_cmd(command); break;
	case tau::mnf_cmd:            result = mnf_cmd(command); break;
	// definition of rec relations to be included during normalization
	case tau::def_rr_cmd:         def_rr_cmd(command); break;
	case tau::def_list_cmd:       def_list_cmd(); break;
	case tau::def_print_cmd:      def_print_cmd(command); break;
	// definitions of i/o streams
	case tau::def_input_cmd:      def_input_cmd(command); break;
	case tau::def_output_cmd:     def_output_cmd(command); break;
	// definition of ADT types
	case tau::def_type_cmd:       def_type_cmd(command); break;
	// qelim
	case tau::qelim_cmd:          result = qelim_cmd(command); break;
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
	// Controls how fixpoint information in satisfiability.h should be printed
	if (!opt.repl_running) use_debug_output_in_sat = true;
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
idni::diagnostics::result<int> repl_evaluator<BAs...>::eval(
	const std::string& src)
{
	// while a `run` session is pending, src is its answer, not a new command
	if (pending) {
		// incomplete value: return 2 so more lines accumulate (multiline)
		if (!run_abort_ && pending->kind == pending_request::stream_value
			&& stream_value_incomplete(src, pending->type_tree))
					return idni::diagnostics::result<int>(2);
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
		return idni::diagnostics::result<int>(0);
	}
	error = false;
	auto tau_spec = tt(make_cli(src));
	int quit = 0;
	if (tau_spec) {
		auto commands = tau_spec || tau::cli_command;
		for (const auto& cmd : commands())
			if (quit = eval_cmd(cmd); quit == 1) break;
	} else if (!error) return idni::diagnostics::result<int>(2);
	std::cout << "\n", std::cout.flush();
	if (error && opt.error_quits)
		return idni::diagnostics::result<int>(quit = 1);
	if (quit == 0) reprompt();
	return idni::diagnostics::result<int>(quit);
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::version_cmd() {
	std::cout << full_version << "\n";
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
		"  charvar                use character variables              on/off\n"
		"  blasting               bitvector predicate blasting         on/off\n"
		"  highlighting           syntax highlighting of Tau formulas  on/off\n"
		"  indenting              indenting of Tau formulas            on/off\n"
		"  benchmarks             print timing benchmarks              on/off\n";
	static const std::string numeric_options =
		"and the numeric limit options, set with `set <option> <n>` "
		"(0 = unlimited;\nspecsizewarn: 0 = off; gcgrowth <= 0 disables "
		"gc; each mirrors the CLI option\nof the same meaning):\n"
		"  <option>               <description>                        <default>\n"
		"  maxsplits              anti-prenex per-block Boole splits   unlimited\n"
		"  maxrounds              anti-prenex driver rounds            unlimited\n"
		"  maxclauses             cqe DNF clauses per distributed scope unlimited\n"
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
		"  revisionalts           revision alternatives kept per part  unlimited\n";
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
		// << "  anf                     convert to algebraic normal form\n"
		<< "  nnf                     convert a Tau expression to negation normal form\n"
		// << "  pnf                     convert to prenex normal form\n"
		<< "  onf                     convert a Tau formula to order normal form\n"
		<< "\n"

		<< "History and definitions:\n"
		<< "  history or hist         show all Tau expressions stored in the repl history\n"
		<< "  definitions or defs     show stored IO variables and function and predicate definitions\n"
		<< "\n"

		<< "Substitution and instantiation command:\n"
		<< "  substitute, subst or s  substitute one or more Tau expressions in a Tau expression by others\n"
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
	case tau::version_sym: std::cout
		<< "version prints the current Tau version\n";
		break;
	case tau::quit_sym: std::cout
		<< "quit exits the Tau repl\n";
		break;
	case tau::clear_sym: std::cout
		<< "clear clears the terminal screen\n";
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
	case tau::run_sym: std::cout
		<< "the run command executes a Tau formula as a program\n"
		<< "\n"
		<< "run automatically chooses a single program from the set of programs satisfying a given Tau formula\n"
		<< "and executes it in such a way that in each step (starting at 0) each input variable reads a value from the specified stream\n"
		<< "and then a value is written into each specified output stream such that the Tau formula being executed is satisfied\n\n"
		<< "usage:\n"
		<< "  run <tau>               execute the given Tau formula\n"
		<< "  run <repl_history>      execute the Tau formula stored at the specified repl history position\n"
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
	// Commented out because they are not implemented yet
	//case tau::anf_sym: std::cout
	//	<< "anf command converts a boolean formula or a well formed formula to algebraic normal form\n"
	//	<< "\n"
	//	<< "usage:\n"
	//	<< "  anf <term>              converts the given term to ANF\n"
	//	<< "  anf <tau>               converts the given tau to ANF\n"
	//	<< "  anf <history>           converts the history with the given id to ANF\n";
	//	break;
	case tau::nnf_sym: std::cout
		<< "nnf converts a Tau expression to negation normal form (NNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  nnf <term>              converts the given term to NNF\n"
		<< "  nnf <tau>               converts the given tau formula to NNF\n"
		<< "  nnf <repl_history>      converts the Tau expression stored at the specified repl history position to NNF\n";
		break;
	// Commented out because they are not implemented yet
	//case tau::pnf_sym: std::cout
	//	<< "pnf command converts a boolean formula or a well formed formula to prenex normal form\n"
	//	<< "\n"
	//	<< "usage:\n"
	//	<< "  pnf <term>              converts the given term to PNF\n"
	//	<< "  pnf <tau>               converts the given term to PNF\n"
	//	<< "  pnf <history>           converts the history with the given id to PNF\n";
	//	break;
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
		<< "the substitute command substitutes one or more Tau expressions in a Tau expression by other Tau expressions\n"
		<< "\n"
		<< "usage:\n"
		<< "  substitute <input> '[' <match> / <replace> ']'\n"
		<< "  substitute <input> '[' <match> / <replace> , <match> / <replace> , ... ']'\n"
		<< "  substitute <input> '[' ... ']' '[' ... ']' ...\n"
		<< "\n"
		<< "where:\n"
		<< "  <input> is the Tau expression in which to replace\n"
		<< "  <match> is the Tau expression to be replaced in <input>\n"
		<< "  <replace> is the Tau expression that is replaced for <match>\n"
		<< "\n"
		<< "  Note that if <input> is of type term, <match> and <replace> must be of type term\n"
		<< "  In general <match> and <replace> must be of the same type, so either both term or tau\n"
		<< "\n"
		<< "  All pairs of a bracket group are applied simultaneously in a single pass over\n"
		<< "  its input: every <match> is found against the original expression and no pair's\n"
		<< "  <replace> is ever re-matched by another pair of the group, so\n"
		<< "  'substitute x & y [x / y, y / x]' swaps x and y\n"
		<< "  Repeating the same <match> in two pairs of one group is an error\n"
		<< "\n"
		<< "  Several bracket groups compose sequentially: each group is applied to the\n"
		<< "  previous group's result, so 'substitute a | c [a / b] [b / d]' chains a to d\n"
		<< "  while 'substitute a | c [a / b, b / d]' yields b | c\n"
		<< "\n"
		<< "  The result must remain well-typed: a <replace> whose type conflicts with the\n"
		<< "  matched context (a different base type, or a different bitvector width) is\n"
		<< "  rejected. An unannotated <replace> adopts the matched context's type\n"
		<< "  A <match> that does not occur in <input> is reported with a warning\n"
		<< "\n";
		break;
	case tau::inst_sym: std::cout
		<< "the instantiate command instantiates one or more variables in a Tau formula with the specified terms\n"
		<< "\n"
		<< "usage:\n"
		<< "  instantiate <input> '[' <variable> / <value> ']'\n"
		<< "  instantiate <input> '[' <variable> / <value> , <variable> / <value> , ... ']'\n"
		<< "  instantiate <input> '[' ... ']' '[' ... ']' ...\n"
		<< "\n"
		<< "where:\n"
		<< "  <input> is the Tau term to instantiate in\n"
		<< "  <variable> is the variable to be instantiated\n"
		<< "  <value> is the Tau term to instantiate with\n"
		<< "\n"
		<< "  Pairs and bracket groups behave exactly as in the substitute command: the pairs\n"
		<< "  of one group are applied simultaneously, several groups compose sequentially,\n"
		<< "  and the same type safety and no-match reporting apply (see 'help subst')\n"
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
		<< "  <def_type_cmd>          defines an ADT type\n"
		<< "  definitions             lists all definitions present in repl\n"
		<< "  definitions <number>    prints predicate or function at specified position\n"
		<< "\n"
		<< "Examples defining stream variables and functions can be found by typing \'help examples\'\n";
		break;
	case tau::examples_sym: std::cout
		<< "examples\n"
		<< "\n"
		<< "  # defining an input stream variable\n"
		<< "  i1 : sbf := in console\n"
		<< "  i2 : tau := in file(\"inputs.in\")\n"
		<< "\n"
		<< "  # defining an output stream variable\n"
		<< "  o1 : sbf := out console\n"
		<< "  o2 : tau := out file(\"outputs.out\")\n"
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
