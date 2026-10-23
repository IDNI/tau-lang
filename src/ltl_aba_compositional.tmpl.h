// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md
//
// Compositional decision of a conjunction (ltl-compositional): the parts of
// a conjunction that share no output are decided, and run, one by one.

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ltl_aba"

namespace idni::tau_lang {

/** @internal @copydoc ltl_output_groups @endinternal */
template <NodeType node>
trefs ltl_output_groups(tref fm) {
	using tau = tree<node>;
	if (!fm || has_ctl_star_operators<node>(fm)
		|| tau::get(fm).find_top(is<node, tau::ref>)) return {};
	auto peel = [](tref n) {
		while (tau::get(n).child_is(tau::wff_parenthesis))
			n = tau::get(n)[0].first();
		return n;
	};
	auto io_vars = [](tref n) {
		return tau::get(n).select_top(is_child<node, tau::io_var>);
	};
	// The clauses, as ltl_skeleton_with_testers splits them, each started
	// at its deepest lookback.
	trefs clauses;
	std::function<void(tref)> split = [&](tref n) {
		if (const auto& t = tau::get(n); t.child_is(tau::wff_and)) {
			split(t[0].first());
			split(t[0].second());
		} else clauses.push_back(n);
	};
	split(fm);
	// A piece is a clause, or a conjunct of the body of an always clause,
	// which then keeps the start of its clause.
	struct piece { tref fm; int_t start; bool in_always; };
	std::vector<piece> pieces;
	for (tref clause : clauses) {
		const auto ks = collect_step_guards<node>(clause);
		const int_t k = std::max(ks.empty() ? 0 : *ks.rbegin(),
			body_max_lookback<node>(clause));
		// a clause outside any temporal operator reads fixed steps only
		if (!tau::get(clause).find_top(is_temporal_quantifier<node>))
			for (tref v : io_vars(clause))
				if (!is_io_initial<node>(v)) return {};
		const auto& t = tau::get(peel(clause));
		if (!t.child_is(tau::wff_always)) {
			pieces.push_back({ clause, k, false });
			continue;
		}
		std::function<void(tref)> body = [&](tref n) {
			if (const auto& b = tau::get(peel(n)); b.child_is(tau::wff_and)) {
				body(b[0].first());
				body(b[0].second());
			} else pieces.push_back({ n, k, true });
		};
		body(t[0].first());
	}
	// The pieces joined by a shared output; inputs and the warm-up marker
	// of pin_written_warm_ups join none.
	std::vector<size_t> parent(pieces.size());
	std::iota(parent.begin(), parent.end(), 0);
	std::function<size_t(size_t)> find = [&](size_t i) {
		return parent[i] == i ? i : parent[i] = find(parent[i]);
	};
	std::map<std::string, size_t> owner;
	std::vector<bool> marker_only(pieces.size(), false);
	for (size_t i = 0; i < pieces.size(); ++i) {
		bool marker_read = false, other_read = false;
		for (tref v : io_vars(pieces[i].fm)) {
			const std::string name = get_var_name<node>(v);
			if (name == "o__warmup") { marker_read = true; continue; }
			other_read = true;
			if (tau::get(v).is_input_variable()) continue;
			auto [it, fresh] = owner.emplace(name, i);
			if (!fresh) parent[find(i)] = find(it->second);
		}
		// a piece that is the marker alone gives way to the anchor below
		marker_only[i] = pieces[i].in_always && marker_read && !other_read;
	}
	std::map<size_t, std::vector<size_t>> groups;
	for (size_t i = 0; i < pieces.size(); ++i)
		if (!marker_only[i]) groups[find(i)].push_back(i);
	if (groups.size() < 2) return {};
	use_hooks_guard<node> hooks_off(false);
	// `x[t-k] = x[t-k]` on a stream x of @p fm: the literal that starts a
	// clause at step k, built without the hooks, which would fold it
	auto anchor = [&](tref fm, int_t k) -> tref {
		for (tref v : io_vars(fm)) {
			if (is_io_initial<node>(v)) continue;
			const size_t type = tau::get(v).get_ba_type();
			tref name = get_var_name_node<node>(v);
			tref x = tau::get(v).is_input_variable()
				? build_in_var_at_t_minus<node>(name, (size_t)k, type)
				: build_out_var_at_t_minus<node>(name, (size_t)k, type);
			return tau::build_bf_eq(x, x);
		}
		return nullptr;
	};
	auto conjunction = [](const trefs& fms) {
		tref c = fms.front();
		for (size_t i = 1; i < fms.size(); ++i)
			c = tau::build_wff_and(c, fms[i]);
		return c;
	};
	trefs out;
	for (const auto& [_, members] : groups) {
		trefs part;
		for (size_t i : members) {
			const piece& p = pieces[i];
			if (!p.in_always) { part.push_back(p.fm); continue; }
			// A piece starts where its clause did; one reading fixed
			// steps only holds at every step or at none.
			tref b = p.fm;
			if (body_max_lookback<node>(b) < p.start)
				if (tref a = anchor(b, p.start))
					b = tau::build_wff_and(b, a);
			part.push_back(tau::build_wff_always(b));
		}
		out.push_back(conjunction(part));
	}
	return out;
}

} // namespace idni::tau_lang
