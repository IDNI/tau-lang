// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "repl_evaluator.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "repl_evaluator"

namespace idni::tau_lang {

// 0 and SIZE_MAX both mean no cap.
inline std::string count_limit_str(size_t v) {
	return v == 0 || v == std::numeric_limits<size_t>::max()
		? "unlimited" : std::to_string(v);
}

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
		if (!silent) out << "history is empty\n";
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
			out << "history location " << TC_OUTPUT
				<< (is_relative ? "%-" : "%");
			if (!is_relative || idx) out << idx;
			out << TC.CLEAR() << " does not exist\n";
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
	out << TC_OUTPUT << "%" << id + 1 << TC.CLEAR();
	if (print_relative_index) {
		out << "/" << TC_OUTPUT;
		if (size - id == 1) out << "%";
		else out << "%-" << size - id - 1;
		out << TC.CLEAR();
	}
	out << ": ";
	tau_api::print(out, mem) << "\n";
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
	if (H.size() == 0) out << "history is empty\n";
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
	auto inferred = tau_api::infer(n);
	return inferred.has_value() ? inferred.value() : n;
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
	auto applied_r = nso_rr_apply(maybe_nso_rr.value());
	tref applied = applied_r.has_value() ? applied_r.value() : nullptr;
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
template <typename T>
void repl_evaluator<BAs...>::print_benchmarks(const result<T>& res) const {
	if (opt.print_benchmarks) print_benchmarks(res.report());
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::print_benchmarks(const report& rep) const {
	if (!opt.print_benchmarks) return;
	rep.print(err);
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::onf_cmd(const tt& n) {
	// grammar: "onf" __ variable __ onf_cmd_arg -- n[1] is the variable
	// itself (must not go through get_any/apply_all_defs, which expect a
	// formula/history argument), n[2] is the formula.
	tref var = n[1].get();
	tref r = nullptr;
	if (auto value = get_any(n[2].get()); value) {
		auto res = tau_api::onf(value, var);
		print_benchmarks(res);
		if (!res.has_value()) { res.print(err); return nullptr; }
		r = res.value();
	}
	return r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::dnf_cmd(const tt& n) {
	tref r = nullptr;
	if (auto value = get_any(n[1].get()); value) {
		auto res = tau_api::dnf(value);
		print_benchmarks(res);
		if (!res.has_value()) { res.print(err); return nullptr; }
		r = res.value();
	}
	return r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::cnf_cmd(const tt& n) {
	tref r = nullptr;
	if (auto value = get_any(n[1].get()); value) {
		auto res = tau_api::cnf(value);
		print_benchmarks(res);
		if (!res.has_value()) { res.print(err); return nullptr; }
		r = res.value();
	}
	return r;
}


template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::nnf_cmd(const tt& n) {
	tref r = nullptr;
	if (auto value = get_any(n[1].get()); value) {
		auto res = tau_api::nnf(value);
		print_benchmarks(res);
		if (!res.has_value()) { res.print(err); return nullptr; }
		r = res.value();
	}
	return r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::anf_cmd(const tt& n) {
	tref r = nullptr;
	if (auto value = get_any(n[1].get()); value) {
		constexpr size_t bf_type = 0;
		r = anf<node, bf_type>(value);
	}
	return r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::pnf_cmd(const tt& n) {
	tref r = nullptr;
	if (auto value = get_any(n[1].get()); value) {
		auto res = tau_api::pnf(value);
		print_benchmarks(res);
		if (!res.has_value()) { res.print(err); return nullptr; }
		r = res.value();
	}
	return r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::mnf_cmd(const tt& n) {
	tref r = nullptr;
	if (auto value = get_any(n[1].get()); value) {
		auto res = tau_api::mnf(value);
		print_benchmarks(res);
		if (!res.has_value()) { res.print(err); return nullptr; }
		r = res.value();
	}
	return r;
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
		auto in_inferred_r = tau_api::infer(in);
		tref in_inferred = in_inferred_r.has_value()
			? in_inferred_r.value() : nullptr;
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
		auto r_res = tau_api::substitute(in,
			std::map<tref, tref>(changes.begin(), changes.end()));
		print_benchmarks(r_res);
		tref r = r_res.has_value() ? r_res.value() : nullptr;
		// Reject a result that no longer type-checks (e.g. an sbf
		// subterm replaced by a bv one, or mismatched bv widths)
		// instead of storing an ill-typed expression that every later
		// inference-running command would fail on. Untyped expressions
		// carry the default type and unannotated replacements are
		// resolved by inference against their context, so only
		// genuinely conflicting annotations are rejected. An input
		// that already failed inference is left to the old behavior.
		if (r && in_typed) {
			auto inferred_r = tau_api::infer(r);
			tref inferred = inferred_r.has_value()
				? inferred_r.value() : nullptr;
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
	return in;
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
	if (reject_ctl_star_if_disabled(value)) return nullptr;
	tref r = nullptr;
	switch (type) {
	case tau::wff: {
		auto res = tau_api::normalize_formula(value);
		print_benchmarks(res);
		if (!res.has_value()) { res.print(err); return nullptr; }
		r = res.value();
		break;
	}
	case tau::bf: {
		auto res = tau_api::normalize_term(value);
		print_benchmarks(res);
		if (!res.has_value()) { res.print(err); return nullptr; }
		r = res.value();
		break;
	}
	default: return nullptr;
	}
	return r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::qelim_cmd(const tt& n) {
	tref r = nullptr;
	if (auto value = get_any(n[1].get());
		value && !reject_ctl_star_if_disabled(value))
	{
		auto res = tau_api::eliminate_quantifiers(value);
		print_benchmarks(res);
		if (!res.has_value()) { res.print(err); return nullptr; }
		r = res.value();
	}
	return r;
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::reset_cmd() {
	// The run goes first: its interpreter holds raw trefs the sweep in
	// api::reset cannot see.
	const bool was_running = (bool)running;
	finish_running();
	H.clear();
	rr_defs.clear();
	io_defs.clear();
	type_defs.clear();
	names = {};
	const size_t freed = api<node>::reset();
	out << "Session reset: " << (was_running ? "run stopped, " : "")
		<< "history, definitions, IO streams and caches cleared, "
		<< freed << " tree nodes freed (live: "
		<< api<node>::tref_count() << ").\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::whatis_cmd(const tt& n) {
	tref arg = n[1].get();
	auto check = get_type_and_arg(arg);
	if (!check) return nullptr;
	auto [type, value] = check.value();
	const std::string& node_type = tau::get(value).get_type_name();
	out << "node type: " << node_type;
	if (type == tau::bf || type == tau::wff) {
		size_t ba_type = tau::get(value).get_ba_type();
		if (ba_type) {
			auto nm = ba_types<node>::name(ba_type);
			if (nm.has_value())
				out << "  BA type: " << nm.value();
			else if (nm.has_error()) {
				// end the partial "node type: ..." line before the report.
				out << "\n";
				nm.report().print(out);
			}
		}
	}
	out << "\n";
	return value;
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::run_cmd(const tt& n) {

	DBG(TAU_LOG_TRACE << "run_cmd: " << TAU_LOG_FM(n.value());)

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

		report setup_rep;
		result<interpreter<node>> gi;
		{
			auto s = setup_rep.open("setup");
			gi = tau_api::get_interpreter(spec);
		}
		if (!gi.has_value()) {
			// Surface the setup timing alongside the failure it led to.
			gi.append(std::move(setup_rep));
			gi.print(err);
			return;
		}

		// A new formula replaces any stored session.
		running = std::make_unique<run_session>(std::move(gi).value());
		running->rep.append(std::move(setup_rep));
		running->steps_done   = 0;
		running->steps_to_run = steps; // 0 = natural
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
	continue_running();
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::stop_cmd() {
	if (!running) { out << "no run in progress\n"; return; }
	finish_running();
	out << "run stopped\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::ltl_cmd(const tt& n) {
	DBG(TAU_LOG_TRACE << "ltl_cmd: " << TAU_LOG_FM(n.value());)

	tref value = get_any(n[1].get());
	if (!value) return;
	// IN-N5: `ltl` was the one formula command with no fragment gate.
	if (reject_ctl_star_if_disabled(value)) return;

	DBG(TAU_LOG_TRACE << "ltl_cmd/value: " << TAU_LOG_FM(value);)

	report rep;
	{
		auto s = rep.open_if(opt.print_benchmarks, "ltl");
		// IN-R4: the synthesis backend reports "no verdict" as a
		// result<T> error, not an exception; nothing above this frame
		// would catch a throw, so a slow or missing ltlsynt (or a
		// refused CTL* placement) used to terminate the REPL. Print the
		// whole report -- UNKNOWN summary plus the refusal detail --
		// exactly once here instead.
		auto explain_r = ltl_explain<node>(value, out);
		if (!explain_r.has_value()) {
			explain_r.print(err);
			error = true;
		}
	}
	print_benchmarks(rep);
}

// Drives a `run` session's step loop, suspending via `pending` (instead of
// blocking) when it needs input; the next eval() call resumes it.
template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::continue_running(
	std::optional<pending_request> retry)
{
	if (!running) return;
	// Times exactly this invocation's synchronous work: continue_running()
	// never blocks for input, it suspends via `pending` and returns, so
	// this scope never spans the interactive wait between invocations.
	auto s = running->rep.open("run");
	bool first = true;
	while (running) {
		// At budget: stop cleanly but KEEP the session for a later `run`.
		if (running->steps_to_run != 0
			&& running->steps_done >= running->steps_to_run) {
			return;
		}
		// time_point advances iff a step produced output; api::step reports
		// an error both when it needs input and when auto_continue is false.
		const size_t tp_before = running->interp.time_point;
		// IN-R4: a mid-run pointwise-revision update can reach ltlsynt
		// (through update() -> pointwise_revision); a backend failure
		// there is a result<T> error on `st`, ended below by the same
		// genuine-step-failure path as any other step error.
		result<std::map<stream_at, std::string>> st;
		st = tau_api::step(running->interp);

		if (!st.has_value()) {
			const bool produced =
				running->interp.time_point != tp_before;
			// Input-independent step: output already written.
			// "continue?" prompt, mirroring C++ run(fm, ctx, N).
			if (produced && running->steps_to_run != 0) {
				++running->steps_done;
				if (opt.print_benchmarks) {
					st.report().print(out);
				}
				out << "\n";
				first = false;
				continue;
			}
			if (!step_awaiting_input(st.report())) {
				// A genuine step failure, not a wait for input
				// (including a synthesis backend failure mid-run):
				// report it and end the run.
				st.print(err);
				s.close();
				running.reset();
				error = true;
				return;
			}
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
					// literal -- label with the stream's own root name,
					// not this member's dotted name, plus a wire-shaped
					// hint of what to type.
					// type_tree stays null: a tuple literal isn't a single
					// BA type, so stream_value_incomplete (below) skips
					// its type-specific incomplete-value checks for it.
					lbl << dict(layout->root_name_sid) << "[" << tp
						<< "] := " << adt_wire_hint<node>(*layout) << " ";
				} else {
					size_t tid = running->interp.ctx.type_of(var);
					auto tn = get_ba_type_name<node>(tid);
					std::string type_name;
					if (tn.has_value())
						type_name = tn.value();
					else if (tn.has_error()) {
						tn.report().print(out);
					}
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
		// Ended before budget (e.g. input EOF): stop, do not prompt.
			if (running->steps_to_run != 0) return;
			// no awaiting stream -> rejected value (re-ask) or end of run
			if (first && retry) {
				st.print(out);
				pending = *retry;
			} else pending = { pending_request::continue_or_quit,
				"continue? [Enter]/[q]: ", nullptr };
			reprompt();
			return; // suspend: wait for the answer
		}
		// this step produced output: print its timing right after (to
		// cout, so it stays in order and belongs to the step it measures,
		// before the next "Execution step"), then a blank line
		++running->steps_done;
		if (opt.print_benchmarks) {
			st.report().print(out);
		}
		out << "\n";
		first = false;
	}
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::finish_running() {
	if (running) print_benchmarks(running->rep);
	running.reset();
	pending.reset();
}

template <typename... BAs>
requires BAsPack<BAs...>
bool repl_evaluator<BAs...>::stream_value_incomplete(
	const std::string& src, tref type_tree) const
{
	// The BA owning the type answers for its own literals. Also covers a
	// tuple-typed (ADT) stream's prompt, whose type_tree is null
	// (continue_running leaves it null: see its own comment) -- a wire
	// literal isn't type-checked line-by-line, so it is always complete.
	if (auto r = pack_literal_incomplete<node>(type_tree, src))
		return *r;
	// Only a tau value is a spec; tau_spec::parse() returns true on
	// EOF-incomplete input and flags is_eof().
	if (type_tree && is_tau_type<node>(type_tree)) {
		// By REPL convention a '.'-terminated line is a completed tau
		// value. member_path makes "name." a valid prefix (of
		// "name.member"), which would otherwise reclassify bad values
		// as incomplete and leave the run silently waiting for more
		// input.
		if (auto p = src.find_last_not_of(" \t\r\n");
			p != std::string::npos && src[p] == '.') return false;
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
// Prints a solve command's result to @p out: "no solution" for nullopt,
// otherwise one `var := value` line per assignment. bf_t/bf_f values are
// rendered as the typed one/zero constant of the variable's own annotated
// type when it has one, falling back to type_id (the command's type).
template <NodeType node>
void print_solver_cmd_solution(std::ostream& out,
		std::optional<solution<node>>& solution, size_t type_id)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	// bf_t/bf_f carry no BA type; serialize_constant renders them as the
	// type's one/zero (see the ba_constant case in tau_tree_printers). This
	// also covers the general ba_constant case, unlike a narrower dedicated
	// bf_t/bf_f branch; type_id is the fallback for untyped variables.
	if (!solution) { out << "no solution\n"; return; }

	out << "solution: {\n";
	for (auto [var, value]: solution.value()) {
		if (tt(value) | tau::ba_constant) {
			print_binding<node>(out, var, value);
			continue;
		}
		size_t t = find_ba_type<node>(var);
		if (t == 0) t = type_id;
		std::stringstream ss;
		auto ser = serialize_constant<node>(ss, value, t);
		if (ser.has_error()) {
			ser.report().print(out);
		}
		if (!ser.has_value() || !ser.value())
			print_binding<node>(out, var, value);
		else {
			auto nm = ba_types<node>::name(t);
			if (nm.has_value())
				out << "\t" << tau::get(var).to_str() << " := { "
					<< ss.str() << " }" << nm.value() << "\n";
			else if (nm.has_error()) {
				nm.report().print(out);
			}
		}
	}
	out << "}\n";
}

// Prints the interpreter memory plus a binding count. Uses the type-agnostic
// form because memory entries span BA types and not all have a resolvable one.
template <NodeType node>
void print_memory(std::ostream& out, const assignment<node>& memory) {
	using tau = tree<node>;
	using tt = typename tau::traverser;
	if (memory.empty()) {
		out << "memory: {}\n0 bindings\n";
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
	out << "memory: {\n";
	for (tref var : io_keys)    print_binding<node>(out, var, memory.at(var));
	for (tref var : other_keys) print_binding<node>(out, var, memory.at(var));
	out << "}\n" << memory.size() << " binding"
		<< (memory.size() == 1 ? "" : "s") << "\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::memory_cmd() {
	if (!running) {
		out << "no run in progress; memory is only tracked "
			"during an active `run` session\n";
		return;
	}
	print_memory<node>(out, running->interp.memory);
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
	auto res = tau_api::solve(value, get_solver_cmd_mode<node>(n.value()));
	print_benchmarks(res);
	if (!res.has_value()) {
		if (report_has_code(res.report(), code::unsat))
			out << "no solution\n";
		else
			res.print(err);
		return;
	}

	// the printer needs the BA type of the solution, not the grammar
	// nonterminal of the argument that get_type_and_arg also returns
	std::optional<solution<node>> solution = std::move(res).value();
	print_solver_cmd_solution<node>(out, solution,
		find_ba_type_or_default<node>(value));
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::lgrs_cmd(const tt& n) {
	tref arg = n.value_tree().first();
	while (tau::get(arg).has_right_sibling())
		arg = tau::get(arg).right_sibling();
	tref value = get_any(arg);
	if (!value) return;
	auto res = tau_api::lgrs(value);
	print_benchmarks(res);
	if (!res.has_value()) {
		if (report_has_code(res.report(), code::unsat))
			out << "no solution\n";
		else
			res.print(err);
		return;
	}
	// trefs vars = tau::get(equations).select_top(is_child<node, tau::variable>);
	// same as solve_cmd: the printer takes a BA type id, not the grammar
	// nonterminal that get_type_and_arg also returns
	std::optional<solution<node>> solution = std::move(res).value();
	print_solver_cmd_solution<node>(out, solution,
		find_ba_type_or_default<node>(value));
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::valid_cmd(const tt& n) {
	tref r = nullptr;
	if (tref value = get_any(n[1].get());
		value && !reject_ctl_star_if_disabled(value))
	{
		auto res = tau_api::valid(value);
		print_benchmarks(res);
		if (!res.has_value()) { res.print(err); return nullptr; }
		r = res.value() ? tau::_T() : tau::_F();
	}
	return r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::sat_cmd(const tt& n) {
	tref r = nullptr;
	if (tref value = get_any(n[1].get());
		value && !reject_ctl_star_if_disabled(value))
	{
		auto res = tau_api::sat(value);
		print_benchmarks(res);
		if (!res.has_value()) { res.print(err); return nullptr; }
		r = res.value() ? tau::_T() : tau::_F();
	}
	return r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::unsat_cmd(const tt& n) {
	tref r = nullptr;
	if (tref value = get_any(n[1].get());
		value && !reject_ctl_star_if_disabled(value))
	{
		auto res = tau_api::unsat(value);
		print_benchmarks(res);
		if (!res.has_value()) { res.print(err); return nullptr; }
		r = res.value() ? tau::_T() : tau::_F();
	}
	return r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::realizable_cmd(const tt& n) {
	tref r = nullptr;
	if (tref value = get_any(n[1].get()); value) {
		auto res = tau_api::realizable(value);
		print_benchmarks(res);
		if (!res.has_value()) { res.print(err); return nullptr; }
		r = res.value() ? tau::_T() : tau::_F();
	}
	return r;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref repl_evaluator<BAs...>::unrealizable_cmd(const tt& n) {
	tref r = nullptr;
	if (tref value = get_any(n[1].get()); value) {
		auto res = tau_api::unrealizable(value);
		print_benchmarks(res);
		if (!res.has_value()) { res.print(err); return nullptr; }
		r = res.value() ? tau::_T() : tau::_F();
	}
	return r;
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
	out << "[" << idx + 1 << "] " << tau::get(rr_defs[idx]->get()).to_str() << "\n";
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
	if (rr_defs.empty()) out << "Definitions: empty\n";
	else out << "Definitions:\n";
	for (size_t i = 0; i < rr_defs.size(); i++)
		out << "    [" << i + 1 << "] "
			<< tau::get(rr_defs[i]->get()).to_str() << "\n";
	if (io_defs.empty()) out << "Streams: empty\n";
	else out << "Streams:\n";
	for (size_t i = 0; i < io_defs.size(); i++)
		out << "    [" << i + 1 << "] "
			<< tau::get(io_defs[i]->get()).to_str() << "\n";
	out << *defs.get_io_context();
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::def_print_cmd(const tt& command) {
	auto num = command | tau::num;
	if (!num) return;
	auto i = num | tt::num;
	if (i && i <= rr_defs.size()) {
		out << tau::get(rr_defs[i-1]->get()).to_str() << "\n";
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
	out << "[" << idx + 1 << "] " << tau::get(io_defs[idx]->get()).to_str() << "\n";
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
	out << "[" << idx + 1 << "] " << tau::get(io_defs[idx]->get()).to_str() << "\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::def_type_cmd(const tt& n) {
	tref def = n | tt::first | tt::ref;
	size_t name_sid = tt(def) | tau::new_type_name | tt::data;
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
		return (tt(hd->get()) | tau::new_type_name | tt::data) == name_sid;
	});
	type_defs.push_back(tau::geth(def));
	size_t idx = type_defs.size() - 1;
	out << "[" << idx + 1 << "] "
		<< tau::get(type_defs[idx]->get()).to_str() << "\n";
}

// make a nso_rr from the given tau source and binder.
template <typename... BAs>
requires BAsPack<BAs...>
result<tref> repl_evaluator<BAs...>::make_cli(const std::string& src) {
	result<tref> r;
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
		.parse(filt.c_str(), filt.size(),
			{ .start = tau::cli, .dynamic_ctx = &names });
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(tau_parser::error::info_lvl::INFO_BASIC);
		if (opt.error_quits
			|| msg.find("Syntax Error: Unexpected end")!=0)
		{
			std::string hint = classify_parse_error<node>(filt);
			TAU_LOG_ERROR << "[repl] " << msg
				<< (hint.empty() ? "" : "\nhint: " + hint) << "\n";
			error = true;
			// The parser's own diagnostic is already shown above; r
			// only needs to stay well-formed for its caller.
			r.error(code::parse_error, messages::failed_to_parse_spec);
			return r;
		}
		return r.with_value(nullptr); // Unexpected eof, continue with reading input
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
	tref bound = r.merge_take(tau::get(tau_parser::tree::get(t), opts))
		.value_or(nullptr);
	if (!bound) {
		error = true;
		if (!r.has_error())
			r.error(code::parse_error, messages::failed_to_parse_spec);
		// eval()'s own result<int> return carries REPL quit codes, not
		// this report, so print it here where it still has a value.
		r.print(err);
		return r;
	}
	return r.with_value(bound);
}

/**
 * @brief Split a qualified BA-option name `family-option` on its first '-'.
 *
 * Called only once the caller has already confirmed @p x contains a '-', so
 * `substr(pos + 1)` is always in range.
 */
inline std::pair<std::string, std::string> split_ba_option_name(
	const std::string& x)
{
	auto pos = x.find('-');
	return { x.substr(0, pos), x.substr(pos + 1) };
}

/**
 * @brief Parse @p v as a plain non-negative integer, or nullopt on failure.
 *
 * Same digits-only rule as set_cmd's local str2count lambda, kept as a free
 * function here so the BA-option path (which set_cmd's lambda is not in
 * scope for) can share it without duplicating the parsing logic.
 */
inline std::optional<size_t> ba_option_str2count(const std::string& v) {
	if (v.empty()) return std::nullopt;
	size_t n = 0;
	for (char c : v) {
		if (c < '0' || c > '9') return std::nullopt;
		n = n * 10 + static_cast<size_t>(c - '0');
	}
	return n;
}

/**
 * @brief Parse @p v as an on/off flag value, or nullopt on failure.
 *
 * Same accepted spellings as set_cmd's local update_bool_value lambda.
 */
inline std::optional<bool> ba_option_str2bool(const std::string& v) {
	if (v == "t" || v == "true" || v == "on" || v == "1"
		|| v == "y" || v == "yes") return true;
	if (v == "f" || v == "false" || v == "off" || v == "0"
		|| v == "n" || v == "no") return false;
	return std::nullopt;
}

/**
 * @brief Raw text of @p n's option_name node, or nullopt when @p n names no
 * option at all (the bare `get`/`enable`/... form).
 *
 * Split out from @ref get_opt so a qualified name (`bv-blasting`) can be
 * recognised and routed to the BA-option path before get_opt's plain-name
 * matching ever sees it. get_opt itself is unchanged and still owns
 * resolution of every core option name.
 */
template <NodeType node>
std::optional<std::string> option_name_str(
	const typename tree<node>::traverser& n)
{
	auto o = n | tau_parser::option_name;
	if (!o) return std::nullopt;
	return o | tree<node>::traverser::string;
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
	if (x == "B" || x == "preprocessing") return preprocessing_opt;
	if (x == "factoring"
		|| x == "bacomponentfactoring") return factoring_opt;
	if (x == "pwrsemantic"
		|| x == "Z")                 return pwr_semantic_opt;
	if (x == "stepprop"
		|| x == "stepdefinitionalpropagation") return step_prop_opt;
	if (x == "H" || x == "highlighting"
		|| x == "highlight")         return highlighting_opt;
	if (x == "I" || x == "indenting"
		|| x == "indent")            return indenting_opt;
	// "B" belongs to preprocessing above; benchmarks takes lowercase "b".
	if (x == "b" || x == "benchmarks"
		|| x == "benchmarking")      return print_benchmarks_opt;
	if (x == "d" || x == "debug"
		|| x == "dbg")               return debug_opt;
	// Full names only: every single letter that would fit is taken (see
	// the note above). These two are numeric options, not flags.
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
	if (x == "lgrsmaxvars")              return lgrs_max_vars_opt;
	if (x == "decisionpins"
		|| x == "badecisionpins")    return decision_pins_opt;
	if (x == "fixpointsteps"
		|| x == "maxfixpointsteps")  return fixpoint_steps_opt;
	if (x == "flagsteps"
		|| x == "maxflagsearchsteps") return flag_search_steps_opt;
	if (x == "squeezecap"
		|| x == "blocksqueezecap")   return squeeze_cap_opt;
	if (x == "simplifyrounds"
		|| x == "maxsimplifyrounds") return simplify_rounds_opt;
	if (x == "defpasses"
		|| x == "maxdefpasses")      return def_passes_opt;
	if (x == "enumsteps"
		|| x == "maxenumsteps")      return enum_steps_opt;
	if (x == "probesteps"
		|| x == "maxprobesteps")     return probe_steps_opt;
	if (x == "rewriterounds"
		|| x == "maxrewriterounds")  return rewrite_rounds_opt;
	if (x == "trefbudget")               return tref_budget_opt;
	if (x == "trefbudgetsoft")           return tref_budget_soft_opt;
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
	if (x == "ltltimeout")               return ltl_timeout_opt;
	if (x == "ltlalg")                   return ltl_alg_opt;
	if (x == "ltlqemaxvars")             return ltl_qe_max_vars_opt;
	if (x == "ltlhoamaxstates")          return ltl_hoa_max_states_opt;
	if (x == "ltlguardmaxcubes")         return ltl_guard_max_cubes_opt;
	if (x == "ltlrefinementrounds")      return ltl_refinement_rounds_opt;
	if (x == "ltlwindowmaxpaths")        return ltl_window_max_paths_opt;
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
	if (auto raw = option_name_str<node>(n);
		raw && raw->find('-') != std::string::npos)
			return get_cmd_ba_option(*raw);
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
		out << "debug-repl:          " << pbool[opt.debug_repl] << "\n"; } },
#endif // DEBUG
	{ status_opt,       [this]() {
		out << "status:              " << pbool[opt.status] << "\n"; } },
	{ colors_opt,       [this]() {
		out << "colors:              " << pbool[opt.colors] << "\n"; } },
	{ charvar_opt,      [this]() {
		out << "charvar:             " << pbool[opt.charvar] << "\n"; } },
	{ preprocessing_opt, [this]() {
		out << "preprocessing:       " << pbool[opt.preprocessing] << "\n"; } },
	{ pwr_semantic_opt,  [this]() {
		out << "pwrsemantic:         " << pbool[pwr_semantic_fallback] << "\n"; } },
	{ step_prop_opt,     [this]() {
		out << "stepprop:            "
			<< pbool[interpreter<node>::definitional_propagation] << "\n"; } },
	{ factoring_opt,     [this]() {
		out << "factoring:           " << pbool[opt.factoring] << "\n"; } },
	{ highlighting_opt, [this]() {
		out << "syntax highlighting: " << pbool[pretty_printer_highlighting] << "\n"; } },
	{ indenting_opt,    [this]() {
		out << "indenting:           " << pbool[pretty_printer_indenting] << "\n"; } },
	{ severity_opt,     [this]() {
		out << "severity:            " << opt.severity << "\n"; } },
	{ print_benchmarks_opt, [this]() {
		out << "benchmarks:          " << pbool[opt.print_benchmarks] << "\n"; } }
	};
	// Read from the library globals, not from `opt`: they are what the
	// algorithm actually consults, and a caller using the api setters
	// directly would otherwise be misreported here. Both "unlimited"
	// representations print alike: 0 for the caps and SIZE_MAX for the
	// two decrementing block budgets.
	auto climit = count_limit_str;
	std::map<repl_option, std::function<void()>> limit_printers = {
	{ block_max_splits_opt, [climit, this]() {
		out << "maxsplits:           " << climit(block_boole_max_splits) << "\n"; } },
	{ block_max_rounds_opt, [climit, this]() {
		out << "maxrounds:           " << climit(block_max_rounds) << "\n"; } },
	{ decision_pins_opt, [this]() { // a raw count: 0 means none, not unlimited
		out << "decisionpins:        " << ba_decision_pins << "\n"; } },
	{ cqe_max_clauses_opt, [climit, this]() {
		out << "maxclauses:          " << climit(cqe_max_clauses) << "\n"; } },
	{ lgrs_max_vars_opt, [climit, this]() {
		out << "lgrsmaxvars:         " << climit(lgrs_max_vars) << "\n"; } },
	{ fixpoint_steps_opt, [climit, this]() {
		out << "fixpointsteps:       " << climit(max_fixpoint_steps) << "\n"; } },
	{ flag_search_steps_opt, [climit, this]() {
		out << "flagsteps:           " << climit(max_flag_search_steps) << "\n"; } },
	{ squeeze_cap_opt, [climit, this]() {
		out << "squeezecap:          " << climit(block_squeeze_cap) << "\n"; } },
	{ simplify_rounds_opt, [climit, this]() {
		out << "simplifyrounds:      " << climit(max_simplify_rounds) << "\n"; } },
	{ def_passes_opt, [climit, this]() {
		out << "defpasses:           " << climit(max_def_passes) << "\n"; } },
	{ probe_steps_opt, [climit, this]() {
		out << "probesteps:          " << climit(max_probe_steps) << "\n"; } },
	{ enum_steps_opt, [climit, this]() {
		out << "enumsteps:           " << climit(max_enum_steps) << "\n"; } },
	{ rewrite_rounds_opt, [climit, this]() {
		out << "rewriterounds:       " << climit(max_rewrite_rounds) << "\n"; } },
	{ gc_min_size_opt, [this]() {
		out << "gcminsize:           " << interpreter<node>::gc_min_size << "\n"; } },
	{ gc_growth_opt, [this]() {
		out << "gcgrowth:            " << interpreter<node>::gc_growth_factor << "\n"; } },
	{ spec_size_warn_opt, [this]() {
		const size_t v = interpreter<node>::spec_size_warn_threshold;
		out << "specsizewarn:        "
			<< (v ? std::to_string(v) : "off") << "\n"; } },
	{ revision_alts_opt, [climit, this]() {
		out << "revisionalts:        " << climit(interpreter<node>::max_revision_alts) << "\n"; } },
	{ consistency_subsets_opt, [climit, this]() {
		out << "maxsubsets:          " << climit(max_consistency_subsets) << "\n"; } },
	{ cache_bound_opt, [climit, this]() {
		out << "cachebound:          " << climit(cache_bound) << "\n"; } },
	{ cover_products_opt, [climit, this]() {
		out << "maxcoverproducts:    " << climit(max_cover_products) << "\n"; } },
	// Effective values, so the environment fallbacks show through when the
	// parameter itself is unset.
	{ ltl_timeout_opt, [this]() {
		const int t = ltl_timeout_sec();
		out << "ltltimeout:          "
			<< (t ? std::to_string(t) + "s" : "off") << "\n"; } },
	{ ltl_alg_opt, [this]() {
		const std::string a = ltl_algorithm_choice();
		out << "ltlalg:              " << (a.empty() ? "auto" : a) << "\n"; } },
	{ ltl_qe_max_vars_opt, [this]() {
		out << "ltlqemaxvars:        " << ltl_qe_max_vars() << "\n"; } },
	{ ltl_hoa_max_states_opt, [climit, this]() {
		out << "ltlhoamaxstates:     " << climit(ltl_hoa_max_states()) << "\n"; } },
	{ ltl_guard_max_cubes_opt, [climit, this]() {
		out << "ltlguardmaxcubes:    " << climit(ltl_guard_max_cubes()) << "\n"; } },
	{ ltl_refinement_rounds_opt, [climit, this]() {
		out << "ltlrefinementrounds: " << climit(ltl_max_refinement_rounds()) << "\n"; } },
	{ ltl_window_max_paths_opt, [climit, this]() {
		out << "ltlwindowmaxpaths:   " << climit(ltl_window_max_paths()) << "\n"; } },
	{ tref_budget_opt, [climit, this]() {
		out << "trefbudget:          " << climit(tref_budget())
			<< " (live: " << api<node>::tref_count() << ")\n"; } },
	{ tref_budget_soft_opt, [this]() {
		out << "trefbudgetsoft:      " << tref_budget_soft_percent()
			<< "%\n"; } }
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
	if (o == none_opt) {
		for (auto& [_, v] : printers) v();
		// Bare `get` also lists the pack's BA-declared options, after the
		// core ones, sorted by family then option name -- a deterministic
		// order independent of pack configuration order, so REPL-output
		// tests never become pack-order-sensitive.
		auto ba_opts = pack_ba_options<node>();
		std::ranges::sort(ba_opts, [](const auto& a, const auto& b) {
			return a.family != b.family ? a.family < b.family
				: std::string(a.option.name)
					< std::string(b.option.name);
		});
		for (const auto& e : ba_opts) {
			out << e.family << "-" << e.option.name << ": ";
			if (e.option.kind == ba_option_kind::flag)
				out << pbool[e.option.get_flag()] << "\n";
			else out << count_limit_str(e.option.get_count()) << "\n";
		}
		return;
	}
	printers[o]();
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::set_cmd(const tt& n) {
	if (auto raw = option_name_str<node>(n);
		raw && raw->find('-') != std::string::npos)
	{
		auto ov = n | tau::option_value;
		if (!ov) { TAU_LOG_ERROR << "Invalid value\n"; return; }
		set_cmd_ba_option(*raw, ov | tt::string);
		get_cmd_ba_option(*raw);
		return;
	}
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
			if (pos != v.size()) {
				TAU_LOG_ERROR << "Invalid value: expected a number\n";
				return {};
			}
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
	{ preprocessing_opt,   [&]() {
		update_preprocessing(update_bool_value(opt.preprocessing)); } },
	{ factoring_opt,   [&]() {
		update_factoring(update_bool_value(opt.factoring)); } },
	{ pwr_semantic_opt, [&]() {
		bool v = pwr_semantic_fallback;
		api<node>::set_pwr_semantic_fallback(update_bool_value(v)); } },
	{ step_prop_opt, [&]() {
		bool v = interpreter<node>::definitional_propagation;
		api<node>::set_step_definitional_propagation(update_bool_value(v)); } },
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
	{ decision_pins_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_ba_decision_pins(*n); } },
	{ cqe_max_clauses_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_cqe_max_clauses(*n); } },
	{ lgrs_max_vars_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_lgrs_max_vars(*n); } },
	{ fixpoint_steps_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_fixpoint_steps(*n); } },
	{ flag_search_steps_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_flag_search_steps(*n); } },
	{ squeeze_cap_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_block_squeeze_cap(*n); } },
	{ simplify_rounds_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_simplify_rounds(*n); } },
	{ def_passes_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_def_passes(*n); } },
	{ enum_steps_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_enum_steps(*n); } },
	{ probe_steps_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_max_probe_steps(*n); } },
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
		api<node>::set_max_cover_products(*n); } },
	{ ltl_timeout_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_ltl_timeout_sec((long) std::min<size_t>(*n,
			(size_t) ltl_timeout_sec_max)); } },
	{ ltl_alg_opt, [&]() {
		std::string a = v;
		for (auto& c : a) c = (char) std::toupper((unsigned char) c);
		if (a != "A" && a != "B" && a != "D" && a != "AUTO") {
			TAU_LOG_ERROR << "Invalid value: expected A, B, D or "
				"auto\n";
			return;
		}
		api<node>::set_ltl_algorithm(a); } },
	{ ltl_qe_max_vars_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_ltl_qe_max_vars(*n); } },
	{ ltl_hoa_max_states_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_ltl_hoa_max_states(*n); } },
	{ ltl_guard_max_cubes_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_ltl_guard_max_cubes(*n); } },
	{ ltl_refinement_rounds_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_ltl_max_refinement_rounds(*n); } },
	{ ltl_window_max_paths_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_ltl_window_max_paths(*n); } },
	{ tref_budget_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_tref_budget(*n); } },
	{ tref_budget_soft_opt, [&]() { if (auto n = str2count(); n)
		api<node>::set_tref_budget_soft_percent(*n); } } };
	setters[o]();
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::update_bool_opt_cmd(const tt& n,
	const std::function<bool(bool&)>& update_fn)
{
	if (auto raw = option_name_str<node>(n);
		raw && raw->find('-') != std::string::npos)
	{
		update_bool_opt_cmd_ba_option(*raw, update_fn);
		if (!error) get_cmd_ba_option(*raw);
		return;
	}
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
	case preprocessing_opt:    update_preprocessing(update_fn(opt.preprocessing)); break;
	case factoring_opt:        update_factoring(update_fn(opt.factoring)); break;
	case pwr_semantic_opt: {
		bool v = pwr_semantic_fallback;
		api<node>::set_pwr_semantic_fallback(update_fn(v)); break;
	}
	case step_prop_opt: {
		bool v = interpreter<node>::definitional_propagation;
		api<node>::set_step_definitional_propagation(update_fn(v)); break;
	}
	case highlighting_opt:     update_fn(pretty_printer_highlighting);break;
	case indenting_opt:        update_fn(pretty_printer_indenting); break;
	case status_opt:           update_fn(opt.status); break;
	case print_benchmarks_opt: update_fn(opt.print_benchmarks); break;
	case block_max_splits_opt:
	case block_max_rounds_opt:
	case cqe_max_clauses_opt:
	case decision_pins_opt:
	case fixpoint_steps_opt:
	case flag_search_steps_opt:
	case squeeze_cap_opt:
	case simplify_rounds_opt:
	case def_passes_opt:
	case enum_steps_opt:
	case probe_steps_opt:
	case rewrite_rounds_opt:
	case gc_min_size_opt:
	case gc_growth_opt:
	case spec_size_warn_opt:
	case revision_alts_opt:
	case consistency_subsets_opt:
	case cache_bound_opt:
	case cover_products_opt:
	case lgrs_max_vars_opt:
		TAU_LOG_ERROR << "This option takes a count, not a flag: use "
			"`set <option> <n>`\n", error = true;
		return;
	default: TAU_LOG_ERROR << "Invalid option\n", error = true; return;
	}
}

template <typename... BAs>
requires BAsPack<BAs...>
const ba_option* repl_evaluator<BAs...>::resolve_ba_option(
	const std::string& family, const std::string& name)
{
	auto res = pack_find_ba_option<node>(family, name);
	switch (res.status) {
	case ba_option_lookup_status::no_such_family:
		TAU_LOG_ERROR << "No BA named '" << family << "' in this pack ("
			<< node::ba::types_joined() << ")\n";
		return nullptr;
	case ba_option_lookup_status::no_such_option:
		TAU_LOG_ERROR << "BA '" << family << "' has no option '" << name
			<< "'\n";
		return nullptr;
	case ba_option_lookup_status::found: return res.option;
	}
	return nullptr; // unreachable: switch above is exhaustive
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::get_cmd_ba_option(const std::string& dotted) {
	static std::string pbool[] = { "off", "on" };
	auto [family, name] = split_ba_option_name(dotted);
	const ba_option* o = resolve_ba_option(family, name);
	if (!o) return;
	out << family << "-" << name << ": "
		<< (o->kind == ba_option_kind::flag
			? pbool[o->get_flag()] : count_limit_str(o->get_count()))
		<< "\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::set_cmd_ba_option(const std::string& dotted,
	const std::string& v)
{
	auto [family, name] = split_ba_option_name(dotted);
	const ba_option* o = resolve_ba_option(family, name);
	if (!o) return;
	if (o->kind == ba_option_kind::flag) {
		if (auto b = ba_option_str2bool(v); b) o->set_flag(*b);
		else TAU_LOG_ERROR << "Invalid value\n";
	} else if (auto n = ba_option_str2count(v); n) o->set_count(*n);
	else TAU_LOG_ERROR << "Invalid value: expected a count\n";
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::update_bool_opt_cmd_ba_option(
	const std::string& dotted, const std::function<bool(bool&)>& update_fn)
{
	auto [family, name] = split_ba_option_name(dotted);
	const ba_option* o = resolve_ba_option(family, name);
	if (!o) return;
	if (o->kind != ba_option_kind::flag) {
		// Same "wrong command shape" error as the core numeric options
		// (block_max_splits_opt and friends, above), just addressed with
		// this option's qualified name instead of a bare one.
		TAU_LOG_ERROR << "This option takes a count, not a flag: use "
			"`set " << family << "-" << name << " <n>`\n";
		error = true;
		return;
	}
	bool v = o->get_flag();
	update_fn(v);
	o->set_flag(v);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool repl_evaluator<BAs...>::update_charvar(bool value) {
	api<node>::set_charvar(opt.charvar = value);
	return value;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool repl_evaluator<BAs...>::update_preprocessing(bool value) {
	api<node>::set_preprocessing(opt.preprocessing = value);
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
		out << "fragment: ltl\n";
	} else if (fnt == tau_parser::fragment_ctl_star) {
		opt.fragment = fragment_ctl_star;
		out << "fragment: ctl_star\n";
	} else {
		TAU_LOG_ERROR << "Unknown fragment. Available: ltl, ctl_star\n";
		error = true;
	}
}

template <typename... BAs>
requires BAsPack<BAs...>
bool repl_evaluator<BAs...>::update_factoring(bool value) {
	api<node>::set_ba_component_factoring(opt.factoring = value);
	return value;
}

template <typename... BAs>
requires BAsPack<BAs...>
int repl_evaluator<BAs...>::eval_cmd(const tt& n) {
	auto command = n | tt::only_child;
	auto command_type = command | tt::nt;
	// Refuse before the command runs, not after: a command allowed to
	// start keeps its result. This covers the commands that do not go
	// through the api -- anf_cmd today, and whatever is added next --
	// while the api's own guard covers the rest; the check is a cheap
	// measurement, so doing it twice costs nothing.
	//
	// The control commands are exempt, and have to be: they are how a
	// session at its budget recovers. Gating `quit` strands the user in a
	// REPL they cannot leave, and gating `set`/`clear` takes away the two
	// ways to get back under the cap.
	switch (command_type) {
	case tau::quit_cmd: case tau::clear_cmd: case tau::help_cmd:
	case tau::version_cmd: case tau::get_cmd: case tau::set_cmd:
	case tau::enable_cmd: case tau::disable_cmd: case tau::toggle_cmd:
	case tau::reset_cmd: case tau::comment:
		break;
	default:
		if (over_tref_budget<node>()) {
			error = true;
			TAU_LOG_ERROR << tref_budget_message<node>();
			return 0;
		}
	}
	budget_scope<node> budget;
#ifdef DEBUG
	if (opt.debug_repl) {
		// out << "command: " << command << "\n";
		command.value_tree().print_tree(out << "tree: ") << "\n";
	}
#endif // DEBUG
	tref result = 0;
	switch (command_type) {
	case tau::quit_cmd:           return out << "Quit.\n", 1;
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
	// definition of ADT types
	case tau::def_type_cmd:       def_type_cmd(command); break;
	// qelim
	case tau::qelim_cmd:          result = qelim_cmd(command); break;
	// type inspection and session management
	case tau::whatis_cmd:         result = whatis_cmd(command); break;
	case tau::reset_cmd:          reset_cmd(); break;
	case tau::comment:            break;
	// error handling
	default: error = true; out << std::endl;
		TAU_LOG_ERROR << "Unknown command";
	}
#ifdef DEBUG
	if (opt.debug_repl && result) tau::get(result).print_tree(
		out << "result tree: ") << "\n";
#endif // DEBUG
	if (result) history_store(result);
	return 0;
}

template <typename... BAs>
requires BAsPack<BAs...>
repl_evaluator<BAs...>::repl_evaluator(options opt, std::ostream& out,
	std::ostream& err): out(out), err(err), opt(opt)
{
	TC.set(opt.colors);
	logging::set_filter(opt.severity);
	if (opt.experimental) out << "\n!!! Experimental features "
		"enabled (expect unstable behavior) !!!\n\n";
	// Propagate the CLI-provided charvar/preprocessing values to the api's
	// global state; without this, --charvar/--preprocessing have no effect
	// in REPL mode until the user runs "set"/"toggle" (they were only ever
	// applied to the api in main.cpp's non-interactive spec-file path).
	update_charvar(opt.charvar);
	update_preprocessing(opt.preprocessing);
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
		// A tuple-typed console stream prompts for whole wire literals
		// ({ ... }); a bare q/quit can never be one (req.type_tree is null
		// exactly for tuple prompts -- see continue_running), so accept it
		// as "end the run" here too. Without this, a piped script has no
		// way to leave an always-constrained tuple run: q would be fed to
		// the wire parser, rejected, and re-prompted until end-of-file.
		// Plain streams are untouched -- there a bare q could be a value.
		bool stop = run_abort_
			|| ((req.kind == pending_request::continue_or_quit
				|| (req.kind == pending_request::stream_value
					&& !req.type_tree))
				&& (src == "q" || src == "quit"));
		if (!run_abort_ && !stop
			&& req.kind == pending_request::stream_value)
			req.stream->set(src);
		run_abort_ = false;
		if (stop) finish_running();
		else {
			if (req.kind == pending_request::stream_value)
				continue_running(req);
			else continue_running();
		}
		out << "\n", out.flush();
		if (!pending) reprompt();
		return idni::diagnostics::result<int>(0);
	}
	error = false;
	// make_cli() already prints its own report (see its own comment);
	// eval()'s result<int> return carries REPL quit codes, not a report.
	tref cli = make_cli(src).value_or(nullptr);
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
	} else if (!error) return idni::diagnostics::result<int>(2);
	out << "\n", out.flush();
	if (error && opt.error_quits)
		return idni::diagnostics::result<int>(quit = 1);
	if (quit == 0) reprompt();
	return idni::diagnostics::result<int>(quit);
}

template <typename... BAs>
requires BAsPack<BAs...>
void repl_evaluator<BAs...>::version_cmd() {
	out << full_version << "\n"
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
		"  preprocessing (B)      BA preprocessing (e.g. bv blasting)  on/off\n"
		"  factoring              tau-algebra component factoring      on/off\n"
		"  pwrsemantic (Z)        semantic pointwise-revision fallback on/off\n"
		"  stepprop               step definitional propagation        on/off\n"
		"  benchmarks (b)         print timing benchmarks              on/off\n";
	static const std::string numeric_options =
		"and the numeric limit options, set with `set <option> <n>` "
		"(0 = unlimited;\nspecsizewarn: 0 = off; gcgrowth <= 0 disables "
		"gc; each mirrors the CLI option\nof the same meaning):\n"
		"  <option>               <description>                        <default>\n"
		"  maxsplits              anti-prenex per-block Boole splits   unlimited\n"
		"  maxrounds              anti-prenex driver rounds            unlimited\n"
		"  maxclauses             cqe DNF clauses per distributed scope unlimited\n"
		"  lgrsmaxvars            pure-equality variables on lgrs route 8\n"
		"  decisionpins           decided tau-algebra rows kept alive  4096\n"
		"  fixpointsteps          temporal-normalization fixpoint steps 500\n"
		"  flagsteps              eventual-flag search steps           500\n"
		"  squeezecap             block-squeeze operand-set size cap   unlimited\n"
		"  simplifyrounds         bitvector simplification rounds      unlimited\n"
		"  defpasses              definition-expansion passes          unlimited\n"
		"  enumsteps              recurrence enumeration steps         unlimited\n"
		"  probesteps             untyped recurrence probe steps       10000\n"
		"  rewriterounds          rewrite-to-fixpoint rounds           unlimited\n"
		"  gcminsize              gc trigger floor (tree nodes)        256\n"
		"  gcgrowth               gc growth-factor trigger (decimal)   1.5\n"
		"  trefbudget             live interned tree nodes allowed     unlimited\n"
		"  trefbudgetsoft         % of trefbudget that forces a sweep  75\n"
		"  specsizewarn           updated-spec size warning (chars)    off\n"
		"  revisionalts           revision alternatives kept per part  unlimited\n"
		"  maxsubsets             k-ary consistency subset checks      4096\n"
		"  cachebound             string-keyed synthesis cache bound   4096\n"
		"  maxcoverproducts       oracle mixed-type coverage products  256\n"
		"  ltltimeout             ltlsynt watchdog in seconds (0 = off) 60\n"
		"  ltlalg                 omcat synthesis algorithm A/B/D/auto auto\n"
		"  ltlqemaxvars           omcat QE fast-path free-variable cap 2\n"
		"  ltlhoamaxstates        accepted ltlsynt strategy states     4194304\n"
		"  ltlguardmaxcubes       Algorithm D guard DNF cubes          512\n"
		"  ltlrefinementrounds    ABA-oracle refinement rounds         64\n"
		"  ltlwindowmaxpaths      window-oracle paths per check        4096\n";
	// BA-declared options ("family-option"), sorted by family then option
	// name for a deterministic listing independent of pack configuration
	// order. Flags join the enable/disable/toggle-eligible list; counts
	// (no enable/disable/toggle, same as core's numeric limit options) join
	// only the get/set list. Both are empty in a pack where no BA declares
	// any option, leaving all_available_options/bool_available_options
	// byte-identical to before.
	auto sorted_ba_options = [](ba_option_kind kind) {
		auto opts = pack_ba_options<node>();
		std::vector<ba_named_option> out;
		for (auto& e : opts) if (e.option.kind == kind) out.push_back(e);
		std::ranges::sort(out, [](const auto& a, const auto& b) {
			return a.family != b.family ? a.family < b.family
				: std::string(a.option.name)
					< std::string(b.option.name);
		});
		std::string s;
		for (auto& e : out) {
			std::string label = "  " + e.family + "-" + e.option.name;
			s += label + std::string(label.size() < 24
				? 24 - label.size() : 1, ' ') + e.option.help
				+ (kind == ba_option_kind::flag ? "   on/off\n" : "\n");
		}
		return s;
	};
	const std::string ba_flag_options = sorted_ba_options(ba_option_kind::flag);
	const std::string ba_count_options = sorted_ba_options(ba_option_kind::count);
	const std::string all_available_options = std::string{} +
		"Available options and values:\n" + bool_options + ba_flag_options +
		"  severity               severity                             error/info/debug/trace\n"
		+ numeric_options + ba_count_options;
	const std::string bool_available_options = std::string{} +
		"Available options and values:\n" + bool_options + ba_flag_options;
	switch (nt) {
	case tau::help_sym: out
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
		<< "  nnf                     convert a Tau expression to negation normal form\n"
		<< "  onf                     convert a Tau formula to order normal form\n"
		<< "\n"

		<< "History and definitions:\n"
		<< "  history or hist         show all Tau expressions stored in the repl history\n"
		<< "  definitions or defs     show stored IO variables and function and predicate definitions\n"
		<< "  reset                   reset the session and free unused memory\n"
		<< "\n"

		<< "Inspection commands:\n"
		<< "  whatis                  show the inferred type of a Tau expression\n"
		<< "\n"

		<< "Substitution and instantiation command:\n"
		<< "  substitute, subst or s  substitute one or more Tau expressions in a Tau expression by others\n"
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
	case tau::version_sym: out
		<< "version prints the current Tau version\n";
		break;
	case tau::quit_sym: out
		<< "quit exits the Tau repl\n";
		break;
	case tau::clear_sym: out
		<< "clear clears the terminal screen\n"
		<< "\n"
		<< "usage:\n"
		<< "  clear or c              clears the terminal screen\n";
		break;
	case tau::get_sym: out
		<< "get                       prints all options and their values\n"
		<< "get <option>              prints the value of the given option\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau::set_sym: out
		<< "set <option> [=] <value> sets option to value\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau::enable_sym: out
		<< "enable <option>           enables option\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau::disable_sym: out
		<< "disable <option>          disables option\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau::toggle_sym: out
		<< "toggle <option>           toggles option value\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau::history_sym: out
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
	case tau::normalize_sym: out
		<< "the normalize command normalizes a Tau expression, prints the result and\n"
		<< "saves it into the repl history\n"
		<< "\n"
		<< "usage:\n"
		<< "  normalize <rr>            normalizes the given tau formula with additional predicate and function definitions\n"
		<< "  normalize <tau>           normalizes the given tau formula\n"
		<< "  normalize <term>          normalizes the given term\n"
		<< "  normalize <repl_history>  normalizes the Tau expression stored at the specified repl history position\n";
		break;
	case tau::qelim_sym: out
		<< "the qelim command eliminates all non-temporal quantifiers, prints the result and\n"
		<< "saves it into the repl history\n"
		<< "\n"
		<< "usage:\n"
		<< "  qelim <tau>             eliminates non-temporal quantifiers in the given tau formula\n"
		<< "  qelim <repl_history>    eliminates non-temporal quantifiers in the Tau formula stored at the specified repl history position\n";
		break;
	case tau::ltl_sym: out
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
	case tau::run_sym: out
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
	case tau::stop_sym: out
		<< "the stop command discards the stored run session\n\n"
		<< "a run started with `run N steps` keeps its session so that a later `run` or\n"
		<< "`run N steps` can continue it; stop clears that session\n"
		<< "\n"
		<< "usage:\n"
		<< "  stop                    discard the stored run session\n";
		break;
	case tau::fragment_sym: out
		<< "the fragment command selects the grammar fragment used to parse formulas\n\n"
		<< "usage:\n"
		<< "  fragment ltl            default LTL fragment (F, G, U, R, W, S, T)\n"
		<< "  fragment ctl_star       CTL* fragment, adds the A, E and - operators\n";
		break;
	case tau::memory_sym: out
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
	case tau::solve_sym: out
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
	case tau::lgrs_sym: out
		<< "the lgrs command computes a LGRS for an equation\n"
		<< "\n"
		<< "usage:\n"
		<< "  lgrs <tau_eq>           computes a LGRS for a given equation\n"
		<< "  lgrs <repl_history>     computes a LGRS for the equation stored at the specified repl history position\n"
		<< "options:\n"
		<< "  --<type>                uses the specified type for the solution\n"
		<< "\n";
		break;
	case tau::whatis_sym: out
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
	case tau::reset_sym: out
		<< "the reset command starts the REPL session afresh\n"
		<< "\n"
		<< "it stops a run in progress, clears the history, the definitions,\n"
		<< "the IO streams and the caches, and frees the tree nodes nothing\n"
		<< "uses any more; options keep the values they were set to\n"
		<< "\n"
		<< "usage:\n"
		<< "  reset                   resets the session\n";
		break;
	case tau::sat_sym: out
		<< "the sat command checks if a Tau formula is satisfiable and if so prints T and else F\n\n"
		<< "a tau formula is satisfiable if there exists a variable assignment to non-temporal variables\n"
		<< "such that for all possible inputs there exist time compatible outputs at each point in time\n"
		<< "\n"
		<< "usage:\n"
		<< "  sat <rr>                checks the given tau formula with additional predicate and function definitions for satisfiability\n"
		<< "  sat <tau>               checks the given tau formula for satisfiability\n"
		<< "  sat <repl_history>      checks the Tau formula stored at the specified repl history position for satisfiability\n";
		break;
	case tau::valid_sym: out
		<< "the valid command checks if a Tau formula is logically equivalent to T and if so prints T and else F\n"
		<< "\n"
		<< "usage:\n"
		<< "  valid <rr>              checks the given tau formula with additional predicate and function definitions for validity\n"
		<< "  valid <tau>             checks the given tau formula for validity\n"
		<< "  valid <repl_history>    checks the Tau formula stored at the specified repl history position for validity\n";
		break;
	case tau::unsat_sym: out
		<< "the unsat command checks if a Tau formula is unsatisfiable and if so prints T and else F\n\n"
		<< "a tau formula is unsatisfiable if for every variable assignment to non-temporal variables\n"
		<< "there exist inputs such that there are no time compatible outputs at some point in time\n"
		<< "\n"
		<< "usage:\n"
		<< "  unsat <rr>              checks the given tau formula with additional predicate and function definitions for unsatisfiability\n"
		<< "  unsat <tau>             checks the given tau formula for unsatisfiability\n"
		<< "  unsat <repl_history>    checks the Tau formula stored at the specified repl history position for unsatisfiability\n";
		break;
	case tau::realizable_sym: out
		<< "the realizable command checks if a Tau specification is realizable and if so prints T and else F\n\n"
		<< "a tau specification is realizable if there exists a winning system strategy that, for\n"
		<< "every possible sequence of inputs, produces outputs satisfying the specification at every point in time\n"
		<< "\n"
		<< "usage:\n"
		<< "  realizable <rr>              checks the given tau formula with additional predicate and function definitions for realizability\n"
		<< "  realizable <tau>             checks the given tau formula for realizability\n"
		<< "  realizable <repl_history>    checks the Tau formula stored at the specified repl history position for realizability\n";
		break;
	case tau::unrealizable_sym: out
		<< "the unrealizable command checks if a Tau specification is unrealizable and if so prints T and else F\n\n"
		<< "a tau specification is unrealizable if no system strategy exists that, for every possible\n"
		<< "sequence of inputs, produces outputs satisfying the specification at every point in time\n"
		<< "\n"
		<< "usage:\n"
		<< "  unrealizable <rr>              checks the given tau formula with additional predicate and function definitions for unrealizability\n"
		<< "  unrealizable <tau>             checks the given tau formula for unrealizability\n"
		<< "  unrealizable <repl_history>    checks the Tau formula stored at the specified repl history position for unrealizability\n";
		break;
	case tau::dnf_sym: out
		<< "dnf converts a Tau expression to disjunctive normal form (DNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  dnf <term>              converts the given term to DNF\n"
		<< "  dnf <tau>               converts the given Tau formula to DNF\n"
		<< "  dnf <repl_history>      converts the Tau expression stored at the specified repl history position to DNF\n";
		break;
	case tau::cnf_sym: out
		<< "cnf converts a Tau expression to conjunctive normal form (CNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  cnf <term>              converts the given term to CNF\n"
		<< "  cnf <tau>               converts the given Tau formula to CNF\n"
		<< "  cnf <repl_history>      converts the Tau expression stored at the specified repl history position to CNF\n";
		break;
	case tau::nnf_sym: out
		<< "nnf converts a Tau expression to negation normal form (NNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  nnf <term>              converts the given term to NNF\n"
		<< "  nnf <tau>               converts the given tau formula to NNF\n"
		<< "  nnf <repl_history>      converts the Tau expression stored at the specified repl history position to NNF\n";
		break;
	case tau::mnf_sym: out
		<< "mnf converts a Tau expression to minterm normal form (MNF)\n"
		<< "\n"
		<< "usage:\n"
		<< "  mnf <term>              converts the given term to MNF\n"
		<< "  mnf <tau>               converts the given tau formula to MNF\n"
		<< "  mnf <repl_history>      converts the Tau expression stored at the specified repl history position to MNF\n";
		break;
	case tau::onf_sym: out
		<< "onf converts a tau formula to order normal form (ONF) on the specified variable\n"
		<< "\n"
		<< "usage:\n"
		<< "  onf <var> <tau>           converts the given tau formula to ONF using <var>\n"
		<< "  onf <var> <repl_history>  converts the Tau formula stored at the specified repl history position to ONF using <var>\n";
		break;
	case tau::subst_sym: out
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
	case tau::inst_sym: out
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
	case tau::def_sym: out
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
	case tau::examples_sym: out
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
	default: out
		<< "no help available for this command; `help` lists the commands\n";
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
